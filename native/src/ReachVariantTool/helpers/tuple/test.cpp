#include "./filter_values_by_type.h"
#include "helpers/tuple/consteval_make.h"

namespace cobb::tuple_tests {
   namespace filter_values_by_type {
      using test_tuple = std::tuple<int, bool, float>;

      constexpr auto src = test_tuple{ 3, true, 1.4F };
      constexpr auto dst = tuple_filter_values_by_type<[]<typename Current>() -> bool {
         return std::is_same_v<Current, bool> || std::is_same_v<Current, float>;
      }>(src);

      constexpr auto dst0 = std::get<0>(dst);
      constexpr auto dst1 = std::get<1>(dst);
      static_assert(std::tuple_size_v<decltype(dst)> == 2);
   }
}

#include <cstdint>
#include "helpers/polyfills/msvc/consteval_compatible_optional.h"
namespace test {
   using member_value = std::intmax_t;
   struct member {
      constexpr member() {}
      constexpr member(const char* const n) : name(n) {}
      consteval member(const char* const n, member_value v) : name(n), value(v) {}

      const char* name = nullptr;
      cobb::polyfills::msvc::consteval_compatible_optional<member_value> value;
   };

   struct foo {
      inline static constexpr const auto barnacles = cobb::consteval_make_tuple(
         member("a"),
         member("b", 999999)//,
      );
   };
}