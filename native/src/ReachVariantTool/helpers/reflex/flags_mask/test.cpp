#include "../flags_mask.h"

namespace cobb::reflex::tests::flags_mask {
   namespace test_basic {
      using test_mask = ::cobb::reflex::flags_mask<
         member<cobb::cs("A")>,
         member<cobb::cs("B")>,
         member<cobb::cs("C")>,
         member<cobb::cs("D"), 7>
      >;

      using underlying = test_mask::underlying_type;
      constexpr auto explicit_underlying_type = test_mask::explicit_underlying_type;

      static_assert(test_mask::has<cobb::cs("A")>);
      static_assert(test_mask::has<cobb::cs("XXX")> == false);

      constexpr auto all_underlying = test_mask::all_underlying_values;
      constexpr auto all_indices    = test_mask::all_indices;

      constexpr auto underlying_a = test_mask::underlying_value_of<cobb::cs("A")>;
      constexpr auto underlying_b = test_mask::underlying_value_of<cobb::cs("B")>;
      constexpr auto underlying_c = test_mask::underlying_value_of<cobb::cs("C")>;
      constexpr auto underlying_d = test_mask::underlying_value_of<cobb::cs("D")>;

      constexpr auto test_modify = [](){
         test_mask mask = {};
         mask.modify_flags<cobb::cs("B"), cobb::cs("D")>(true);
         return mask.to_int();
      }();
   }

   namespace test_gaps {
      using test_mask = ::cobb::reflex::flags_mask<
         member<cobb::cs("A")>,
         member_gap,
         member<cobb::cs("B")>,
         member_gap,
         member<cobb::cs("C")>,
         member_gap,
         member<cobb::cs("D"), 7>
      >;

      using underlying = test_mask::underlying_type;

      static_assert(test_mask::has<cobb::cs("A")>);
      static_assert(test_mask::has<cobb::cs("XXX")> == false);

      constexpr auto all_underlying = test_mask::all_underlying_values;

      constexpr auto underlying_a = test_mask::underlying_value_of<cobb::cs("A")>;
      constexpr auto underlying_b = test_mask::underlying_value_of<cobb::cs("B")>;
      constexpr auto underlying_c = test_mask::underlying_value_of<cobb::cs("C")>;
      constexpr auto underlying_d = test_mask::underlying_value_of<cobb::cs("D")>;
   }

   namespace test_range {
      using test_mask = cobb::reflex::flags_mask<
         member<cobb::cs("A")>,
         member_range<cobb::cs("B"), 3>,
         member<cobb::cs("C")>
      >;

      using underlying = test_mask::underlying_type;

      constexpr bool test = is_member_range<member_range<cobb::cs("B"), 3>>;

      static_assert(is_member_range<member_range<cobb::cs("B"), 3>>);

      static_assert(test_mask::has<cobb::cs("A")>);
      static_assert(test_mask::has<cobb::cs("B")>);
      static_assert(test_mask::has<cobb::cs("C")>);

      constexpr auto all_underlying = test_mask::all_underlying_values;

      constexpr auto underlying_a = test_mask::underlying_value_of<cobb::cs("A")>;
      constexpr auto underlying_b = test_mask::underlying_value_of<cobb::cs("B")>;
      constexpr auto underlying_c = test_mask::underlying_value_of<cobb::cs("C")>;

      using typeof_b = test_mask::member_type<cobb::cs("B")>;

      constexpr auto underlying_b0 = test_mask::underlying_value_of<cobb::cs("B"), 0>;
      constexpr auto underlying_b1 = test_mask::underlying_value_of<cobb::cs("B"), 1>;
      constexpr auto underlying_b2 = test_mask::underlying_value_of<cobb::cs("B"), 2>;
      //constexpr auto underlying_c2 = test_mask::underlying_value_of<cobb::cs("C"), 2>;
   }

   namespace test_explicit_underlying_type {
      using test_mask = cobb::reflex::flags_mask<
         int16_t,
         member<cobb::cs("A")>,
         member<cobb::cs("B")>,
         member<cobb::cs("C")>,
         member<cobb::cs("D"), 15>
      >;

      using underlying = test_mask::underlying_type;

      constexpr auto all_underlying = test_mask::all_underlying_values;
   }

   namespace test_metadata {
      struct metadata {
         int id = {};
      };
      
      using test_mask = cobb::reflex::flags_mask<
         member<cobb::cs("A"), undefined, metadata{ .id = 123 }>,
         member<cobb::cs("B")>,
         member<cobb::cs("C"), undefined, metadata{ .id = 456 }>,
         member<cobb::cs("D"), 7>
      >;

      using metadata_type = test_mask::metadata_type;
      static_assert(std::is_same_v<metadata_type, test_mask::member_type<cobb::cs("A")>::metadata_type>);

      constexpr auto value = test_mask::value_of<cobb::cs("C")>;
      constexpr auto multi = test_mask::from<
         cobb::cs("A"),
         cobb::cs("B"),
         cobb::cs("C")
      >;

      constexpr auto total = [](){
         auto value = test_mask::from<
            cobb::cs("A"),
            cobb::cs("B"),
            cobb::cs("C")
         >;
         //
         int total = 0;
         value.for_each_metadata([&total](const metadata& md) {
            total += md.id;
         });
         return total;
      }();
   }

   namespace test_all_constraints {
      template<typename... Types> concept is_valid_specialization = requires {
         typename cobb::reflex::flags_mask<Types...>;
      };
      static_assert(
         is_valid_specialization<
            member<cobb::cs("A")>,
            member<cobb::cs("B")>
         >,
         "Sanity check for is_valid_specialization concept."
      );


      static_assert(
         false == is_valid_specialization<
            void,
            void,
            void
         >,
         "Test failed: bad type params"
      );
      static_assert(
         false == is_valid_specialization<
            uint8_t,
            member<cobb::cs("A"), 1234>
         >,
         "Test failed: member with explicit value not representable in underlying type"
      );
      static_assert(
         false == is_valid_specialization<
            member<cobb::cs("A")>,
            member<cobb::cs("B")>,
            member<cobb::cs("B")>
         >,
         "Test failed: members with non-unique names"
      );
   }
}