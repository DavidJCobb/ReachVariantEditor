#pragma once
#include <type_traits>

namespace cobb {
   template<typename T, typename Result = int> requires std::is_arithmetic_v<T> constexpr Result sign(T v) {
      return (T(0) < v) - (v < T(0));
   }
}
