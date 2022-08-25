#pragma once
#include <type_traits>

namespace cobb {
   namespace impl {
      template<typename...> struct all_same;
      template<> struct all_same<> {
         static constexpr bool value = true;
      };
      template<typename T> struct all_same<T> {
         static constexpr bool value = true;
      };
      template<typename A, typename... B> struct all_same<A, B...> {
         static constexpr bool value = (std::is_same_v<A, B> && ...);
      };
   }

   template<typename... T> concept all_same = impl::all_same<T...>::value;
}
