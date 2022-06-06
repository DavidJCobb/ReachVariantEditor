#include "./constraints.h"
#include "../member_concepts/names_are_unique.h"

namespace cobb::reflex::tests::enumeration {
   constexpr bool types1 = impl::enumeration::member_types_are_valid<
      member<cobb::cs("A")>,
      member<cobb::cs("B")>,
      member<cobb::cs("C")>,
      member<cobb::cs("D"), 1234>
   >;
   constexpr bool types2 = impl::enumeration::member_types_are_valid<
      int,
      member<cobb::cs("A")>,
      member<cobb::cs("B")>,
      member<cobb::cs("C")>,
      member<cobb::cs("D"), 1234>
   >;

   constexpr bool names1 = member_concepts::names_are_unique<
      member<cobb::cs("A")>,
      member<cobb::cs("B")>,
      member<cobb::cs("C")>,
      member<cobb::cs("D"), 1234>
   >;
   constexpr bool names2 = member_concepts::names_are_unique<
      int,
      member<cobb::cs("A")>,
      member<cobb::cs("B")>,
      member<cobb::cs("C")>,
      member<cobb::cs("D"), 1234>
   >;
   constexpr bool names3 = member_concepts::names_are_unique<
      member<cobb::cs("A")>,
      member_gap,
      member<cobb::cs("B")>,
      member_gap,
      member<cobb::cs("C")>,
      member<cobb::cs("D"), 1234>
   >;
}