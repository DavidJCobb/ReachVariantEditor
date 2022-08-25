#pragma once
#include <tuple>
#include <type_traits>
#include "type_traits/all_same.h"

namespace cobb {
   template<typename Functor, typename... Args> requires (cobb::all_same<Args...>)
   void for_each_nttp_of_pack(Functor&& f, Args... args) {
      using argument_type = std::tuple_element_t<0, std::tuple<Args...>>;

      if constexpr (std::is_same_v<bool, std::invoke_result_t<Functor, argument_type>>) {
         bool go = true;
         ((go ? go &= f(args) : false), ...);
      } else {
         (f(args), ...);
      }
   }
}