#pragma once
#include <tuple>
#include <type_traits>

namespace cobb {
   namespace impl {
      template<typename T> struct _is_std_tuple {
         static constexpr bool value = false;
      };
      template<typename... Types> struct _is_std_tuple<std::tuple<Types...>> {
         static constexpr bool value = true;
      };
   }
   template<typename T> constexpr bool is_std_tuple = impl::_is_std_tuple<std::decay_t<T>>::value;
}