#include "../enumeration.h"
#include <array>

namespace cobb::reflex2 {
   class test_enum;
   template<> struct enumeration_members<test_enum> {
      static constexpr auto list = std::array{
         member{ "a" },
         member{ "b" },
         member{ "c" },
         member{ "d", 100 },
         member{ "e" },
      };
   };
   class test_enum : public enumeration<test_enum> {};


   constexpr auto member_count = test_enum::member_count;
   constexpr auto all_underlying = test_enum::all_underlying_values;

   constexpr auto value_a = test_enum::underlying_value_of("a");
   constexpr auto value_b = test_enum::underlying_value_of("b");
   constexpr auto value_c = test_enum::underlying_value_of("c");
   constexpr auto value_d = test_enum::underlying_value_of("d");
   constexpr auto value_e = test_enum::underlying_value_of("e");
   constexpr auto value_f = test_enum::underlying_value_of("BAD");

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
}