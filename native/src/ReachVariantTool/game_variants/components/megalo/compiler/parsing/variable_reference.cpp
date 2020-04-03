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
   VariableReference::VariableReference(int32_t i) {
      this->resolved.constant = i;
      this->resolved.done = true;
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
         this->resolved.constant = text.toInt(&ok);
         if (ok) {
            this->resolved.done = true;
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
   void VariableReference::resolve() {
      if (this->resolved.done)
         return;
      //
      // TODO: Us doing this here instead of on VariableReference::Part could cause issues with expanding 
      // aliases, no? Hm...
      //
      size_t i = 0;
      {  // Identify the "which."
         auto part = this->_part(i);
         //
         Namespace* ns = namespaces::get_by_name(part->name);
         if (!ns) {
            //
            // Try matching this to a statically-indexable type, e.g. player[0] or script_widget[1]; if that 
            // fails, then fall back to the unnamed namespace.
            //
            auto type = OpcodeArgTypeRegistry::get().get_by_internal_name(part->name);
            if (type && type->can_be_static()) {
               if (!part->has_index())
                  throw compile_exception(QString("You cannot use a typename such as \"%1\" as a value. If you meant to refer to a specific instance of that type, e.g. player[0], then specify an index in square brackets.").arg(type->internal_name.c_str()));
               if (part->index < 0)
                  throw compile_exception(QString("Typename indices cannot be negative."));
               if (part->index >= type->static_count)
                  throw compile_exception(QString("You specified \"%1[%2]\", but the maximum allowed index is %3.").arg(type->internal_name.c_str()).arg(part->index).arg(type->static_count - 1));
               //
               this->resolved.which_type = type;
               if (type->is_variable()) {
                  //
                  // A variable type that can be statically indexed should have an enum describing all 
                  // of its global values (i.e. megalo_players or megalo_teams). The type should also 
                  // specify which value in that enum matches the first static index, i.e. which value 
                  // is "player[0]" or "team[0]". Because the enum values are contiguous (i.e. for any 
                  // "player[n]", "player[n + 1]" comes after it), we can just add the variable index 
                  // to that.
                  //
                  auto var_scope = _var_scope_for_type(*type);
                  assert(var_scope);
                  this->resolved.which = var_scope->list.lookup_by_signature(type->which_sig_static);
                  if (this->resolved.which >= 0)
                     this->resolved.which += part->index;
               } else {
                  //
                  // The type is not a variable. (This can occur for script traits and widgets.)
                  //
                  this->resolved.var_index = part->index;
               }
            } else
               ns = &namespaces::unnamed;
         }
         if (ns) { // if (ns == nullptr), then we already resolved the "which" in the code above
            part = this->_part(++i);
            //
            // Try matching this to a variable, e.g. global.player[0]. If that fails or if the namespace 
            // can't hold variables, then see if it's a namespace member.
            //
            if (!part)
               throw compile_exception(QString("You cannot use a namespace such as \"%1\" as a value.").arg(ns->name.c_str()));
            auto type = OpcodeArgTypeRegistry::get().get_by_internal_name(part->name);
            if (ns->can_have_variables && type && type->is_variable()) {
               if (!part->has_index())
                  throw compile_exception(QString("You cannot use a typename such as \"%1\" as a value. If you meant to refer to a specific instance of that type, e.g. player[0], then specify an index in square brackets.").arg(type->internal_name.c_str()));
               if (part->index < 0)
                  throw compile_exception(QString("Variable indices cannot be negative."));
                  
               //
               // TODO: Use (MegaloVariableScopeGlobal) to bounds-check the index.
               //
               this->resolved.which_type = type;
               //
               // TODO: Identify the (which) value. (Requires that the typeinfo specify the signature of 
               // the first global-variable entry, e.g. the signature of "global.number[0]" for numbers; 
               // we can assume that those are contiguous and just add to the enum value index.)
               //
            } else {
               auto member = ns->get_member(part->name);
               if (!member)
                  //
                  // TODO: resolve aliases
                  //
                  throw compile_exception(QString("Namespace \"%1\" does not have a member named \"%2\".").arg(ns->name.c_str()).arg(part->name));
               this->resolved.which_type = &member->type;
               this->resolved.which = member->which;
               this->resolved.scope = member->scope_indicator_id;
            }
         }
         // Done with resolving the "which."
      }
      auto part = this->_part(++i);
      if (part) {
         if (this->resolved.which_type->can_have_variables()) {

         }

      }
      //
      // TODO: EVERYTHING AFTER THE (which) VALUE
      //
      this->resolved.done = true;
   }
}