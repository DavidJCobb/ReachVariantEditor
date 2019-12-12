#pragma once
#include <type_traits>

namespace cobb {
   template<typename T, bool = std::is_enum_v<T>> struct enum_or_int_to_int {
   };
   template<typename T> struct enum_or_int_to_int<T, true> {
      using type = std::underlying_type_t<T>;
   };
   template<typename T> struct enum_or_int_to_int<T, false> {
      using type = T;
   };
   template<typename T> using enum_or_int_to_int_t = typename enum_or_int_to_int<T>::type;
}
