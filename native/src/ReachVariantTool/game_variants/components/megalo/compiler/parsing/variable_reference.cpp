#include "variable_reference.h"
#include "../compiler.h"
#include "../namespaces.h"
#include "../../variables_and_scopes.h"
#include "../../opcode_arg_types/variables/all_core.h"
#include "../../../../helpers/qt/string.h"

namespace {
   const Megalo::VariableScope* _var_scope_for_type(const Megalo::OpcodeArgTypeinfo& type) {
      //
      // Only variable types that can be scopes will have a (which) value, and the list 
      // of such values is accessible via Megalo::VariableScope::list.
      //
      if (&type == &Megalo::OpcodeArgValueObject::typeinfo)
         return &Megalo::MegaloVariableScopeObject;
      if (&type == &Megalo::OpcodeArgValuePlayer::typeinfo)
         return &Megalo::MegaloVariableScopePlayer;
      if (&type == &Megalo::OpcodeArgValueTeam::typeinfo)
         return &Megalo::MegaloVariableScopeTeam;
      return nullptr;
   }
   Megalo::variable_type _var_type_constant_for_type(const Megalo::OpcodeArgTypeinfo& type) {
      if (&type == &Megalo::OpcodeArgValueScalar::typeinfo)
         return Megalo::variable_type::scalar;
      if (&type == &Megalo::OpcodeArgValueObject::typeinfo)
         return Megalo::variable_type::object;
      if (&type == &Megalo::OpcodeArgValuePlayer::typeinfo)
         return Megalo::variable_type::player;
      if (&type == &Megalo::OpcodeArgValueTeam::typeinfo)
         return Megalo::variable_type::team;
      if (&type == &Megalo::OpcodeArgValueTimer::typeinfo)
         return Megalo::variable_type::timer;
      return Megalo::variable_type::not_a_variable;
   }
}
namespace Megalo::Script {
   VariableReference::Part& VariableReference::Part::operator=(const VariableReference::Part& other) noexcept {
      this->name      = other.name;
      this->index     = other.index;
      this->index_str = other.index_str;
      this->index_is_numeric = other.index_is_numeric;
      return *this;
   }
   void VariableReference::Part::resolve_index(Compiler& compiler) {
      if (this->index_is_numeric)
         return;
      auto abs = compiler.lookup_absolute_alias(this->index_str);
      if (!abs)
         throw compile_exception(QString("\"%1\" is not a valid index.").arg(this->index_str));
      if (!abs->is_integer_constant())
         throw compile_exception(QString("Alias \"%1\" is not an integer constant and therefore cannot be used as an index.").arg(abs->name));
      this->index = abs->get_integer_constant();
      this->index_is_numeric = true;
      this->index_str.clear();
   }

