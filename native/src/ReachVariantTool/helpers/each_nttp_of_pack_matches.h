#pragma once
#include <tuple>
#include <type_traits>
#include "type_traits/all_same.h"

namespace cobb {
   template<typename Functor, typename... Args> requires (
      cobb::all_same<Args...>
   && std::is_same_v<
         bool,
         std::invoke_result_t<
            Functor,
            std::tuple_element_t<0, std::tuple<Args...>>
         >
      >
   )
   void each_nttp_of_type_matches(Functor&& f, Args... args) {
      bool match = true;
      ((match ? match &= f(args) : false), ...);
      return match;
   }
   
   // Variant wherein the functor is called with the element index and the element to match.
   template<typename Functor, typename... Args> requires (
      cobb::all_same<Args...>
   && std::is_same_v<
         bool,
         std::invoke_result_t<
            Functor,
            size_t,
            std::tuple_element_t<0, std::tuple<Args...>>
         >
      >
   )
   void each_nttp_of_type_matches(Functor&& f, Args... args) {
      size_t which = 0;
      bool   match = true;
      ((match ? match &= f(which++, args) : false), ...);
      return match;
   }
}