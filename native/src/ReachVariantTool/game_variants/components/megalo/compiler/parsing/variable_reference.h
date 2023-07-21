#pragma once
#include "base.h"
#include <expected>
#include <variant>
#include <vector>
#include "../../opcode.h"
#include "../../opcode_arg.h"

namespace Megalo {
   class VariableScopeIndicatorValue;
   class VariableScopeWhichValue;
   //
   namespace Script {
      class Enum;
      class Namespace;
      class NamespaceMember;
      class Property;

      class Alias;
      class VariableReference : public ParsedItem {
         //
         // VariableReference represents a reference to entities of any of the following types:
         // 
         //  - variables
         //     - number
         //     - object
         //     - player
         //     - team
         //     - timer
         //  - properties of variables
         //  - accessors  on variables
         //  - integer constants (as in compiled Megalo, these "work" the same as number variables)
         //  - widgets
         // 
         // If the naming seems confusing, that is likely because the term "register" would be more 
         // appropriate than "variable" here.
         // 
         // This class *is not* used to resolve aliases of imported names (e.g. `none` as a variant 
         // string ID); if it encounters an imported name, it will throw an exception. User-defined 
         // aliases have to store the imported name separately.
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
               bool resolve_index(Compiler&);
            };
            std::vector<RawPart> raw;

            #pragma region REWRITE 7-21-2023 -- STRUCTS
            template<typename T> struct try_resolve_result {
               try_resolve_result() {}
               try_resolve_result(const T& d) : data(d) {}
               try_resolve_result(const QString& s) {
                  this->errors.push_back(s);
               }

               std::optional<T> data;
               std::vector<QString> errors;
            };

         public:
            struct namespace_member {
               const OpcodeArgTypeinfo* type   = nullptr;
               const NamespaceMember*   target = nullptr;
               std::optional<int> index;
            };

            struct nested_variable_reference {
               const OpcodeArgTypeinfo* type = nullptr;
               std::optional<int> index;

               static try_resolve_result<nested_variable_reference> try_resolve(const RawPart&, const OpcodeArgTypeinfo& member_of);
            };
            struct variable_property_reference {
               const Property* definition = nullptr;
               std::optional<int> index;

               static try_resolve_result<variable_property_reference> try_resolve(const RawPart&, const OpcodeArgTypeinfo& member_of);
            };
            struct variable_accessor_reference {
               const AccessorRegistry::Definition* definition = nullptr;
               QString accessor_name; // needed for VariableReference::to_string, for accessor-arguments e.g. player.frag_grenades

               static std::optional<variable_accessor_reference> try_resolve(const RawPart&, const OpcodeArgTypeinfo& member_of);
            };

            struct member_reference_details {

               //
               // Nested:
               //    <...>.<type>[n]
               //
               nested_variable_reference nested;

               variable_property_reference property;
               variable_accessor_reference accessor;
            };

            struct static_variable_reference { // e.g. player[0], team[1]
               const OpcodeArgTypeinfo* type = nullptr;
               int index = 0;

               static try_resolve_result<static_variable_reference> try_resolve(const RawPart&);
            };
            struct non_member_indexed_variable_reference { // e.g. global.number[3], temporaries.object[1]
               const Namespace* containing_namespace = nullptr; // `global` or `temporaries`
               const OpcodeArgTypeinfo* type = nullptr;
               int index = 0;

               static try_resolve_result<non_member_indexed_variable_reference> try_resolve(const RawPart&, const Namespace&);
            };

            class top_level_variable_reference : public std::variant<
               std::monostate,
               namespace_member,
               non_member_indexed_variable_reference,
               static_variable_reference
            > {
               public:
                  const OpcodeArgTypeinfo* megalo_type() const {
                     if (auto* self = std::get_if<namespace_member>(this))
                        return self->type;
                     if (auto* self = std::get_if<non_member_indexed_variable_reference>(this))
                        return self->type;
                     if (auto* self = std::get_if<static_variable_reference>(this))
                        return self->type;
                     return nullptr;
                  }
            };
            
            struct enumeration {
               const Enum* built_in = nullptr; // during parsing/resolution, this will also hold user-defined enum pointers, but we don't retain those since they may be deleted out from under us
               int32_t     value    = 0;

               // TODO: `built_in`: how would a UDE be deleted out from under us? if we're an alias, then we're block-scoped, too; 
               //       we can't refer to any UDE that would go out of scope before we would, though it may go out of scope at the 
               //       same time that we do. if we're not an alias, are we even retained past compiling individual `Statement`s?
            };
            struct integer_constant {
               int16_t value = 0;
            };
            struct none {
               const OpcodeArgTypeinfo* type = nullptr;
            };
            struct variable : public member_reference_details {
               top_level_variable_reference top_level;
            };
            struct relative_alias : public member_reference_details {
               const OpcodeArgTypeinfo* alias_basis = nullptr;
            };
            #pragma endregion

         public:
            QString content; // only retained for debugging
            //
            bool is_resolved = false;
            bool is_invalid  = false;