   VariableReference::VariableReference(int32_t i) {
      this->set_to_constant_integer(i);
      #if _DEBUG
         this->content = QString("%1").arg(i);
      #endif
   }
   VariableReference::VariableReference(QString text) {
      #if _DEBUG
         this->content = text;
      #endif
      {
         bool ok = false;
         int32_t v = text.toInt(&ok);
         if (ok) {
            this->set_to_constant_integer(v);
            return;
         }
      }
      //
      // The text isn't an integer literal, so we need to split it up into parts, dividing it at 
      // each '.' glyph.
      //
      auto    size  = text.size();
      QString name;
      QString index;
      bool    is_index = false;
      //
      static const QString BAD_CHARS = QString("[.=!<>+-*/%&|~^");
      //
      for (int i = 0; i < size; i++) {
         auto c = text[i];
         if (is_index) {
            if (BAD_CHARS.indexOf(c) >= 0) {
               if (index.isEmpty() && c == '-') { // negative indices are invalid but should not be a fatal parse error
                  index += c;
                  continue;
               }
               throw compile_exception(QString("Character %1 is not valid inside of an index.").arg(c));
            }
            if (c == ']') {
               is_index = false;
               if (index.isEmpty()) // "name[]" is a syntax error
                  throw compile_exception("Variables of the form \"name[]\" are not valid. Specify an index if appropriate, or no square brackets at all otherwise.");
               continue;
            }
            if (index.isEmpty() && string_scanner::is_whitespace_char(c))
               continue;
            index += c;
         } else {
            if (c == ']')
               throw compile_exception("Unexpected \"]\".");
            if (c == '[') {
               if (cobb::qt::string_is_integer(name)) { // "123[4]" is a syntax error
                  if (this->parts.empty())
                     throw compile_exception("An integer literal cannot be indexed.");
                  throw compile_exception("An integer literal cannot be a property or nested variable."); // "name.123" is also a syntax error and should take priority
               }
               if (!index.isEmpty())
                  throw compile_exception("Variables of the form \"name[1][2]\" are not valid. Only specify one index.");
               is_index = true;
               continue;
            }
            if (c == '.') {
               if (cobb::qt::string_is_integer(name)) { // "1234.name" and "name.1234.name" are syntax errors
                  if (this->parts.empty())
                     throw compile_exception("An integer literal cannot have properties or nested variables."); // 1234.name"
                  throw compile_exception("An integer literal cannot be a property or nested variable."); // "name.1234.name"
               }
               auto part = Part(name, index);
               this->parts.push_back(part);
               name  = "";
               index = "";
               continue;
            }
            if (!index.isEmpty()) // "name[1]name" is a syntax error
               throw compile_exception("Expected a period. Variables of the form \"name[1]name\" are not valid.");
            name += c;
         }
      }
      //
      // We've reached the end of the string. We were only saving parts when we encountered a '.', so the 
      // last part in the string still needs to be validated and added.
      //
      {
         if (name.isEmpty()) // text ended in '.' or was empty string
            throw compile_exception("Variables cannot end in '.', and cannot be nameless.");
         if (cobb::qt::string_is_integer(name)) {
            if (this->parts.empty())
               //
               // If there are no earlier parts, then this must have been something like "123[4]". If there 
               // weren't an index, then the entire string would just be the integer and we would've treated 
               // it accordingly at the very start of this function.
               //
               throw compile_exception("An integer literal cannot be indexed.");
            throw compile_exception("An integer literal cannot be a property or nested variable.");
         }
         if (is_index)
            throw compile_exception("Expected a closing square bracket."); // "name[1].name[2" is a syntax error
         auto part = Part(name, index);
         this->parts.push_back(part);
      }
      //
      // Next, let's do some final basic validation -- specifically, disallow the use of keywords as part 
      // names and indices, disallow empty part names, and store integer indices as integers.
      //
      for (auto& part : this->parts) {
         if (part.name.isEmpty())
            throw compile_exception("Invalid variable name. Did you accidentally type two periods instead of one?");
         if (Compiler::is_keyword(part.name))
            throw compile_exception(QString("Keywords, including \"%1\", cannot be used as variable names.").arg(part.name));
         if (Compiler::is_keyword(part.index_str))
            throw compile_exception(QString("Keywords, including \"%1\", cannot be used as indices in a collection.").arg(part.index_str));
         //
         bool ok = false;
         part.index = part.index_str.toInt(&ok);
         part.index_is_numeric = ok;
         if (ok)
            part.index_str = "";
      }
   }

   void VariableReference::set_to_constant_integer(int32_t v) {
      this->interpreted.clear();
      InterpretedPart part;
      part.type          = nullptr;
      part.disambiguator = v;
      part.disambig_type = disambig_type::constant_integer;
      this->interpreted.push_back(part);
      this->resolved = true;
   }

