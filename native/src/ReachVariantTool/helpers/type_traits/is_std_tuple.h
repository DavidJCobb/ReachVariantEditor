#pragma once
#include <tuple>
#include <type_traits>

namespace cobb {
   namespace impl::is_std_tuple {
      template<typename T> struct result {
         static constexpr bool value = false;
      };
      template<typename... Types> struct result<std::tuple<Types...>> {
         static constexpr bool value = true;
      };
   }
   template<typename T> constexpr bool is_std_tuple = impl::is_std_tuple<std::decay_t<T>>;
}