            #pragma region REWRITE 7-21-2023
            using resolved_variant = std::variant<
               std::monostate,
               enumeration,
               integer_constant,
               namespace_member,
               none,
               relative_alias,
               variable
            >;
            resolved_variant new_resolved_data;
            bool fully_resolved = false;
            struct {
               bool target_not_identifiable = false; // we know the intended type of the reference, but cannot meaningfully identify what entity of that type is being referenced
               bool type_not_identifiable   = false; // implies target not identifiable
            } invalidity;
            //
            bool _resolve_new_aliases_from(Compiler&, size_t raw_index, const OpcodeArgTypeinfo* basis = nullptr); // replaces the raw part with the content of a found alias, unless it's an absolute alias that resolves to a constant integer, in which case: sets resolved.top_level
            size_t _resolve_new_top_level(Compiler&, bool is_alias_definition = false);
            void _resolve_new(Compiler&, bool is_alias_definition = false, bool is_write_access = false);
            #pragma endregion
            
            struct {
               //
               // A resolved variable-reference can take either of two forms:
               //
               //  - dead_end_value
               //  - var.var.property.accessor, where everything after the first (var) is optional
               //
               const OpcodeArgTypeinfo* alias_basis = nullptr; // only for aliases
               struct {
                  const OpcodeArgTypeinfo* type = nullptr;

                  const Enum* enumeration = nullptr;
                  struct {
                     const VariableScopeIndicatorValue* scope = nullptr; // for "dead-end" values that are members of any namespace, including the unnamed namespace, e.g. `no_team`, `current_player`
                     const VariableScopeWhichValue*     which = nullptr;
                  } namespace_member;
                  int32_t index        = 0;     // always present for global variables and static variables; used to hold integer constants; for scopes, used to hold the index if the scope has an index
                  bool    is_static    = false; // if false, then we're accessing a global variable
                  bool    is_constant  = false; // true if this is an integer constant
                  bool    is_temporary = false; // disambiguates global.type[n] from temporaries.type[n]

                  bool via_alias = false;
               } top_level;
               struct {
                  const OpcodeArgTypeinfo* type = nullptr;
                  int32_t index = 0; // always present

                  bool via_alias = false;
               } nested;
               struct {
                  const Property* definition = nullptr;
                  int32_t index = 0; // present only if the property uses an index

                  bool via_alias = false;
               } property;
               const AccessorRegistry::Definition* accessor = nullptr;
               QString accessor_name; // needed for VariableReference::to_string, for accessor-arguments e.g. player.frag_grenades
            } resolved;
            
            VariableReference(Compiler&, QString); // can throw compile_exception
            VariableReference(int32_t);
            //
            void set_to_constant_integer(int32_t);

            [[nodiscard]] const OpcodeArgTypeinfo* get_type() const noexcept; // NOTE: accessors do not have types per se; calling this on an accessor returns the type of whatever the accessor is being invoked on
            
            constexpr const AccessorRegistry::Definition* get_accessor_definition() const noexcept;
            constexpr const OpcodeArgTypeinfo* get_alias_basis_type() const noexcept;
            constexpr int32_t get_constant_integer() const noexcept;
            constexpr bool is_accessor() const noexcept;
            constexpr bool is_constant_integer() const noexcept;
            constexpr bool is_namespace_member() const noexcept;
            constexpr bool is_none() const noexcept;
            constexpr bool is_property() const noexcept;
            [[nodiscard]] bool is_read_only() const noexcept;
            constexpr bool is_statically_indexable_value() const noexcept; // Tests if it's a direct reference to a static value -- NOT a reference to a member of a static value.
            constexpr bool is_variable() const noexcept;
            
            void resolve(Compiler&, bool is_alias_definition = false, bool is_write_access = false); // can throw compile_exception
            //
            [[nodiscard]] QString to_string() const noexcept;
            [[nodiscard]] QString to_string_from_raw(int8_t start = 0, int8_t up_to = -1) const noexcept;
            //
            [[nodiscard]] variable_scope get_containing_scope() const noexcept; // "containing scope," as opposed to "scope enum value"
            [[nodiscard]] const_team to_const_team(bool* success) const noexcept; // only valid if the variable reference refers to a statically-indexable team, or neutral/none
            //
            void strip_accessor() noexcept;
            
            // MegaloEdit uses the term "transient" for any variable that doesn't exist across time, e.g. `current_player` or `temporaries.number[0]`
            bool is_transient() const;

            bool is_usable_in_pregame() const;
            
         protected:
            bool _resolve_aliases_from(Compiler&, size_t raw_index, const OpcodeArgTypeinfo* basis = nullptr); // replaces the raw part with the content of a found alias, unless it's an absolute alias that resolves to a constant integer, in which case: sets resolved.top_level
            //
            size_t _resolve_top_level(Compiler&, bool is_alias_definition = false);
            bool   _resolve_enumeration(Compiler&, size_t raw_index); // identifies what value we're accessing, after _resolve_top_level gets the enum itself; returns true if an enum was detected, even if we ran into an error trying to deal with it
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

#include "./variable_reference.inl"