   size_t VariableReference::_resolve_first_parts(Compiler& compiler) {
      size_t i    = 0;
      auto   part = this->_part(i);
      {
         auto abs = compiler.lookup_absolute_alias(part->name);
         if (abs) {
            if (part->has_index())
               throw compile_exception(QString("Aliases such as \"%1\" can refer to indexed data but cannot themselves be indexed.").arg(abs->name));
            this->_transclude(i, *abs);
            part = this->_part(i);
         }
      }
      Namespace* ns = namespaces::get_by_name(part->name);
      if (!ns) {
         ns = &namespaces::unnamed;
         //
         // Let's real quick check to handle "static indexable" references, e.g. "player[0]" or "script_widget[2]".
         //
         auto type = OpcodeArgTypeRegistry::get().get_static_indexable_type(part->name);
         if (type) {
            if (!part->has_index())
               throw compile_exception(QString("You cannot use a typename such as \"%1\" as a value. If you meant to refer to a specific instance of that type, e.g. player[0], then specify an index in square brackets.").arg(type->internal_name.c_str()));
            part->resolve_index(compiler);
            if (part->index < 0)
               throw compile_exception(QString("Typename indices cannot be negative."));
            if (part->index >= type->static_count)
               throw compile_exception(QString("You specified \"%1[%2]\", but the maximum allowed index is %3.").arg(type->internal_name.c_str()).arg(part->index).arg(type->static_count - 1));
            //
            InterpretedPart interpreted;
            interpreted.type          = type;
            interpreted.disambiguator = part->index;
            interpreted.disambig_type = disambig_type::index;
            if (type->is_variable()) {
               auto var_scope = _var_scope_for_type(*type);
               if (var_scope) {
                  auto which = var_scope->list.lookup_by_signature(type->which_sig_static);
                  assert(which >= 0 && "Bad OpcodeArgTypeinfo::which_sig_global for the type being matched here.");
                  interpreted.disambiguator += which;
                  interpreted.disambig_type = disambig_type::which;
               }
            }
            this->interpreted.push_back(interpreted);
            //
            // We return the number of parts consumed, so that code to read the rest of the VariableReference 
            // knows where to continue from.
            //
            return ++i;
         }
         //
         // If no static type was found, fall through to matching members.
         //
      }
      part = this->_part(++i);
      if (!part)
         throw compile_exception(QString("You cannot use a namespace such as \"%1\" as a value.").arg(ns->name.c_str()));
      if (ns->can_have_variables) {
         //
         // Let's check to see if we're looking at a global variable.
         //
         #if _DEBUG
            assert(ns == &namespaces::global && "Only the global namespace can have variables, and some of the code here is written on the assumption that that will not change in the future.");
         #endif
         auto type = OpcodeArgTypeRegistry::get().get_variable_type(part->name);
         if (type) {
            if (!part->has_index())
               throw compile_exception(QString("You must indicate which variable you are referring to, using an index, e.g. \"global.%1[0]\" instead of \"global.%1\".").arg(type->internal_name.c_str()));
            part->resolve_index(compiler);
            if (part->index < 0)
               throw compile_exception(QString("Typename indices cannot be negative."));
            {
               Megalo::variable_type vt = _var_type_constant_for_type(*type);
               auto max = Megalo::MegaloVariableScopeGlobal.max_variables_of_type(vt);
               if (part->index >= max)
                  throw compile_exception(QString("You specified \"%1[%2]\", but the maximum allowed index is %3.").arg(type->internal_name.c_str()).arg(part->index).arg(max - 1));
            }
            //
            InterpretedPart interpreted;
            interpreted.type          = type;
            interpreted.disambiguator = part->index;
            interpreted.disambig_type = disambig_type::index;
            {
               auto var_scope = _var_scope_for_type(*type);
               if (var_scope) {
                  //
                  // Global instances of this variable type can be represented by a "which" value, so use 
                  // that instead.
                  //
                  auto which = var_scope->list.lookup_by_signature(type->which_sig_global);
                  assert(which >= 0 && "Bad OpcodeArgTypeinfo::which_sig_global for the type being matched here.");
                  interpreted.disambiguator += which;
                  interpreted.disambig_type = disambig_type::which;
               }
            }
            this->interpreted.push_back(interpreted);
            return ++i;
         }
      }
      //
      // The first part(s) of the VariableReference were not a statically-indexable type e.g. player[0], nor were 
      // they a global variable e.g. global.player[0], so they must instead be a namespace member.
      //
      auto member = ns->get_member(part->name);
      if (!member) {
         if (ns == &namespaces::unnamed)
            throw compile_exception(QString("There are no unscoped values named \"%1\".").arg(part->name));
         throw compile_exception(QString("Namespace \"%1\" does not have a member named \"%2\".").arg(ns->name.c_str()).arg(part->name));
      }
      InterpretedPart interpreted;
      interpreted.type = &member->type;
      if (member->is_which_member()) {
         interpreted.disambiguator = member->which;
         interpreted.disambig_type = disambig_type::which;
      } else {
         interpreted.disambiguator = member->scope_indicator_id;
         interpreted.disambig_type = disambig_type::scope;
      }
      this->interpreted.push_back(interpreted);
      return ++i;
   }
   //
   void VariableReference::resolve(Compiler& compiler) {
      if (this->resolved)
         return;
      size_t i = this->_resolve_first_parts(compiler);
      assert(this->interpreted.size() > 0 && "We failed to interpret the first parts of a VariableReference, but also failed to log an error. How did this happen?");
      while (i < this->parts.size()) {
         auto* part = &this->parts[i];
         auto& prev = this->interpreted.back();
         {
            //
            // There are two approaches for resolving aliases. If we check for aliases after checking for everything else, 
            // then we'll have to repeat the checks for everything else in the event that an alias is used. If we check 
            // for aliases before checking for everything else, then there's no need for repetition, but if we make any 
            // mistakes in our code for reading alias definitions, then aliases can shadow built-ins, which we don't want. 
            // Basically, all of the things we check for here? We have to explicitly disallow shadowing them when we're 
            // reading an alias definition.
            //
            auto alias = compiler.lookup_relative_alias(part->name, prev.type);
            if (alias) {
               if (part->has_index())
                  throw compile_exception(QString("Aliases such as \"%1\" can refer to indexed data but cannot themselves be indexed.").arg(alias->name));
               this->_transclude(i, *alias);
               part = &this->parts[i];
            }
         }
         //
         // If a relative alias was used, it has now been resolved, which means that now, either we're looking at a 
         // built-in, or we're looking at something invalid.
         //
         if (prev.type->can_have_variables()) {
            //
            // Handle nested variables, e.g. global.player[0].number[1]
            //
            auto type = OpcodeArgTypeRegistry::get().get_variable_type(part->name);
            if (type) {
               auto prev_scope = _var_scope_for_type(*prev.type);
               assert(prev_scope && "If a variable type doesn't have a VariableScope object, then it shouldn't claim to be able to hold variables.");
               //
               if (!part->has_index())
                  throw compile_exception(QString("You must indicate which variable you are referring to, using an index, e.g. \"global.%1[0]\" instead of \"global.%1\".").arg(type->internal_name.c_str()));
               part->resolve_index(compiler);
               if (part->index < 0)
                  throw compile_exception(QString("Typename indices cannot be negative."));
               {
                  Megalo::variable_type vt = _var_type_constant_for_type(*type);
                  auto max = prev_scope->max_variables_of_type(vt);
                  if (part->index >= max)
                     throw compile_exception(QString("You specified \"%1[%2]\", but the maximum allowed index is %3.").arg(type->internal_name.c_str()).arg(part->index).arg(max - 1));
               }
               //
               InterpretedPart interpreted;
               interpreted.type          = type;
               interpreted.disambiguator = part->index;
               interpreted.disambig_type = disambig_type::index;
               this->interpreted.push_back(interpreted);
               continue;
            }
         }
         auto prop = prev.type->get_property_by_name(part->name);
         if (prop) {
            if (part->has_index())
               throw compile_exception(QString("Properties, such as \"%1\" on the %2 type, cannot be indexed.").arg(prop->name.c_str()).arg(prev.type->internal_name.c_str()));
            InterpretedPart interpreted;
            interpreted.type     = &prop->type;
            interpreted.property = prop->name.c_str();
            this->interpreted.push_back(interpreted);
            continue;
         }
         //
         // TODO: PROPERTIES DEFINED BY WAY OF OPCODES
         //

         // else, if no match:
         throw compile_exception(QString("Member name \"%1\" is not defined for type %2.").arg(part->name).arg(prev.type->internal_name.c_str()));
      }
      this->resolved = true;
   }
}