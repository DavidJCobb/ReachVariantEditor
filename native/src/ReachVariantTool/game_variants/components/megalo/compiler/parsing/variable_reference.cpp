#include "variable_reference.h"
#include "../compiler.h"
#include "../enums.h"
#include "../namespaces.h"
#include "../../actions.h"
#include "../../variables_and_scopes.h"
#include "../../opcode_arg_type_registry.h"
#include "../../opcode_arg_types/variables/all_core.h"
#include "../../../../../helpers/qt/string.h"

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
}

namespace Megalo::Script {
   VariableReference::RawPart& VariableReference::RawPart::operator=(const VariableReference::RawPart& other) noexcept {
      this->name = other.name;
      this->index = other.index;
      this->index_str = other.index_str;
      this->index_is_numeric = other.index_is_numeric;
      return *this;
   }
   bool VariableReference::RawPart::resolve_index(Compiler& compiler) {
      if (this->index_is_numeric)
         return true;
      if (this->index_str.isEmpty()) // if the string is empty and the "numeric" bool is not true, then there is no index
         return true;
      auto abs = compiler.lookup_absolute_alias(this->index_str);
      if (!abs) {
         compiler.raise_error(QString("\"%1\" is not a valid index. Only integer constants and aliases of integer constants can be indices.").arg(this->index_str));
         return false;
      }
      if (!abs->is_integer_constant()) {
         compiler.raise_error(QString("Alias \"%1\" is not an integer constant and therefore cannot be used as an index.").arg(abs->name));
         return false;
      }
      this->index = abs->get_integer_constant();
      this->index_is_numeric = true;
      this->index_str.clear();
      return true;
   }

