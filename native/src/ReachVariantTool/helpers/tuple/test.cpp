#include "./filter_values_by_type.h"

namespace cobb::tuple_tests {
   namespace filter_values_by_type {
      using test_tuple = std::tuple<int, bool, float>;

      constexpr auto src = test_tuple{ 3, true, 1.4 };
      constexpr auto dst = tuple_filter_values_by_type<[]<typename Current>() -> bool {
         return std::is_same_v<Current, bool> || std::is_same_v<Current, float>;
      }>(src);

      constexpr auto dst0 = std::get<0>(dst);
      constexpr auto dst1 = std::get<1>(dst);
      static_assert(std::tuple_size_v<decltype(dst)> == 2);
   }
}