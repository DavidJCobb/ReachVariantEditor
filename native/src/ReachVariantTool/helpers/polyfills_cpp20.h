#pragma once
#include <cstddef>

#if __cplusplus <= 201703L && !(defined(_MSVC_LANG) || !defined(_HAS_CXX20))
   namespace std {
      template<class T> struct is_bounded_array : std::false_type {};
      template<class T, std::size_t N> struct is_bounded_array<T[N]> : std::true_type {};

      template<class T> inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value;
   }
#endif