#include "../enumeration.h"
#include <array>
#include <cstdint>
#include <tuple>
#include <type_traits>

namespace tests {
   /*
   namespace invalid_enum_duplicate_names {
      class test_enum;
      template<> struct cobb__reflex3__enumeration_data<test_enum> {
         using underlying_type = int16_t;
         static constexpr const std::array members = {
            cobb::reflex3::member("a"),
            cobb::reflex3::member("a"),
            cobb::reflex3::member("b"),
            cobb::reflex3::member("b"),
         };
      };
      class test_enum : public cobb::reflex3::enumeration<test_enum> {};

      using t = decltype(cobb__reflex3__enumeration_data<test_enum>::members);
   }
   //*/
   /*
   namespace invalid_enum_unrepresentable_values_basic {
      class test_enum;
      template<> struct cobb__reflex3__enumeration_data<test_enum> {
         using underlying_type = int16_t;
         static constexpr const std::array members = {
            cobb::reflex3::member("a"),
            cobb::reflex3::member("b", 999999),
         };
      };
      class test_enum : public cobb::reflex3::enumeration<test_enum> {};

      using t = decltype(cobb__reflex3__enumeration_data<test_enum>::members);
   }
   namespace invalid_enum_unrepresentable_values_complex {
      class test_enum;
      template<> struct cobb__reflex3__enumeration_data<test_enum> {
         using underlying_type = int16_t;
         inline static constexpr const auto members = std::make_tuple(
            cobb::reflex3::member("a"),
            cobb::reflex3::member("b", 999999)//,
         );
      };
      class test_enum : public cobb::reflex3::enumeration<test_enum> {};

      using t = decltype(cobb__reflex3__enumeration_data<test_enum>::members);
   }
   //*/

   namespace valid_enum_basic {
      class test_enum;
      template<> struct cobb__reflex3__enumeration_data<test_enum> {
         using underlying_type = int16_t;
         static constexpr const std::array members = {
            cobb::reflex3::member("a"),
            cobb::reflex3::member("b"),
            cobb::reflex3::member("c"),
            cobb::reflex3::member("d", 100),
            cobb::reflex3::member("e"),
         };
      };
      class test_enum : public cobb::reflex3::enumeration<test_enum> {};

      constexpr auto value_count = test_enum::value_count;
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
      template<> struct cobb__reflex3__enumeration_data<test_enum> {
         using underlying_type = int16_t;
         static constexpr const std::array members = {
            cobb::reflex3::member("e", 101),
            cobb::reflex3::member("b", 1),
            cobb::reflex3::member("a", 0),
            cobb::reflex3::member("c", 2),
            cobb::reflex3::member("d", 100),
         };
      };
      class test_enum : public cobb::reflex3::enumeration<test_enum> {};

      constexpr auto value_count = test_enum::value_count;
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
      template<> struct cobb__reflex3__enumeration_data<test_enum> {
         using underlying_type = int16_t;
         inline static constexpr const auto members = std::make_tuple(
            cobb::reflex3::member("a"),
            cobb::reflex3::member_gap{},
            cobb::reflex3::member("b", 3),
            cobb::reflex3::member_gap{},
            cobb::reflex3::member("c")//,
         );
      };
      class test_enum : public cobb::reflex3::enumeration<test_enum> {};

      constexpr auto value_count = test_enum::value_count;
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
      template<> struct cobb__reflex3__enumeration_data<test_enum> {
         using underlying_type = int16_t;
         inline static constexpr const auto members = std::make_tuple(
            cobb::reflex3::member("a"),
            cobb::reflex3::member_gap{},
            cobb::reflex3::member("b"),
            cobb::reflex3::member_gap{},
            cobb::reflex3::member("c"),
            cobb::reflex3::member_range("d", 4)//,
         );
      };
      class test_enum : public cobb::reflex3::enumeration<test_enum> {};

      constexpr auto value_count   = test_enum::value_count;
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
   namespace valid_nested_enum {
      class inner_enum;
      template<> struct cobb__reflex3__enumeration_data<inner_enum> {
         using underlying_type = int16_t;
         inline static constexpr const auto members = std::make_tuple(
            cobb::reflex3::member("x"),
            cobb::reflex3::member_gap{},
            cobb::reflex3::member("y"),
            cobb::reflex3::member_range("z", 4)//,
         );
      };
      class inner_enum : public cobb::reflex3::enumeration<inner_enum> {};

      constexpr auto max_inner = inner_enum::max_underlying_value();

      class outer_enum;
      template<> struct cobb__reflex3__enumeration_data<outer_enum> {
         using underlying_type = int16_t;
         inline static constexpr const auto members = std::make_tuple(
            cobb::reflex3::member("a"),
            cobb::reflex3::member("b"),
            cobb::reflex3::member("c"),
            cobb::reflex3::member_enum<inner_enum>("d", 2)//,
         );
      };
      class outer_enum : public cobb::reflex3::enumeration<outer_enum> {};

      constexpr auto outer_explicit = outer_enum::has_explicit_underlying_type;
using inner_ml = inner_enum::member_list;
using outer_ml = outer_enum::member_list;
constexpr outer_ml foo{};
constexpr auto iml_count = inner_ml::value_count;
constexpr auto oml_count = outer_ml::value_count;

      constexpr auto outer_count = outer_enum::value_count + 0;
      constexpr auto max_outer = outer_enum::max_underlying_value();

      constexpr auto test_x = cobb::reflex3::member_enum<int>("a", 3).value.value();
      constexpr auto test_y = cobb::reflex3::member_enum<inner_enum>("a", 4).value.value();
      using test_y_e = decltype(cobb::reflex3::member_enum<inner_enum>("a", 4))::enumeration;

      constexpr auto test_z = std::make_tuple(cobb::reflex3::member_enum<inner_enum>("a", 4));
      constexpr auto test_w = std::make_tuple(
         cobb::reflex3::member("a"),
         cobb::reflex3::member("b"),
         cobb::reflex3::member("c"),
         cobb::reflex3::member_enum<inner_enum>("d")//, // `template` keyword required to avoid confusing IntelliSense
      );

      using x = cobb::reflex3::enumeration_data<outer_enum>;
      using y = decltype(x::members);
      constexpr const auto& m = x::members;
      static_assert(
         std::is_same_v<
            typename std::tuple_element_t<3, std::decay_t<decltype(cobb::reflex3::enumeration_data<outer_enum>::members)>>::enumeration,
            inner_enum
         >
      );

      // this stuff compiles but intellisense fails
      // would have to toss it into godbolt to know if it's valid (maybe link-time errors?)

      // kinda wanna redo the reflex stuff one last time:
      //  - have the enumeration store a map of underlying values, sorted in ascending order, 
      //    to names and whatever other data we need
      //  - this would allow us to include a nested enum's data in the parent enum in order.

      constexpr auto all_under = outer_enum::all_underlying_values;
      constexpr auto value_a = outer_enum::underlying_value_of("a");
      constexpr auto value_b = outer_enum::underlying_value_of("b");
      constexpr auto value_c = outer_enum::underlying_value_of("c");
      constexpr auto value_d = outer_enum::underlying_value_of("d");

      constexpr auto test = value_a + 2;
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