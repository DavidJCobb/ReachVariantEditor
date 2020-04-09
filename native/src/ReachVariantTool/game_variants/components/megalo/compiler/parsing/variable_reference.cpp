#include "variable_reference.h"
#include "../compiler.h"
#include "../namespaces.h"
#include "../../actions.h"
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
      this->name = other.name;
      this->index = other.index;
      this->index_str = other.index_str;
      this->index_is_numeric = other.index_is_numeric;
      return *this;
   }
   void VariableReference::Part::resolve_index(Compiler& compiler) {
      if (this->index_is_numeric)
         return;
      auto abs = compiler.lookup_absolute_alias(this->index_str);
      if (!abs)
         throw compile_exception(QString("\"%1\" is not a valid index. Only integer constants and aliases of integer constants can be indices.").arg(this->index_str));
      if (!abs->is_integer_constant())
         throw compile_exception(QString("Alias \"%1\" is not an integer constant and therefore cannot be used as an index.").arg(abs->name));
      this->index = abs->get_integer_constant();
      this->index_is_numeric = true;
      this->index_str.clear();
   }

   bool VariableReference::InterpretedPart::is_none() const noexcept {
      if (this->disambig_type != disambig_type::which)
         return false;
      return this->which->is_none();
   }
   bool VariableReference::InterpretedPart::is_read_only() const noexcept {
      switch (this->disambig_type) {
         case disambig_type::static_var:
         case disambig_type::constant_integer:
            return true;
         case disambig_type::scope:
            return this->scope->is_readonly();
         case disambig_type::which:
            return this->which->is_read_only();
      }
      return false;
   }
   bool VariableReference::InterpretedPart::is_variable() const noexcept {
      if (!this->type)
         return false;
      if (!this->type->is_variable())
         return false;
      return this->disambig_type == disambig_type::index || this->disambig_type == disambig_type::static_var;
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
      auto    size = text.size();
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
               name = "";
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
      part.type = nullptr;
      part.disambiguator = v;
      part.disambig_type = disambig_type::constant_integer;
      this->interpreted.push_back(part);
      this->resolved = true;
   }
   bool VariableReference::is_read_only() const noexcept {
      if (this->property.abstract)
         return this->property.abstract->setter != nullptr;
      if (this->property.normal)
         return this->property.normal->scope->is_readonly();
      if (!this->interpreted.size())
         return false;
      auto& last = this->interpreted.back();
      return last.is_read_only();
   }
   bool VariableReference::is_statically_indexable_value() const noexcept {
      if (!this->interpreted.size())
         return false;
      return this->interpreted.front().disambig_type == disambig_type::static_var;
   }
   const OpcodeArgTypeinfo* VariableReference::get_type() const noexcept {
      if (!this->interpreted.size())
         return nullptr;
      return this->interpreted.back().type;
   }
   QString VariableReference::to_string() const noexcept {
      QString result;
      for (size_t i = 0; i < this->parts.size(); ++i) { // I wanted to use (this->interpreted) but it doesn't retain enough information
         if (i)
            result += '.';
         auto& part = this->parts[i];
         result += part.name;
         if (part.has_index()) {
            result += '[';
            if (part.index_is_numeric)
               result += part.index;
            else
               result += part.index_str;
            result += ']';
         }
      }
      return result;
   }

   #pragma region Variable reference resolution code
   size_t VariableReference::_resolve_first_parts(Compiler& compiler, bool is_alias_definition) {
      size_t i    = 0;
      auto   part = this->_part(i);
      if (is_alias_definition && !part->has_index()) {
         auto type = OpcodeArgTypeRegistry::get().get_variable_type(part->name);
         if (type && type->can_have_variables()) {
            InterpretedPart interpreted;
            interpreted.type = type;
            interpreted.disambig_type = disambig_type::alias_relative_to;
            this->interpreted.push_back(interpreted);
            return ++i;
         }
      }
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
            interpreted.disambig_type = disambig_type::static_var;
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
         interpreted.which         = member->which;
         interpreted.disambig_type = disambig_type::which;
      } else {
         interpreted.scope         = member->scope;
         interpreted.disambig_type = disambig_type::scope;
      }
      this->interpreted.push_back(interpreted);
      return ++i;
   }
   //
   bool VariableReference::_resolve_abstract_property(Compiler& compiler, QString name, const OpcodeArgTypeinfo& property_is_on) {
      auto&       manager = AbstractPropertyRegistry::get();
      const auto* entry   = manager.get_by_name(name.toStdString().c_str());
      if (!entry)
         entry = manager.get_variably_named_property(compiler, name, property_is_on);
      if (entry) {
         this->property.abstract      = entry;
         this->property.abstract_name = name;
         return true;
      }
      return false;
   }
   //
   void VariableReference::resolve(Compiler& compiler, bool is_alias_definition) {
      if (this->resolved)
         return;
      size_t i = this->_resolve_first_parts(compiler, is_alias_definition);
      assert(this->interpreted.size() > 0 && "We failed to interpret the first parts of a VariableReference, but also failed to log an error. How did this happen?");
      for (; i < this->parts.size(); ++i) {
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
         if (prev.is_none())
            throw compile_exception("Cannot access members on a none value.");
         if (prev.disambig_type == disambig_type::scope)
            throw compile_exception(QString("Cannot access members on this value. Copy it into an intermediate variable."));
         //
         if (prev.type->can_have_variables()) {
            //
            // Handle nested variables, e.g. global.player[0].number[1]
            //
            // TODO: Do not allow (static.var.var) or (var.var.var)
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
            bool has_index         = part->has_index();
            auto should_have_index = prop->scope->has_index();
            if (has_index != should_have_index) {
               if (should_have_index)
                  throw compile_exception(QString("The \"%1\" property on the %2 type must be indexed.").arg(prop->name.c_str()).arg(prev.type->internal_name.c_str()));
               else
                  throw compile_exception(QString("The \"%1\" property on the %2 type cannot be indexed.").arg(prop->name.c_str()).arg(prev.type->internal_name.c_str()));
            }
            if (has_index)
               this->property.normal_index = part->index;
            this->property.normal = prop;
            ++i; // move to the next part, and then bail out of the loop so we can use property-specific logic
            break;
         }
         //
         // The name didn't match any properties that are explicitly defined on the previous type. Let's try the 
         // abstract properties:
         //
         if (this->_resolve_abstract_property(compiler, part->name, *prev.type)) {
            ++i; // move to the next part, and then bail out of the loop so we can use property-specific logic
            break;
         }
         throw compile_exception(QString("Member name \"%1\" is not defined for type %2.").arg(part->name).arg(prev.type->internal_name.c_str()));
      }
      if (i < this->parts.size()) {
         //
         // We've handled everything up to the first property access, if any. If there is any more left to handle, 
         // then it means that the script author is trying to access the member of a property. This is allowed if 
         // they are accessing an abstract property through a non-abstract property, and disallowed in all other 
         // cases.
         //
         #if _DEBUG
            assert(!(this->property.normal && this->property.abstract) && "Only one of these should've been set; we should've bailed out of the above loop at the first property we found.");
         #endif
         auto* part = &this->parts[i];
         auto& prev = this->interpreted.back();
         //
         if (this->property.abstract) {
            //
            // You cannot access the members of an abstract property.
            //
            QString name = this->property.abstract_name;
            throw compile_exception(QString("You cannot access the members of an abstract property like \"%1.%2\".").arg(prev.type->internal_name.c_str()).arg(name));
         }
         if (this->property.normal) {
            //
            // You can access an abstract property through a non-abstract property, but you cannot access 
            // anything else through a non-abstract property: current_player.biped.shields is allowed, but 
            // current_player.team.score is not.
            //
            auto& prop_type = this->property.normal->type;
            if (!this->_resolve_abstract_property(compiler, part->name, prop_type))
               throw compile_exception(QString("The %1 type does not have an abstract property named \"%2\", and in this context, no other members are accessible.")
                  .arg(prop_type.internal_name.c_str())
                  .arg(part->name)
               );
         }
         if (++i < this->parts.size())
            //
            // We did something like (var.normal_property.abstract_property.something_else).
            //
            throw compile_exception(QString("No further members are accessible here. Use an intermediate variable."));
      }
      if (is_alias_definition && this->interpreted.size() == 1 && !this->is_property()) {
         //
         // If this is an alias definition, then we would interpret something like "player" or "team" as the type 
         // that the alias is relative to, but specifying only a type should be a syntax error.
         //
         auto& first = this->interpreted[0];
         if (first.disambig_type == disambig_type::alias_relative_to)
            throw compile_exception(QString("You cannot alias a typename such as \"%1\".").arg(first.type->internal_name.c_str()));
      }
      this->resolved = true;
   }
   #pragma endregion
}