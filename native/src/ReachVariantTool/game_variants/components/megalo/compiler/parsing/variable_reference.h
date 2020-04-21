#pragma once
#include "base.h"
#include <vector>
#include "../../opcode.h"
#include "../../opcode_arg.h"

namespace Megalo {
   class VariableScopeIndicatorValue;
   class VariableScopeWhichValue;
   //
   namespace Script {
      class Property;

      class Alias;
      class VariableReference : public ParsedItem {
         //
         // VariableReference represents a reference to any variable, or to certain other types' values 
         // e.g. script_widget[0]. It can be used to resolve aliases of variables, properties, accessors, 
         // and integer constants. It notably *cannot* be used to resolve aliases of imported names; if 
         // it encounters one, it will throw an exception.
         //
         protected:
            struct RawPart {
               //
               // Represents a raw segment from the variable. For example, given "global.player[0]" you'd 
               // have two RawParts, one for "global" and another for "player" with index 0.
               //
               QString name;
               QString index_str;
               int32_t index = -1;
               bool    index_is_numeric = false;
               //
               RawPart() {} // needed for std::vector::push_back
               RawPart(QString n) : name(n) {}
               RawPart(QString n, QString i) : name(n), index_str(i) {}
               RawPart(QString n, int32_t i) : name(n), index(i), index_is_numeric(true) {}
               //
               RawPart& operator=(const RawPart& other) noexcept;
               //
               [[nodiscard]] inline bool has_index() const noexcept { return this->index_is_numeric || !this->index_str.isEmpty(); }
               void resolve_index(Compiler&);
            };
            std::vector<RawPart> raw;
            //
         public:
            QString content;
            #if !_DEBUG
               static_assert(false, "replace (content) with logic to actually parse parts");
            #endif
            //
            bool is_resolved = false;
            bool is_invalid  = false;
            //
            struct {
               //
               // A resolved variable-reference can take either of two forms:
               //
               //  - dead_end_value
               //  - var.var.property.accessor, where everything after the first (var) is optional
               //
               const OpcodeArgTypeinfo* alias_basis = nullptr; // only for aliases
               struct {
                  const VariableScopeIndicatorValue* scope = nullptr; // needed for namespace members; indicates a "dead-end" top-level value
                  const VariableScopeWhichValue*     which = nullptr; // needed for namespace members
                  const OpcodeArgTypeinfo* type = nullptr;
                  int32_t index       = 0;     // always present for global variables and static variables; used to hold integer constants; for scopes, used to hold the index if the scope has an index
                  bool    is_static   = false; // if false, then we're accessing a global variable
                  bool    is_constant = false; // true if this is an integer constant
               } top_level;
               struct {
                  const OpcodeArgTypeinfo* type = nullptr;
                  int32_t index = 0; // always present
               } nested;
               struct {
                  const Property* definition = nullptr;
                  int32_t index = 0; // present only if the property uses an index
               } property;
               const AccessorRegistry::Definition* accessor = nullptr;
               QString accessor_name; // needed for VariableReference::to_string, for accessor-arguments e.g. player.frag_grenades
            } resolved;
            //
            VariableReference(QString); // can throw compile_exception
            VariableReference(int32_t);
            //
            void set_to_constant_integer(int32_t);
            [[nodiscard]] const OpcodeArgTypeinfo* get_type() const noexcept; // NOTE: accessors do not have types per se; calling this on an accessor returns the type of whatever the accessor is being invoked on
            //
            [[nodiscard]] inline const OpcodeArgTypeinfo* get_alias_basis_type() const noexcept { return this->resolved.alias_basis; }
            [[nodiscard]] inline bool is_accessor() const noexcept { return this->resolved.accessor; }
            [[nodiscard]] inline bool is_constant_integer() const noexcept { return this->resolved.top_level.is_constant; }
            [[nodiscard]] bool is_property() const noexcept;
            [[nodiscard]] inline const AccessorRegistry::Definition* get_accessor_definition() const noexcept { return this->resolved.accessor; }
            [[nodiscard]] bool is_none() const noexcept;
            [[nodiscard]] bool is_read_only() const noexcept;
            [[nodiscard]] bool is_statically_indexable_value() const noexcept;
            [[nodiscard]] bool is_variable() const noexcept;
            //
            void resolve(Compiler&, bool is_alias_definition = false, bool is_write_access = false); // can throw compile_exception
            //
            [[nodiscard]] QString to_string() const noexcept;
            [[nodiscard]] QString to_string_from_raw(int8_t start = 0, int8_t up_to = -1) const noexcept;
            //
            [[nodiscard]] variable_scope get_containing_scope() const noexcept; // "containing scope," as opposed to "scope enum value"
            [[nodiscard]] const_team to_const_team(bool* success) const noexcept; // only valid if the variable reference refers to a statically-indexable team, or neutral/none
            //
            void strip_accessor() noexcept;
            //
         protected:
            bool _resolve_aliases_from(Compiler&, size_t raw_index, const OpcodeArgTypeinfo* basis = nullptr); // replaces the raw part with the content of a found alias, unless it's an absolute alias that resolves to a constant integer, in which case: sets resolved.top_level
            //
            size_t _resolve_top_level(Compiler&, bool is_alias_definition = false);
            bool   _resolve_nested_variable(Compiler&, size_t raw_index);
            bool   _resolve_property(Compiler&, size_t raw_index);
            bool   _resolve_accessor(Compiler&, size_t raw_index);
            //
            [[nodiscard]] inline RawPart* _get_raw_part(int i) {
               if (i < 0)
                  i += this->raw.size();
               else if (i >= this->raw.size())
                  return nullptr;
               return &this->raw[i];
            }
            void __transclude_alias(uint32_t raw_index, Alias&); // replaces (this->raw[index]) with the contents of the alias. if the alias is a relative alias, does not include the alias typename
      };
   }
}