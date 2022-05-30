#include "./constraints.h"

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

   constexpr bool names1 = impl::enumeration::member_names_are_unique<
      member<cobb::cs("A")>,
      member<cobb::cs("B")>,
      member<cobb::cs("C")>,
      member<cobb::cs("D"), 1234>
   >;
   constexpr bool names2 = impl::enumeration::member_names_are_unique<
      int,
      member<cobb::cs("A")>,
      member<cobb::cs("B")>,
      member<cobb::cs("C")>,
      member<cobb::cs("D"), 1234>
   >;
   constexpr bool names3 = impl::enumeration::member_names_are_unique<
      member<cobb::cs("A")>,
      member_gap,
      member<cobb::cs("B")>,
      member_gap,
      member<cobb::cs("C")>,
      member<cobb::cs("D"), 1234>
   >;
}