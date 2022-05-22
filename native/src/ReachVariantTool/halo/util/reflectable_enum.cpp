#include "reflectable_enum.h"

namespace halo::util {
   namespace impl::reflex_enum {
      constexpr auto test = all_names_unique<
         reflex_enum_value<cs("foo")>,
         reflex_enum_value<cs("bar")>
      >::value;
   }

   constexpr auto value_test = impl::reflex_enum::is_value_type<reflex_enum_value<cs("foo")>>;
   using src = reflex_enum_value<cs("foo")>;
   using dst = reflex_enum_value<src::name, src::value>;

   // should fail:
   using invalid_enum = reflex_enum<
      float,
      double
   >;

   using test_enum = reflex_enum<
      reflex_enum_value<cs("bar")>,
      reflex_enum_value<cs("foo")>,
      reflex_enum_value<cs("baz")>,
      reflex_enum_value<cs("ack"), 1234>
   >;
   constexpr auto idx_foo = test_enum::index_of<cs("foo")>;
   constexpr auto idx_bar = test_enum::index_of<cs("bar")>;
   constexpr auto idx_bad = test_enum::index_of<cs("xxxx")>;

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
}