#pragma once
#include <type_traits>

namespace cobb {
   template<typename T, bool = std::is_enum_v<T>> struct strip_enum {
   };
   template<typename T> struct strip_enum<T, true> {
      using type = std::underlying_type_t<T>;
   };
   template<typename T> struct strip_enum<T, false> {
      using type = T;
   };
   template<typename T> using strip_enum_t = typename strip_enum<T>::type;
}
