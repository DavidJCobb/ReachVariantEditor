#include "../enumeration.h"
#include <array>
#include <cstdint>
#include <tuple>

namespace tests {
   /*
   namespace invalid_enum_duplicate_names {
      class test_enum;
      template<> struct cobb__reflex2__enumeration_data<test_enum> {
         using underlying_type = int16_t;
         static constexpr const std::array members = {
            cobb::reflex2::member("a"),
            cobb::reflex2::member("a"),
            cobb::reflex2::member("b"),
            cobb::reflex2::member("b"),
         };
      };
      class test_enum : public cobb::reflex2::enumeration<test_enum> {};

      using t = decltype(cobb__reflex2__enumeration_data<test_enum>::members);
   }
   //*/
   /*
   namespace invalid_enum_unrepresentable_values_basic {
      class test_enum;
      template<> struct cobb__reflex2__enumeration_data<test_enum> {
         using underlying_type = int16_t;
         static constexpr const std::array members = {
            cobb::reflex2::member("a"),
            cobb::reflex2::member("b", 999999),
         };
      };
      class test_enum : public cobb::reflex2::enumeration<test_enum> {};

      using t = decltype(cobb__reflex2__enumeration_data<test_enum>::members);
   }
   namespace invalid_enum_unrepresentable_values_complex {
      class test_enum;
      template<> struct cobb__reflex2__enumeration_data<test_enum> {
         using underlying_type = int16_t;
         inline static constexpr const auto members = std::make_tuple(
            cobb::reflex2::member("a"),
            cobb::reflex2::member("b", 999999)//,
         );
      };
      class test_enum : public cobb::reflex2::enumeration<test_enum> {};

      using t = decltype(cobb__reflex2__enumeration_data<test_enum>::members);
   }
   //*/

   namespace valid_enum_basic {
      class test_enum;
      template<> struct cobb__reflex2__enumeration_data<test_enum> {
         using underlying_type = int16_t;
         static constexpr const std::array members = {
            cobb::reflex2::member("a"),
            cobb::reflex2::member("b"),
            cobb::reflex2::member("c"),
            cobb::reflex2::member("d", 100),
            cobb::reflex2::member("e"),
         };
      };
      class test_enum : public cobb::reflex2::enumeration<test_enum> {};

      constexpr auto member_count = test_enum::member_count;
      constexpr auto all_underlying = test_enum::all_underlying_values;
      using underlying_type = test_enum::underlying_type;

      constexpr auto value_a = test_enum::underlying_value_of("a");
      constexpr auto value_b = test_enum::underlying_value_of("b");
      constexpr auto value_c = test_enum::underlying_value_of("c");
      constexpr auto value_d = test_enum::underlying_value_of("d");
      constexpr auto value_e = test_enum::underlying_value_of("e");

      constexpr auto instance = test_enum::value_of("d");
      constexpr auto instance_name = instance.to_c_str();

      //constexpr auto value_f = test_enum::underlying_value_of("BAD");
   }
   namespace valid_enum_basic_scrambled {
      class test_enum;
      template<> struct cobb__reflex2__enumeration_data<test_enum> {
         using underlying_type = int16_t;
         static constexpr const std::array members = {
            cobb::reflex2::member("e", 101),
            cobb::reflex2::member("b", 1),
            cobb::reflex2::member("a", 0),
            cobb::reflex2::member("c", 2),
            cobb::reflex2::member("d", 100),
         };
      };
      class test_enum : public cobb::reflex2::enumeration<test_enum> {};

      constexpr auto member_count = test_enum::member_count;
      constexpr auto all_underlying = test_enum::all_underlying_values;
      using underlying_type = test_enum::underlying_type;

      constexpr auto value_a = test_enum::underlying_value_of("a");
      constexpr auto value_b = test_enum::underlying_value_of("b");
      constexpr auto value_c = test_enum::underlying_value_of("c");
      constexpr auto value_d = test_enum::underlying_value_of("d");
      constexpr auto value_e = test_enum::underlying_value_of("e");

      constexpr auto instance = test_enum::value_of("a");
      constexpr auto instance_name = instance.to_c_str();
   }
   namespace valid_enum_with_gaps {
      class test_enum;
      template<> struct cobb__reflex2__enumeration_data<test_enum> {
         using underlying_type = int16_t;
         inline static constexpr const auto members = std::make_tuple(
            cobb::reflex2::member("a"),
            cobb::reflex2::member_gap{},
            cobb::reflex2::member("b", 3),
            cobb::reflex2::member_gap{},
            cobb::reflex2::member("c")//,
         );
      };
      class test_enum : public cobb::reflex2::enumeration<test_enum> {};

      constexpr auto member_count   = test_enum::member_count;
      constexpr auto all_underlying = test_enum::all_underlying_values;
      using underlying_type = test_enum::underlying_type;

      constexpr auto value_a = test_enum::underlying_value_of("a");
      constexpr auto value_b = test_enum::underlying_value_of("b");
      constexpr auto value_c = test_enum::underlying_value_of("c");

      constexpr auto instance = test_enum::value_of("c");
      constexpr auto instance_name = instance.to_c_str();

      //constexpr auto value_f = test_enum::underlying_value_of("BAD");
   }
   namespace valid_enum_with_ranges {
      class test_enum;
      template<> struct cobb__reflex2__enumeration_data<test_enum> {
         using underlying_type = int16_t;
         inline static constexpr const auto members = std::make_tuple(
            cobb::reflex2::member("a"),
            cobb::reflex2::member_gap{},
            cobb::reflex2::member("b"),
            cobb::reflex2::member_gap{},
            cobb::reflex2::member("c"),
            cobb::reflex2::member_range("d", 4)//,
         );
      };
      class test_enum : public cobb::reflex2::enumeration<test_enum> {};

      constexpr auto member_count   = test_enum::member_count;
      constexpr auto all_underlying = test_enum::all_underlying_values;
      using underlying_type = test_enum::underlying_type;

      constexpr auto value_a   = test_enum::underlying_value_of("a");
      constexpr auto value_b   = test_enum::underlying_value_of("b");
      constexpr auto value_c   = test_enum::underlying_value_of("c");
      constexpr auto value_d   = test_enum::underlying_value_of("d");
      constexpr auto value_d_0 = test_enum::underlying_value_of("d", 0);
      constexpr auto value_d_1 = test_enum::underlying_value_of("d", 1);
      constexpr auto value_d_2 = test_enum::underlying_value_of("d", 2);
      constexpr auto value_d_3 = test_enum::underlying_value_of("d", 3);

      constexpr auto instance = test_enum::value_of("d", 2);
      constexpr auto instance_name = instance.to_c_str();
   }
}


//
// TODO: Rethink the current design/implementation we have here: we want to 
//       be able to add range and nested-enum members.
// 
// Currently, we use a std::array of values. This array is consteval, so it 
// shouldn't be instantiated at run-time; moreover, with appropriate use of 
// constexpr code, we can ensure that the array itself never exists at run-
// time. This in turn means that we can use any list type we need.
// 
// What if we defined a list type (templated on size) whose elements are 
// variants? This would allow the list to store elements of different types, 
// i.e. `member`, `member_range`, and `nested_enum`. The only caveat is that 
// `nested_enum` would have to be templated on the reflex enum class to use, 
// and so the list would have to be templated as well (deducing all of the 
// reflex enum classes passed to it, if any, and modifying its variant to 
// support all of them).
// 
//