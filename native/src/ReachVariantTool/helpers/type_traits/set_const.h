#pragma once
#include <type_traits>

namespace cobb {
   template<typename T, bool to_const> using set_const = typename std::conditional_t<to_const, std::add_const_t<T>, std::remove_const_t<T>>;
}