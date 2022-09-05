#pragma once
#include <stdexcept>
#include <utility>
#include "helpers/type_traits/is_std_array.h"
#include "helpers/string/strcmp.h"
#include "helpers/list_items_are_unique.h"
#include "./member.h"
#include "./member_range.h"
#include "./enumeration/enumeration_data_forward_declare.h"
#include "./enumeration/extract_underlying_type.h"
#include "./enumeration/member_list_from_array.h"
#include "./enumeration/member_list_from_tuple.h"

namespace cobb::reflex2 {
   namespace impl::enumeration {
      template<class Enumeration, typename DefinitionListType> struct member_list;

      template<class Enumeration> using member_list_helper_type = member_list<Enumeration, std::decay_t<decltype(enumeration_data<Enumeration>::members)>>;

      template<class Enumeration> concept enum_defines_members = requires {
         typename enumeration_data<Enumeration>;
         enumeration_data<Enumeration>::members;

         typename member_list_helper_type<Enumeration>;
      };
      template<class Enumeration> constexpr bool enum_has_no_duplicate_names = []() { // making this a concept fails silently; dunno why; MSVC issue with executing lambdas in concepts maybe?
         using ml = member_list_helper_type<Enumeration>;
         return cobb::list_items_are_unique(ml::all_names, [](const char* a, const char* b) { return (a != b) && cobb::strcmp(a, b) != 0; });
      }();
      template<class Enumeration>concept enum_has_no_unrepresentable_values = requires {
         typename member_list_helper_type<Enumeration>;
         requires (!member_list_helper_type<Enumeration>::any_values_unrepresentable);
      };

      // Pick your poison: static assertions allow you to know what's wrong, but not what template is affected; concepts 
      //                   allow you to know what template (i.e. enum) is wrong, but not how. Fun.
      template<class Enumeration> concept is_valid_enumeration = requires {
         requires enum_defines_members<Enumeration>;
         requires enum_has_no_duplicate_names<Enumeration>;
         requires enum_has_no_unrepresentable_values<Enumeration>;
      };
   }

   template<class Subclass> requires impl::enumeration::is_valid_enumeration<Subclass>//*/
   class enumeration {
      using my_type     = Subclass;

public:
      using member_definition_list_type = std::decay_t<decltype(enumeration_data<my_type>::members)>;
      using member_list = impl::enumeration::member_list<my_type, member_definition_list_type>;

      public:
         using underlying_type = typename impl::enumeration::extract_underlying_type<my_type>::type;

         static constexpr bool has_explicit_underlying_type    = impl::enumeration::has_explicit_underlying_type<my_type>;
         static constexpr bool member_definition_list_is_array = cobb::is_std_array<member_definition_list_type>; // convenience value

         static constexpr auto   all_names             = member_list::all_names;             // std::array<const char*, N>
         static constexpr auto   all_underlying_values = member_list::all_underlying_values; // std::array<underlying_type, N>
         static constexpr size_t member_count          = member_list::member_count;
         
         static_assert(
            cobb::list_items_are_unique(all_names, [](const char* a, const char* b) { return (a != b) && cobb::strcmp(a, b) != 0; }),
            "No two enum members can have the same name."
         );
         static_assert(
            !member_list::any_values_unrepresentable,
            "This enum contains values that the underlying type cannot store."
         );

      protected:
         underlying_type _value = 0;

      public: // reflection
         static consteval bool has(const char* name) {
            return member_list::has(name);
         }
         static consteval bool is_range_member(const char* name) {
            return member_list::is_range_member(name);
         }
         static consteval underlying_type max_underlying_value() requires (member_count > 0) {
            return member_list::max_underlying_value();
         }
         static consteval underlying_type min_underlying_value() requires (member_count > 0) {
            return member_list::min_underlying_value();
         }
         static consteval underlying_type underlying_value_range() {
            return max_underlying_value() - min_underlying_value() + 1;
         }
         static consteval underlying_type underlying_value_of(const char* name) {
            return member_list::underlying_value_of(name);
         }

         // Overload for range members.
         static consteval underlying_type underlying_value_of(const char* name, size_t range_index) {
            return member_list::underlying_value_of(name, range_index);
         }

      public: // construction
         constexpr enumeration() {}
         explicit constexpr enumeration(underlying_type v) : _value(v) {}
         template<typename V> requires std::is_convertible_v<V, underlying_type> constexpr static enumeration from_int(V v) {
            enumeration out;
            out._value = v;
            return out;
         }

         static consteval enumeration min_value() { return from_int(min_underlying_value()); }
         static consteval enumeration max_value() { return from_int(max_underlying_value()); }

         static consteval enumeration value_of(const char* name) {
            return enumeration{ underlying_value_of(name) };
         }

         // Overload for range members.
         static consteval enumeration value_of(const char* name, size_t range_index) {
            return enumeration{ underlying_value_of(name, range_index) };
         }

      public: // instance members
         constexpr underlying_type to_int() const {
            return this->_value;
         }
         constexpr const char* to_c_str() const {
            return member_list::name_of_value(this->_value);
         }
         std::string to_string() const {
            auto p = to_c_str();
            return p ? p : std::string{};
         }

         explicit operator underlying_type() const { return this->_value; }

         constexpr bool operator==(const enumeration&) const = default; // in C++20, implicitly defines != as well
   };
}