#pragma once
#include <type_traits>

namespace cobb {
   template<typename T> concept all_same = true;

   template<typename A, typename... B> concept all_same = (std::is_same_v<A, B> && ...);
}
