#pragma once
#include "./member_concepts.h"

namespace cobb::reflex::impl::flags_mask {
   template<typename First, typename... Types> concept member_types_are_valid = (
      (is_underlying_type<First> || is_valid_member<First> || std::is_same_v<First, member_gap>) &&
      ((is_valid_member<Types> || std::is_same_v<Types, member_gap>) && ...)
   );

   template<typename Underlying, typename Member> constexpr bool member_explicit_value_is_representable() {
      if constexpr (std::is_same_v<Member, member_gap>) {
         return true;
      } else {
         static_assert(is_underlying_type<Underlying>);
         using limits = std::numeric_limits<Underlying>;
         //
         if constexpr (is_member<Member> || is_member_range<Member>) {
            constexpr auto value = Member::value;
            if (value == undefined)
               return true;
            return (value >= limits::lowest()) && (value <= limits::max());
         }
         return true;
      }
   }
   template<typename First, typename... Types> struct member_explicit_values_are_representable_worker;
   template<typename First, typename... Types> requires (is_underlying_type<First>) struct member_explicit_values_are_representable_worker<First, Types...> {
      static constexpr bool value = (member_explicit_value_is_representable<First, Types>() && ...);
   };
   template<typename First, typename... Types> requires (!is_underlying_type<First>) struct member_explicit_values_are_representable_worker<First, Types...> {
      static constexpr bool value = member_explicit_value_is_representable<default_underlying_enum_type, First>() && (member_explicit_value_is_representable<default_underlying_enum_type, Types>() && ...);
   };

   template<typename First, typename... Types> concept member_explicit_values_are_representable = member_explicit_values_are_representable_worker<First, Types...>::value;
}
