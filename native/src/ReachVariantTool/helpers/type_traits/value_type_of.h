#pragma once
#include <type_traits>

namespace cobb {
   // quick helper e.g. cobb::value_type_of<decltype(this->some_array_or_vector)>
   template<typename T> using value_type_of = typename std::remove_reference_t<T>::value_type;
}