#pragma once
#include "./member_concepts.h"

namespace cobb::reflex::impl::enumeration {
   template<typename First, typename... Types> concept member_types_are_valid = (
      (is_underlying_type<First> || is_valid_member<First> || std::is_same_v<First, member_gap>) &&
      ((is_valid_member<Types> || std::is_same_v<Types, member_gap>) && ...)
   );

   template<typename A, typename... B> class member_names_are_unique_worker {
      protected:
         template<typename A, typename B> struct compare {
            static constexpr bool value = true;
         };
         template<typename A, typename B> requires (is_named_member<A> && is_named_member<B>) struct compare<A, B> {
            static constexpr bool value = (A::name == B::name);
         };

      public:
         static constexpr bool value = ([]() {
            if constexpr (is_named_member<A>) {
               bool a_overlaps = (compare<A, B>::value || ...);
               if (a_overlaps)
                  return false;
            }
            if constexpr (sizeof...(B) > 1) {
               return member_names_are_unique_worker<B...>::value;
            }
            return true;
         })();
   };
   template<is_named_member A> class member_names_are_unique_worker<A> {
      public:
         static constexpr bool value = true;
   };
   //
   template<typename... Types> concept member_names_are_unique = member_names_are_unique_worker<Types...>::value;

   template<typename Underlying, typename Member> constexpr bool member_explicit_value_is_representable() {
      if constexpr (std::is_same_v<Member, member_gap>) {
         return true;
      }
      //
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

   template<typename First, typename... Types> concept member_explicit_values_are_representable =
      (is_underlying_type<First> ?
         (member_explicit_value_is_representable<First, Types>() && ...)
      :
         member_explicit_value_is_representable<default_underlying_enum_type, First>() && (member_explicit_value_is_representable<default_underlying_enum_type, Types>() && ...)
      );
}