   VariableReference::VariableReference(int32_t i) {
      this->set_to_constant_integer(i);
      #if _DEBUG
         this->content = QString("%1").arg(i);
      #endif
   }
   VariableReference::VariableReference(Compiler& compiler, QString text) {
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
               compiler.raise_fatal(QString("Character %1 is not valid inside of an index.").arg(c));
               return;
            }
            if (c == ']') {
               is_index = false;
               if (index.isEmpty()) { // "name[]" is a syntax error
                  compiler.raise_fatal("Variables of the form \"name[]\" are not valid. Specify an index if appropriate, or no square brackets at all otherwise.");
                  return;
               }
               continue;
            }
            if (index.isEmpty() && cobb::string_scanner::is_whitespace_char(c))
               continue;
            index += c;
         } else {
            if (c == ']') {
               compiler.raise_fatal("Unexpected \"]\".");
               return;
            }
            if (c == '[') {
               if (cobb::qt::string_is_integer(name)) { // "123[4]" is a syntax error
                  if (this->raw.empty())
                     compiler.raise_fatal("An integer literal cannot be indexed.");
                  else
                     compiler.raise_fatal("An integer literal cannot be a property or nested variable."); // "name.123" is also a syntax error and should take priority
                  return;
               }
               if (!index.isEmpty()) {
                  compiler.raise_fatal("Variables of the form \"name[1][2]\" are not valid. Only specify one index.");
                  return;
               }
               is_index = true;
               continue;
            }
            if (c == '.') {
               if (cobb::qt::string_is_integer(name)) { // "1234.name" and "name.1234.name" are syntax errors
                  if (this->raw.empty())
                     compiler.raise_fatal("An integer literal cannot have properties or nested variables."); // 1234.name"
                  else
                     compiler.raise_fatal("An integer literal cannot be a property or nested variable."); // "name.1234.name"
                  return;
               }
               index = index.trimmed();
               for (auto c : index) {
                  if (c.isSpace()) { // "name[1 2]", "name[word word]", etc.
                     compiler.raise_fatal("An index cannot consist of multiple space-separated terms.");
                     return;
                  }
               }
               auto part = RawPart(name, index);
               this->raw.push_back(part);
               name = "";
               index = "";
               continue;
            }
            if (!index.isEmpty()) { // "name[1]name" is a syntax error
               compiler.raise_fatal("Expected a period. Variables of the form \"name[1]name\" are not valid.");
               return;
            }
            name += c;
         }
      }
      //
      // We've reached the end of the string. We were only saving parts when we encountered a '.', so the 
      // last part in the string still needs to be validated and added.
      //
      {
         if (name.isEmpty()) { // text ended in '.' or was empty string
            compiler.raise_fatal("Variables cannot end in '.', and cannot be nameless.");
            return;
         }
         if (is_index) {
            compiler.raise_fatal("Expected a closing square bracket."); // "name[1].name[2" is a syntax error
            return;
         }
         if (cobb::qt::string_is_integer(name)) {
            if (this->raw.empty())
               //
               // If there are no earlier parts, then this must have been something like "123[4]". If there 
               // weren't an index, then the entire string would just be the integer and we would've treated 
               // it accordingly at the very start of this function.
               //
               compiler.raise_fatal("An integer literal cannot be indexed.");
            else
               compiler.raise_fatal("An integer literal cannot be a property or nested variable.");
            return;
         }
         index = index.trimmed();
         for (auto c : index) {
            if (c.isSpace()) { // "name[1 2]", "name[word word]", etc.
               compiler.raise_fatal("An index cannot consist of multiple space-separated terms.");
               return;
            }
         }
         auto part = RawPart(name, index);
         this->raw.push_back(part);
      }
      //
      // Next, let's do some final basic validation -- specifically, disallow the use of keywords as part 
      // names and indices, disallow empty part names, and store integer indices as integers.
      //
      for (auto& part : this->raw) {
         bool ok = false;
         if (part.name.isEmpty())
            compiler.raise_fatal("Invalid variable name. Did you accidentally type two periods instead of one?");
         else if (Compiler::is_keyword(part.name))
            compiler.raise_fatal(QString("Keywords, including \"%1\", cannot be used as variable names.").arg(part.name));
         else if (Compiler::is_keyword(part.index_str))
            compiler.raise_fatal(QString("Keywords, including \"%1\", cannot be used as indices in a collection.").arg(part.index_str));
         else
            ok = true;
         if (!ok)
            return;
         //
         ok = false;
         part.index = part.index_str.toInt(&ok);
         part.index_is_numeric = ok;
         if (ok)
            part.index_str = "";
      }
   }

   void VariableReference::set_to_constant_integer(int32_t v) {
      this->is_resolved = true;
      this->resolved.top_level.is_constant = true;
      this->resolved.top_level.index       = v;
   }
   QString VariableReference::to_string() const noexcept {
      if (!this->is_resolved || this->is_invalid)
         return this->to_string_from_raw();
      auto& resolved  = this->resolved;
      auto& top_level = resolved.top_level;
      auto& nested    = resolved.nested;
      if (top_level.is_constant)
         return QString("%1").arg(top_level.index);
      if (top_level.namespace_member.scope) {
         //
         // If the (scope) is set, then we're dealing with a dead-end value that has no 
         // which. If it also lacks an index, then we can just use its decompile format 
         // string; there's nothing we need to pass to it.
         //
         auto    scope  = top_level.namespace_member.scope;
         QString result = scope->format;
         if (!scope->has_index())
            return result;
         result.replace("%i", QString("%1").arg(top_level.index));
         return result;
      }
      //
      QString result;
      if (top_level.namespace_member.which) {
         result = top_level.namespace_member.which->name.c_str();
      } else if (top_level.type) {
         if (!top_level.is_static) {
            if (top_level.is_temporary) {
               result = "temporaries.";
            } else {
               result = "global.";
            }
         }
         result += top_level.type->internal_name.c_str();
         result += '[';
         result += QString("%1").arg(top_level.index);
         result += ']';
      } else {
         auto top_type = this->resolved.alias_basis;
         //
         if (top_level.is_temporary) {
            result = "temporaries.";
         } else {
            result = "global.";
         }
         if (top_type) {
            result += top_type->internal_name.c_str();
         } else {
            result += "?????";
         }
      }
      //
      if (nested.type) {
         result += '.';
         result += nested.type->internal_name.c_str();
         result += '[';
         result += QString("%1").arg(nested.index);
         result += ']';
      }
      if (auto property = resolved.property.definition) {
         result += '.';
         result += property->name.c_str();
         if (property->has_index()) {
            result += '[';
            result += QString("%1").arg(resolved.property.index);
            result += ']';
         }
      }
      if (auto accessor = resolved.accessor) {
         result += '.';
         if (accessor->name.empty())
            result += resolved.accessor_name;
         else
            result += accessor->name.c_str();
      }
      return result;
   }
   QString VariableReference::to_string_from_raw(int8_t start, int8_t up_to) const noexcept {
      if (up_to < 0)
         up_to += this->raw.size();
      QString result;
      for (size_t i = start; i < up_to; ++i) {
         if (i)
            result += '.';
         auto& part = this->raw[i];
         result += part.name;
         if (part.has_index()) {
            result += '[';
            if (part.index_is_numeric)
               result += QString::number(part.index);
            else
               result += part.index_str;
            result += ']';
         }
      }
      return result;
   }

   const OpcodeArgTypeinfo* VariableReference::get_type() const noexcept {
      auto& resolved = this->resolved;
      if (resolved.top_level.is_constant)
         return &OpcodeArgValueScalar::typeinfo;
      if (resolved.property.definition)
         return &resolved.property.definition->type;
      if (resolved.nested.type)
         return resolved.nested.type;
      if (resolved.top_level.type)
         return resolved.top_level.type;
      return resolved.alias_basis;
   }
   bool VariableReference::is_none() const noexcept {
      auto which = this->resolved.top_level.namespace_member.which;
      return which ? which->is_none() : false;
   }
   bool VariableReference::is_property() const noexcept { return this->resolved.property.definition && !this->is_accessor(); }
   bool VariableReference::is_read_only() const noexcept {
      auto& resolved = this->resolved;
      if (!resolved.top_level.type)
         return false;
      if (auto scope = resolved.top_level.namespace_member.scope)
         return scope->flags & VariableScopeIndicatorValue::flags::is_readonly;
      if (resolved.accessor)
         return resolved.accessor->setter == nullptr;
      if (resolved.property.definition) {
         auto scope = resolved.property.definition->scope;
         if (!scope)
            return true;
         return scope->is_readonly();
      }
      if (resolved.nested.type)
         return !resolved.nested.type->is_variable(); // only variables can be assigned to
      if (resolved.top_level.is_static || resolved.top_level.is_constant)
         return true;
      if (resolved.top_level.namespace_member.which)
         return resolved.top_level.namespace_member.which->is_read_only();
      return !resolved.top_level.type->is_variable(); // only variables can be assigned to
   }
   bool VariableReference::is_statically_indexable_value() const noexcept {
      if (this->is_accessor())
         return false;
      if (this->is_property())
         return false;
      if (this->resolved.nested.type)
         return false;
      return this->resolved.top_level.is_static;
   }
   bool VariableReference::is_variable() const noexcept {
      if (this->is_accessor())
         return false;
      if (this->is_property())
         return false;
      if (this->is_constant_integer())
         return false;
      if (auto type = this->resolved.nested.type)
         return type->is_variable();
      if (auto type = this->resolved.top_level.type)
         return type->is_variable();
      return false;
   }
   //
   variable_scope VariableReference::get_containing_scope() const noexcept {
      auto& top  = this->resolved.top_level;
      auto& nest = this->resolved.nested;
      if (!top.type || !top.type->is_variable())
         return variable_scope::not_a_scope;
      if (!nest.type) {
         if (top.is_temporary)
            return variable_scope::temporary;
         return variable_scope::global;
      }
      if (top.type == &OpcodeArgValueObject::typeinfo)
         return variable_scope::object;
      if (top.type == &OpcodeArgValuePlayer::typeinfo)
         return variable_scope::player;
      if (top.type == &OpcodeArgValueTeam::typeinfo)
         return variable_scope::team;
      return variable_scope::not_a_scope;
   }
   const_team VariableReference::to_const_team(bool* success) const noexcept {
      if (success)
         *success = false;
      auto& top = this->resolved.top_level;
      if (top.type != &OpcodeArgValueTeam::typeinfo)
         return const_team::none;
      //
      if (success)
         *success = true;
      if (top.is_static)
         return (const_team)((uint8_t)const_team::team_1 + top.index);
      if (top.namespace_member.which == &variable_which_values::team::no_team)
         return const_team::none;
      if (top.namespace_member.which == &variable_which_values::team::neutral_team)
         return const_team::neutral;
      //
      if (success)
         *success = false;
      return const_team::none;
   }
   //
   void VariableReference::strip_accessor() noexcept {
      auto& res = this->resolved;
      res.accessor = nullptr;
      res.accessor_name.clear();
   }

   bool VariableReference::is_transient() const {
      if (this->is_invalid)
         return false;
      {
         auto* scope = this->resolved.top_level.namespace_member.scope;
         auto* which = this->resolved.top_level.namespace_member.which;
         if (which)
            if (which->flags & VariableScopeWhichValue::flag::is_transient)
               return true;
      }
      return false;
   }

   bool VariableReference::is_usable_in_pregame() const {
      if (this->is_invalid)
         return true;
      auto* type = this->get_type();
      if (type != &OpcodeArgValueScalar::typeinfo)
         //
         // MegaloEdit only seems to enforce pre-game limits on number variables. I can't 
         // imagine it'd make much sense to screw around with objects and players during 
         // pre-game. Teams? Timers? Maybe.
         //
         return true;
      
      auto* scope = this->resolved.top_level.namespace_member.scope;
      assert(scope != nullptr);
      return (scope->flags & VariableScopeIndicatorValue::flags::allowed_in_pregame) != 0;
   }

   #pragma region Variable reference resolution code
   void VariableReference::__transclude_alias(uint32_t raw_index, Alias& alias) {
      #if _DEBUG
         assert(alias.target);
         assert(raw_index < this->raw.size());
      #endif
      auto& resolved = alias.target->resolved;
      //
      std::vector<RawPart> replacements;
      if (!resolved.alias_basis) {
         //
         // If we're transcluding an absolute alias, then start with the top level. If we're 
         // transcluding a relative alias, then there is no top level.
         //
         auto& top_level = resolved.top_level;
         //
         QString name;
         int32_t index = top_level.index;
         bool    has_index = false;
         //
         if (auto scope = top_level.namespace_member.scope) {
            name      = scope->format;
            has_index = scope->has_index();
            if (has_index) {
               //
               // The scope format may be something like "script_option[%i]" so we gotta shear that spot for 
               // the index off.
               //
               auto i = name.lastIndexOf('[');
               if (i >= 0)
                  name = name.left(i);
            }
         } else if (auto which = top_level.namespace_member.which) {
            name = top_level.namespace_member.which->name.c_str();
         } else {
            has_index = true;
            if (!top_level.is_static) {
               if (top_level.is_temporary)
                  replacements.emplace_back("temporaries");
               else
                  replacements.emplace_back("global");
            }
            name = top_level.type->internal_name.c_str();
         }
         //
         if (has_index)
            replacements.emplace_back(name, index);
         else
            replacements.emplace_back(name);
      }
      if (auto type = resolved.nested.type) {
         QString name = type->internal_name.c_str();
         replacements.emplace_back(name, resolved.nested.index);
      }
      if (auto prop = resolved.property.definition) {
         QString name = prop->name.c_str();
         if (prop->has_index())
            replacements.emplace_back(name, resolved.property.index);
         else
            replacements.emplace_back(name);
      }
      if (auto accessor = resolved.accessor) {
         QString name;
         if (accessor->name.empty())
            name = resolved.accessor_name;
         else
            name = accessor->name.c_str();
         replacements.emplace_back(name);
      }
      //
      // Now that we've generated RawParts from the alias content, we need to replace the 
      // target raw-part with them.
      //
      auto it = this->raw.begin() + raw_index + 1; // we want to insert after the target element, but (insert) inserts before, so add one
      this->raw.insert(it, replacements.begin(), replacements.end());
      this->raw.erase(this->raw.begin() + raw_index);
   }
   //
   bool VariableReference::_resolve_aliases_from(Compiler& compiler, size_t raw_index, const OpcodeArgTypeinfo* basis) {
      auto   part  = this->_get_raw_part(raw_index);
      Alias* alias = nullptr;
      if (basis)
         alias = compiler.lookup_relative_alias(part->name, basis);
      else {
         alias = compiler.lookup_absolute_alias(part->name);
         if (alias && alias->is_imported_name()) {
            compiler.raise_error(QString("Alias \"%1\" refers to imported name \"%2\" and cannot appear where a variable can.").arg(alias->name).arg(alias->target_imported_name));
            this->is_invalid = true;
            return false;
         }
      }
      if (!alias)
         return false;
      if (part->has_index()) {
         compiler.raise_error(QString("Aliases such as \"%1\" can refer to indexed data but cannot themselves be indexed.").arg(alias->name));
         this->is_invalid = true;
         return false;
      }
      if (!basis) {
         if (alias->is_integer_constant()) {
            this->resolved.top_level.is_constant = true;
            this->resolved.top_level.index       = alias->get_integer_constant();
            return true;
         }
         if (alias->is_enumeration()) {
            this->resolved.top_level.enumeration = alias->get_enumeration();
            return true;
         }
      }
      this->__transclude_alias(raw_index, *alias);
      return true;
   }
   size_t VariableReference::_resolve_top_level(Compiler& compiler, bool is_alias_definition) {
      size_t i    = 0;
      auto   part = this->_get_raw_part(i);
      if (is_alias_definition && !part->has_index()) {
         auto type = OpcodeArgTypeRegistry::get().get_variable_type(part->name);
         if (type && type->can_have_variables()) {
            this->resolved.alias_basis = type;
            return ++i;
         }
      }
      Namespace* ns = nullptr;
      if (!part->has_index())
         ns = namespaces::get_by_name(part->name);
      if (ns) {
         //
         // The part is the name of a namespace, so move onto the next part.
         //
         part = this->_get_raw_part(++i);
         if (!part) {
            compiler.raise_error(QString("You cannot use a namespace such as \"%1\" as a value.").arg(ns->name.c_str()));
            this->is_invalid = true;
            return 0;
         }
      } else {
         ns = &namespaces::unnamed;
         //
         // Let's real quick check to handle "static indexable" references, e.g. "player[0]" or "script_widget[2]".
         //
         auto type = OpcodeArgTypeRegistry::get().get_static_indexable_type(part->name);
         if (type) {
            if (!part->has_index()) {
               compiler.raise_error(QString("You cannot use a typename such as \"%1\" as a value. If you meant to refer to a specific instance of that type, e.g. player[0], then specify an index in square brackets.").arg(type->internal_name.c_str()));
               this->is_invalid = true;
               return 0;
            }
            if (part->index < 0) {
               compiler.raise_error(QString("Typename indices cannot be negative."));
               this->is_invalid = true;
               return 0;
            }
            if (part->index >= type->static_count) {
               compiler.raise_error(QString("You specified \"%1[%2]\", but the maximum allowed index is %3.").arg(type->internal_name.c_str()).arg(part->index).arg(type->static_count - 1));
               this->is_invalid = true;
               return 0;
            }
            //
            auto& res = this->resolved.top_level;
            res.type      = type;
            res.index     = part->index;
            res.is_static = true;
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
      if (ns->can_have_variables) {
         //
         // Let's check to see if we're looking at a global variable.
         //
         auto type = OpcodeArgTypeRegistry::get().get_variable_type(part->name);
         if (type) {
            if (!part->has_index()) {
               compiler.raise_error(
                  QString("You must indicate which variable you are referring to, using an index, e.g. \"%2.%1[0]\" instead of \"%2.%1\".")
                     .arg(type->internal_name.c_str())
                     .arg(ns->name.c_str())
               );
               this->is_invalid = true;
               return 0;
            }
            if (part->index < 0) {
               compiler.raise_error(QString("Typename indices cannot be negative."));
               this->is_invalid = true;
               return 0;
            }
            {
               Megalo::variable_type vt = getVariableTypeForTypeinfo(type);
               int max;
               if (ns == &namespaces::global) {
                  max = Megalo::MegaloVariableScopeGlobal.max_variables_of_type(vt);
               } else if (ns == &namespaces::temporaries) {
                  this->resolved.top_level.is_temporary = true;
                  if (vt == Megalo::variable_type::timer) {
                     compiler.raise_error(QString("Temporary timers cannot exist. (How would a timer keep track of time if it only exists during a single instant?)"));
                     this->is_invalid = true;
                     return 0;
                  }
                  max = Megalo::MegaloVariableScopeTemporary.max_variables_of_type(vt);
               } else {
                  assert(false && "Unhandled namespace type!");
               }
               if (part->index >= max) {
                  compiler.raise_error(QString("You specified \"%1[%2]\", but the maximum allowed index is %3.").arg(type->internal_name.c_str()).arg(part->index).arg(max - 1));
                  this->is_invalid = true;
                  return 0;
               }
            }
            //
            auto& res = this->resolved.top_level;
            res.type  = type;
            res.index = part->index;
            //
            return ++i;
         }
      }
      //
      // The first part(s) of the VariableReference were not a statically-indexable type e.g. player[0], nor were 
      // they a global variable e.g. global.player[0], so they must instead be a namespace member.
      //
      auto member = ns->get_member(part->name);
      if (!member) {
         //
         // Real quick: could this be a user-defined enum?
         //
         auto ude = compiler.lookup_user_defined_enum(part->name);
         if (ude) {
            auto& res = this->resolved.top_level;
            res.type        = &OpcodeArgValueScalar::typeinfo;
            res.enumeration = ude->definition;
            return ++i;
         }
         //
         // Guess not. Error time! :)
         //
         if (ns == &namespaces::unnamed)
            compiler.raise_error(QString("There are no unscoped values named \"%1\".").arg(part->name));
         else
            compiler.raise_error(QString("Namespace \"%1\" does not have a member named \"%2\".").arg(ns->name.c_str()).arg(part->name));
         this->is_invalid = true;
         return 0;
      }
      auto& res = this->resolved.top_level;
      if (member->is_enum_member()) {
         res.type        = &OpcodeArgValueScalar::typeinfo;
         res.enumeration = member->enumeration;
         return ++i;
      }
      res.type = &member->type;
      res.namespace_member.which = member->which;
      res.namespace_member.scope = member->scope;
      {
         //
         // Validate the presence or absence of an index.
         //
         bool should_have_index = member->has_index();
         if (part->has_index() != should_have_index) {
            if (should_have_index)
               compiler.raise_error(QString("The \"%1.%2\" value must be indexed.").arg(member->name.c_str()).arg(part->name));
            else
               compiler.raise_error(QString("The \"%1.%2\" value cannot be indexed.").arg(member->name.c_str()).arg(part->name));
            this->is_invalid = true;
            return 0;
         }
         if (should_have_index)
            res.index = part->index;
      }
      return ++i;
   }
   bool VariableReference::_resolve_enumeration(Compiler& compiler, size_t raw_index) {
      auto& top  = this->resolved.top_level;
      auto* e    = top.enumeration;
      if (!e)
         return false;
      auto  part = this->_get_raw_part(raw_index);
      auto& name = part->name;
      //
      top.enumeration = nullptr; // do not retain references to enums, as user-defined enums may be deleted when they go out of scope (the exception is aliases of enums, which would also go out of scope)
      //
      top.is_constant = true;
      if (!e->lookup(name, top.index)) {
         this->is_invalid = true;
         compiler.raise_error(QString("Enumeration %1 does not have a value named \"%2\".").arg(this->to_string_from_raw(0, raw_index)).arg(name));
         return true;
      }
      bool has_more = this->raw.size() > raw_index + 1;
      if (has_more) {
         this->is_invalid = true;
         compiler.raise_error("You cannot access members of a constant integer.");
         return true;
      }
      this->is_resolved = true;
      return true;
   }
   bool VariableReference::_resolve_nested_variable(Compiler& compiler, size_t raw_index) {
      auto prev = this->resolved.top_level.type;
      if (!prev) {
         prev = this->resolved.alias_basis;
         if (!prev) // should never happen
            return false;
      }
      if (!prev->can_have_variables())
         return false;
      if (this->_resolve_aliases_from(compiler, raw_index, prev)) { // handle relative aliases, if any are present
         this->resolution_involved_aliases.nested = true;
      }
      auto part = this->_get_raw_part(raw_index);
      auto type = OpcodeArgTypeRegistry::get().get_variable_type(part->name);
      if (type) {
         auto prev_scope = _var_scope_for_type(*prev);
         assert(prev_scope && "If a variable type doesn't have a VariableScope object, then it shouldn't claim to be able to hold variables.");
         //
         if (!part->has_index()) {
            if (prev->get_property_by_name(part->name)) {
               //
               // There is a property with this name, so if we're not using an index here, then assume 
               // we're referring to the property.
               //
               return false;
            }
            compiler.raise_error(QString("You must indicate which variable you are referring to, using an index, e.g. \"global.%1[0]\" instead of \"global.%1\".").arg(type->internal_name.c_str()));
            this->is_invalid = true;
            return false;
         }
         if (part->index < 0) {
            compiler.raise_error(QString("Typename indices cannot be negative."));
            this->is_invalid = true;
            return false;
         }
         {
            Megalo::variable_type vt = getVariableTypeForTypeinfo(type);
            auto max = prev_scope->max_variables_of_type(vt);
            if (part->index >= max) {
               compiler.raise_error(QString("You specified \"%1[%2]\", but the maximum allowed index is %3.").arg(type->internal_name.c_str()).arg(part->index).arg(max - 1));
               this->is_invalid = true;
               return false;
            }
         }
         //
         this->resolved.nested.type  = type;
         this->resolved.nested.index = part->index;
         return true;
      }
      return false;
   }
   bool VariableReference::_resolve_property(Compiler& compiler, size_t raw_index) {
      auto type = this->get_type();
      this->_resolve_aliases_from(compiler, raw_index, type); // handle relative aliases, if any are present
      auto part = this->_get_raw_part(raw_index);
      auto prop = type->get_property_by_name(part->name);
      if (prop) {
         bool has_index         = part->has_index();
         auto should_have_index = prop->has_index();
         if (has_index != should_have_index) {
            const char* pn = prop->name.c_str();
            const char* tn = type->internal_name.c_str();
            if (should_have_index)
               compiler.raise_error(QString("The \"%1\" property on the %2 type must be indexed.").arg(pn).arg(tn));
            else
               compiler.raise_error(QString("The \"%1\" property on the %2 type cannot be indexed.").arg(pn).arg(tn));
            this->is_invalid = true;
            return false;
         }
         if (has_index)
            this->resolved.property.index = part->index;
         this->resolved.property.definition = prop;
         return true;
      }
      return false;
   }
   bool VariableReference::_resolve_accessor(Compiler& compiler, size_t raw_index) {
      auto type = this->get_type();
      this->_resolve_aliases_from(compiler, raw_index, type); // handle relative aliases, if any are present
      const QString& name = this->_get_raw_part(raw_index)->name;
      auto&       manager = AccessorRegistry::get();
      const auto* entry   = manager.get_by_name(name.toStdString().c_str(), *type);
      if (!entry) {
         assert(type && "Problem in VariableReference::_resolve_accessor; somehow we do not have an identifiable type.");
         entry = manager.get_variably_named_accessor(compiler, name, *type);
      }
      if (entry) {
         this->resolved.accessor      = entry;
         this->resolved.accessor_name = name;
         return true;
      }
      return false;
   }
   void VariableReference::resolve(Compiler& compiler, bool is_alias_definition, bool is_write_access) {
      if (this->is_resolved)
         return;
      //
      // All variable references take one of the two following forms:
      //
      //  - dead_end_value
      //  - var.var.property.accessor, where everything after the first (var) is optional
      //    (e.g. var.property.accessor, var.var.accessor, etc.)
      //
      // Accordingly, once we resolve that first chunk, everything else is simple: check for 
      // an X and handle it if it's present, and then either way, fall through to checking 
      // for a Y; and then a Z.
      //
      auto& res = this->resolved;
      //
      for (auto& part : this->raw) {
         //
         // Let's resolve raw part indices first: if any of them are still strings, then we 
         // need to either resolve them (if they are absolute integer aliases) or error.
         //
         if (!part.resolve_index(compiler))
            this->is_invalid = true;
      }
      if (this->is_invalid)
         return;
      
      if (this->_resolve_aliases_from(compiler, 0)) {
         //
         // We normally call `_resolve_aliases_from` inside of the code that handles each 
         // part of the variable-reference. However, absolute aliases need to be handled 
         // here, so we can handle the case of them resolving to an integer, etc..
         //
         this->resolution_involved_aliases.top_level = true;
         bool has_more = this->raw.size() > 1;
         if (this->is_constant_integer()) {
            if (has_more) {
               compiler.raise_error(QString("Alias \"%1\" resolved to a constant integer. You cannot access members on an integer.").arg(this->raw[0].name));
               this->is_invalid = true;
               return;
            }
            this->is_resolved = true;
            return;
         }
         if (this->_resolve_enumeration(compiler, 1)) // if the absolute alias was an alias of an enum, then the enum value being accessed is at index 1
            return;
      }
      size_t i = this->_resolve_top_level(compiler, is_alias_definition);
      if (this->is_invalid)
         return;
      if (i >= this->raw.size()) {
         if (!is_alias_definition && res.top_level.enumeration) {
            compiler.raise_error("The names of enums cannot be used as values.");
            this->is_invalid = true;
            return;
         }
         this->is_resolved = true;
         return;
      }
      if (this->is_none()) {
         compiler.raise_error("You cannot access the members of a none value.");
         this->is_invalid = true;
         return;
      }
      if (res.top_level.namespace_member.scope) {
         //
         // If the top-level value has a scope listed, then that means we found a namespace member that is, 
         // itself, a fully-resolved reference to a value consisting of a scope, no which, and no index, 
         // such as (game.round_timer). Member access past that point is not possible.
         //
         compiler.raise_error(QString("You cannot access the %1 member on %2.").arg(this->_get_raw_part(i)->name).arg(this->to_string_from_raw(0, i)));
         this->is_invalid = true;
         return;
      }
      if (this->_resolve_enumeration(compiler, i))
         return;
      //
      // The basic plan is to resolve variable parts in the order they can appear: first, the top-level 
      // part; then, the nested variable; then, the property; and then, the accessor.
      //
      if (this->_resolve_nested_variable(compiler, i)) {
         if (++i >= this->raw.size()) {
            this->is_resolved = true;
            return;
         }
      }
      if (this->is_invalid)
         return;
      if (this->_resolve_property(compiler, i)) {
         auto prop = this->resolved.property.definition;
         //
         if (is_write_access && !prop->has_index()) {
            //
            // It's possible for a read-only property to have a setter-accessor. The main example of this 
            // is the "score" property on players and teams; Bungie and 343i never write to it, preferring 
            // instead to use a setter-accessor to change player and team scores.
            //
            if (auto scope = prop->scope) {
               if (scope->is_readonly()) {
                  //
                  // Temporarily wipe the resolved property information, and try to resolve the current 
                  // part as an accessor.
                  //
                  this->resolved.property.definition = nullptr;
                  //
                  if (this->_resolve_accessor(compiler, i)) {
                     if (++i < this->raw.size()) {
                        compiler.raise_error("Attempted to access a member of an accessor.");
                        this->is_invalid = true;
                        return;
                     }
                     this->is_resolved = true;
                     return;
                  }
                  //
                  // If we've made it to here, then the current part did not resolve as an accessor. 
                  // Let's continue handling the property as normal.
                  //
                  this->resolved.property.definition = prop;
               }
            }
         }
         //
         if (auto prev = this->resolved.nested.type) {
            //
            // Not all properties can be accessed as (var.var.property). Enforce this.
            //
            if (prop && !prop->allow_from_nested) {
               compiler.raise_error(
                  QString("The %1 property can only be accessed from a top-level %2 variable. Copy the value (%3) into an intermediate %2 variable and then access the property through that.")
                  .arg(prop->name.c_str())
                  .arg(prev->internal_name.c_str())
                  .arg(this->to_string_from_raw(0, i))
               );
               this->is_invalid = true;
               return;
            }
         }
         if (++i >= this->raw.size()) {
            this->is_resolved = true;
            return;
         }
      }
      if (this->is_invalid)
         return;
      if (this->_resolve_accessor(compiler, i)) {
         if (++i < this->raw.size()) {
            compiler.raise_error("Attempted to access a member of an accessor.");
            this->is_invalid = true;
            return;
         }
         this->is_resolved = true;
         return;
      }
      if (this->is_invalid)
         return;
      //
      // If we reach this point, then the raw part that we're currently looking at is unrecognized. That's 
      // an error, but let's do some work and see if we can't display a good error message to the script 
      // author.
      //
      this->is_invalid = true;
      //
      auto part = this->_get_raw_part(i);
      if (this->resolved.nested.type && !this->resolved.property.definition && !this->resolved.accessor) {
         //
         // Let's check for excess variable depth (e.g. var.var.var) and if that's what's happening here, 
         // show a detailed error message.
         //
         auto prev = this->resolved.nested.type;
         if (prev->can_have_variables()) {
            auto type = OpcodeArgTypeRegistry::get().get_variable_type(part->name);
            if (type) {
               compiler.raise_error(
                  QString("Variable access can only go two levels deep; references of the form (var.var.var) are not possible. Copy the target variable (%1) into an intermediate %2 variable first.")
                     .arg(this->to_string_from_raw(0, i))
                     .arg(prev->internal_name.c_str())
               );
               return;
            }
         }
         compiler.raise_error(QString("The %1 type does not have a member named \"%2\".").arg(prev->internal_name.c_str()).arg(part->name));
         return;
      }
      if (this->raw.size() == 1) {
         auto& first = this->raw[0];
         if (!first.has_index()) {
            //
            // Let's check for an attempt to use an imported name where a variable was expected.
            //
            OpcodeArgTypeRegistry::type_list_t types;
            OpcodeArgTypeRegistry::get().lookup_imported_name(first.name, types);
            if (types.size()) {
               compiler.raise_error(QString("The \"%1\" value cannot appear here.").arg(first.name));
               return;
            }
         }
      }
      auto type = this->get_type();
      if (type)
         compiler.raise_error(QString("The %1 type does not have a member named \"%2\".").arg(type->internal_name.c_str()).arg(part->name));
      else
         compiler.raise_error(QString("Unable to identify the member \"%1\".").arg(part->name));
      return;
   }
   #pragma endregion
}