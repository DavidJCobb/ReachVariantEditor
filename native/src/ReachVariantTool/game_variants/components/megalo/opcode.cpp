#include "opcode.h"
#include <cassert>

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
                  args[ac]->decompile(out);
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
}