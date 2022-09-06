#pragma once
#include <stdexcept>
#include <utility>
#include "helpers/type_traits/is_std_array.h"
#include "helpers/string/strcmp.h"
#include "helpers/list_items_are_unique.h"
#include "./member.h"
#include "./enumeration/enumeration_data_forward_declare.h"
#include "./enumeration/extract_underlying_type.h"
#include "./enumeration/member_list_from_array.h"
#include "./enumeration/member_list_from_tuple.h"

namespace cobb::reflex3 {
   namespace impl::enumeration {
      template<class Enumeration, typename DefinitionListType> struct member_list;
   }

   template<class Subclass> //requires impl::enumeration::is_valid_enumeration<Subclass>//*/
   class enumeration {
      using my_type = Subclass;

public:
      using member_definition_list_type = std::decay_t<decltype(enumeration_data<my_type>::members)>;
      using member_list = impl::enumeration::member_list<my_type, member_definition_list_type>;

      public:
         using underlying_type = typename impl::enumeration::extract_underlying_type<my_type>::type;

         static constexpr bool has_explicit_underlying_type    = impl::enumeration::has_explicit_underlying_type<my_type>;
         static constexpr bool member_definition_list_is_array = cobb::is_std_array<member_definition_list_type>; // convenience value

         static constexpr auto   all_names             = member_list::all_names;             // std::array<const char*, N>
         static constexpr auto   all_underlying_values = member_list::all_underlying_values; // std::array<underlying_type, N>
         static constexpr size_t value_count           = member_list::value_count;
         
      protected:
         underlying_type _value = 0;

      public: // reflection
         static consteval bool has(const char* name) {
            return member_list::has(name);
         }
         static consteval bool is_range_member(const char* name) {
            return member_list::is_range_member(name);
         }
         static consteval underlying_type max_underlying_value() requires (value_count > 0) {
            return member_list::max_underlying_value();
         }
         static consteval underlying_type min_underlying_value() requires (value_count > 0) {
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