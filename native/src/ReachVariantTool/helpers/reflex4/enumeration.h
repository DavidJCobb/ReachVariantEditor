#pragma once
#include <stdexcept>
#include <utility>
#include "helpers/array/cast.h"
#include "helpers/polyfills/msvc/const_reference_nttp_decltype.h"
#include "helpers/type_traits/is_std_array.h"
#include "helpers/string/strcmp.h"
#include "helpers/list_items_are_unique.h"
#include "./member.h"
#include "./enumeration/member_list_from_array.h"
#include "./enumeration/member_list_from_tuple.h"

namespace cobb::reflex4 {
   namespace impl::enumeration {
      template<const auto& V> concept is_enumeration_member_list =
            cobb::is_std_array_of_type<cobb::polyfills::msvc::nttp_decltype<V>, member>
         || cobb::is_std_tuple<cobb::polyfills::msvc::nttp_decltype<V>>;

      template<typename T> struct member_list_type_resolver {
         template<const auto& Data> using type = void;
      };
      template<size_t N> struct member_list_type_resolver<std::array<member, N>> {
         template<const auto& Data> using type = member_list_from_array<Data>;
      };
      template<typename... Types> struct member_list_type_resolver<std::tuple<Types...>> {
         template<const auto& Data> using type = member_list_from_tuple<Data>;
      };

      template<const auto& Data> struct member_list_helper {
         using data_type = std::decay_t<cobb::polyfills::msvc::nttp_decltype<Data>>;
         using type = typename member_list_type_resolver<data_type>::template type<Data>;
      };

   }

   template<class Subclass, typename Underlying, const auto& MemberDefinitions>
   class enumeration {
      using my_type = Subclass;

      static constexpr const auto& member_definitions = MemberDefinitions;
      using member_definition_list_type = std::decay_t<cobb::polyfills::msvc::nttp_decltype<member_definitions>>;
      using member_list = impl::enumeration::member_list_helper<MemberDefinitions>::type;

      static constexpr bool _definition_is_valid() {
         //
         // Don't actually call this function. The reason we use a non-consteval function is so we can 
         // abuse __FUNCTION__, on supported compilers, to show better diagnostics.
         // 
         //  - Static assertions tell you what's wrong, but not what template is wrong.
         // 
         //  - Concepts tell you what template is wrong, but not what's wrong with it.
         // 
         // On compilers that treat __FUNCTION__ as a macro, we can use string literal combining to 
         // append the template name (by way of the name of this function) to the static assertion's 
         // error message. We have to test whether __FUNCTION__ is a macro first, as some compilers 
         // implement it differently.
         //
         static_assert(
            member_list::validity_check_results.members_tuple_has_only_relevant_types,
            "The \"members\" tuple contains types unrelated to reflex enums."
            #if defined(__FUNCTION__)
               " (" __FUNCTION__ ")"
            #endif
         );
         static_assert(
            member_list::names_are_unique,
            "Name collision among this enum's members."
            #if defined(__FUNCTION__)
               " (" __FUNCTION__ ")"
            #endif
         );
         static_assert(
            member_list::names_are_not_blank,
            "Enum contains members with blank names."
            #if defined(__FUNCTION__)
               " (" __FUNCTION__ ")"
            #endif
         );
         static_assert(
            []() consteval -> bool {
               for (const auto v : member_list::all_underlying_values)
                  if (static_cast<underlying_type>(v) != v)
                     return true;
               return false;
            }(),
            "Enum contains members whose values are not representable (do not fit) in the underlying type."
            #if defined(__FUNCTION__)
               " (" __FUNCTION__ ")"
            #endif
         );
         return true;
      }

      public:
         using underlying_type = Underlying;

         static constexpr bool has_explicit_underlying_type    = true;
         static constexpr bool member_definition_list_is_array = cobb::is_std_array<member_definition_list_type>; // convenience value

         static constexpr auto   all_names             = member_list::all_names; // std::array<const char*, N>
         static constexpr auto   all_underlying_values = cobb::array::cast<underlying_type>(member_list::all_underlying_values); // std::array<underlying_type, N>
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
            return static_cast<underlying_type>(member_list::max_underlying_value());
         }
         static consteval underlying_type min_underlying_value() requires (value_count > 0) {
            return static_cast<underlying_type>(member_list::min_underlying_value());
         }
         static consteval underlying_type underlying_value_range() {
            return static_cast<underlying_type>(max_underlying_value() - min_underlying_value() + 1);
         }
         static consteval underlying_type underlying_value_of(const char* name) {
            return static_cast<underlying_type>(member_list::underlying_value_of(name));
         }

         // Overload for range members.
         static consteval underlying_type underlying_value_of(const char* name, size_t range_index) {
            return static_cast<underlying_type>(member_list::underlying_value_of(name, range_index));
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