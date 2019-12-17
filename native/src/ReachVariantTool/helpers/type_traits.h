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

   //template<typename T> constexpr const bool has_standard_size_getter = std::is_same_v<decltype(T::size), size_t(T::*)() const noexcept>;

   template<typename T, bool = std::is_same_v<decltype(T::size), size_t(T::*)() const noexcept>> constexpr const bool has_standard_size_getter = false;
   template<typename T> constexpr const bool has_standard_size_getter<T, true> = true;
}
