#include "opcode.h"
#include <cassert>
#include "actions.h"
#include "compiler/string_scanner.h"

namespace Megalo {
   void OpcodeBase::decompile(Decompiler& out, std::vector<OpcodeArgValue*>& args) const noexcept {
      auto& mapping = this->mapping;
      switch (mapping.type) {
         case OpcodeFuncToScriptMapping::mapping_type::none:
            out.write("nop");
            return;
         case OpcodeFuncToScriptMapping::mapping_type::assign:
            args[0]->decompile(out); // target
            out.write(' ');
            args[2]->decompile(out); // operator
            out.write(' ');
            args[1]->decompile(out); // source
            return;
         case OpcodeFuncToScriptMapping::mapping_type::compare:
            args[0]->decompile(out); // lhs
            out.write(' ');
            args[2]->decompile(out); // operator
            out.write(' ');
            args[1]->decompile(out); // rhs
            return;
         case OpcodeFuncToScriptMapping::mapping_type::function:
            {  // result
               auto&  list  = this->arguments;
               size_t count = list.size();
               for (size_t i = 0; i < count; ++i) {
                  if (list[i].is_out_variable) {
                     args[i]->decompile(out);
                     out.write(" = ");
                     break;
                  }
               }
            }
            if (mapping.arg_context != OpcodeFuncToScriptMapping::no_context) {
               auto ac = mapping.arg_context;
               if (ac == OpcodeFuncToScriptMapping::game_namespace) {
                  out.write("game");
               } else {
                  args[ac]->decompile(out, Decompiler::flags::is_call_context);
               }
               out.write('.');
            }
            out.write(mapping.primary_name.c_str());
            out.write('(');
            for (size_t i = 0; i < std::extent<decltype(OpcodeFuncToScriptMapping::arg_index_mappings)>::value; ++i) {
               auto mapped_index = mapping.arg_index_mappings[i];
               if (mapped_index < 0)
                  break;
               if (i > 0)
                  out.write(", ");
               args[mapped_index]->decompile(out);
            }
            out.write(')');
            return;
         case OpcodeFuncToScriptMapping::mapping_type::property_get:
            {  // result
               auto&  list  = this->arguments;
               size_t count = list.size();
               for (size_t i = 0; i < count; ++i) {
                  if (list[i].is_out_variable) {
                     args[i]->decompile(out);
                     break;
                  }
               }
            }
            out.write(" = ");
            if (mapping.arg_context != OpcodeFuncToScriptMapping::no_context) {
               auto ac = mapping.arg_context;
               if (ac == OpcodeFuncToScriptMapping::game_namespace) {
                  out.write("game");
               } else {
                  args[ac]->decompile(out);
               }
               out.write('.');
            }
            out.write(mapping.primary_name.c_str());
            return;
         case OpcodeFuncToScriptMapping::mapping_type::property_set:
            args[mapping.arg_context]->decompile(out);
            out.write('.');
            if (mapping.primary_name.empty()) {
               assert(mapping.arg_name >= 0);
               args[mapping.arg_name]->decompile(out);
            } else {
               out.write(mapping.primary_name.c_str());
            }
            out.write(' ');
            args[mapping.arg_operator]->decompile(out);
            out.write(' ');
            {  // operand
               //
               // We don't explicitly specify the operand index; we instead deduce it as the 
               // argument index that isn't being used for any other role.
               //
               auto& list = this->arguments;
               size_t count = list.size();
               for (size_t i = 0; i < count; ++i) {
                  if (i != mapping.arg_context && i != mapping.arg_operator) {
                     if (mapping.primary_name.empty()) {
                        if (i == mapping.arg_name)
                           continue;
                     }
                     args[i]->decompile(out);
                     break;
                  }
               }
            }
            return;
      }
   }

   AccessorRegistry::AccessorRegistry() {
      using mapping_type = OpcodeFuncToScriptMapping::mapping_type;
      //
      // Build the registry.
      //
      for (auto& action : actionFunctionList) {
         auto& mapping = action.mapping;
         if (mapping.type != mapping_type::property_get && mapping.type != mapping_type::property_set)
            continue;
         Definition* entry = nullptr;
         if (mapping.primary_name.empty() && mapping.arg_name != OpcodeFuncToScriptMapping::no_argument) {
            auto& base = action.arguments[mapping.arg_name];
            entry = this->get_variably_named_property(base.typeinfo);
            if (!entry)
               entry = &this->definitions.emplace_back();
         } else {
            entry = this->get_by_name(mapping.primary_name.c_str());
            if (!entry)
               entry = &this->definitions.emplace_back(mapping.primary_name.c_str());
         }
         if (mapping.type == mapping_type::property_get)
            entry->getter = &action;
         else
            entry->setter = &action;
      }
   }
   AccessorRegistry::Definition* AccessorRegistry::get_by_name(const char* name) const noexcept {
      for (auto& entry : this->definitions) {
         if (entry.name == name)
            return (Definition*)&entry; // cast needed to strip const
      }
      return nullptr;
   }
   AccessorRegistry::Definition* AccessorRegistry::get_variably_named_accessor(const OpcodeArgTypeinfo& property_name_type) const noexcept {
      for (auto& entry : this->definitions) {
         if (!entry.name.empty())
            continue;
         const OpcodeBase* function = nullptr;
         if (entry.getter)
            function = entry.getter;
         else
            function = entry.setter;
         #if _DEBUG
            assert(function && "An entry for a variably-named accessor should not exist unless we've identified at least one of its opcodes.");
         #endif
         auto& mapping = function->mapping;
         if (mapping.arg_name != OpcodeFuncToScriptMapping::no_argument)
            continue;
         auto& base = function->arguments[mapping.arg_name];
         if (&base.typeinfo == &property_name_type)
            return (Definition*)&entry; // cast needed to strip const
      }
      return nullptr;
   }
   const AccessorRegistry::Definition* AccessorRegistry::get_variably_named_accessor(Compiler& compiler, const QString& name, const OpcodeArgTypeinfo& property_is_on) const noexcept {
      //
      // If a property-getter or property-setter has no defined primary name, then one of the opcode arguments 
      // is the name. The easiest way to test against such getters/setters is to simply instantiate that 
      // argument, pass the current property name to its compile method, and see if that succeeds. I know that 
      // that's kinda ugly. Maybe we can alter the code to retain a successfully-compiled argument for use in 
      // the Opcode we'll eventually create.
      //
      std::unique_ptr<OpcodeArgValue> arg;
      for (auto& entry : this->definitions) {
         if (!entry.is_variably_named())
            continue;
         const OpcodeBase* function = nullptr;
         if (entry.getter)
            function = entry.getter;
         else
            function = entry.setter;
         //
         auto& mapping = function->mapping;
         if (&function->arguments[mapping.arg_context].typeinfo != &property_is_on) // validate context type
            continue;
         auto& base = function->arguments[mapping.arg_name];
         auto& type = base.typeinfo;
         //
         arg.reset((type.factory)());
         if (arg->compile(compiler, Script::string_scanner(name)) == arg_compile_result::success)
            return &entry;
      }
      return nullptr;
   }
}