#include "reflex_enum.h"

namespace cobb {
   /*// tests

   constexpr auto value_test = impl::reflex_enum::is_member_type<reflex_enum_member<cs("foo")>>;
   using src = reflex_enum_member<cs("foo")>;
   using dst = reflex_enum_member<src::name, src::value>;


   using test_enum = reflex_enum<
      reflex_enum_member<cs("bar")>,
      reflex_enum_member<cs("foo")>,
      reflex_enum_member<cs("baz")>,
      reflex_enum_member<cs("ack"), 1234>
   >;
   constexpr auto idx_foo = test_enum::index_of<cs("foo")>;
   constexpr auto idx_bar = test_enum::index_of<cs("bar")>;
   constexpr auto idx_bad = test_enum::index_of<cs("xxxx")>;

   using member_test = reflex_enum_member<cs("bar")>;
   constexpr auto test_is_test   = std::is_same_v<std::decay_t<member_test>, reflex_enum_member<member_test::name, member_test::value>>;
   constexpr auto test_is_member = impl::reflex_enum::is_member_type<member_test>;

   using members = test_enum::members;
   using _mat = members::as_tuple;
   using _mat0 = std::tuple_element_t<0, _mat>;

   constexpr auto count = test_enum::member_count;
   constexpr auto names = test_enum::members::all_names;
   constexpr const char* name0b = test_enum::members::nth_type<0>::name.c_str();
   constexpr auto under_a = test_enum::underlying_value_of<(cs("ack"))>;

   constexpr auto value_foo = test_enum::value_of<cs("foo")>;
   constexpr auto value_bar = test_enum::value_of<cs("bar")>;
   constexpr auto value_baz = test_enum::value_of<cs("baz")>;
   constexpr auto value_ack = test_enum::value_of<cs("ack")>;

   constexpr auto test_val = test_enum::make<(cs("ack"))>(); // must wrap the CS object in parentheses to avoid confusing the IntelliSense parser
   constexpr auto test_val_val = test_val.to_int();
   constexpr auto test_val_str = test_val.to_string();
   constexpr auto test_val_str0 = test_val_str[0] == 'a';
   constexpr auto test_val_str1 = test_val_str[1] == 'c';
   constexpr auto test_val_str2 = test_val_str[2] == 'k';
   constexpr auto test_val_eq = test_val.is<(cs("ack"))>();
   constexpr auto test_val_neq = test_val.is<(cs("foo"))>();

   constexpr auto test_val_eq2 = test_val == test_enum::value_of<cs("ack")>;

   */
}