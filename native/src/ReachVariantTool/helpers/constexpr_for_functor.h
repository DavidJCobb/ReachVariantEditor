#pragma once
#include <type_traits>

namespace cobb {
   template<auto Start, auto End, auto Inc, typename Functor> requires requires(Functor&& f) {
      requires std::is_convertible_v<std::decay_t<decltype(Start)>, std::decay_t<decltype(End)>>;
      requires std::is_convertible_v<std::decay_t<decltype(Start)>, std::decay_t<decltype(Inc)>>;
      { f.template operator()<Start>() };

      // We can't simply wrap the entire function body in an if-constexpr for this, 
      // because the templated operator() would still be instantiated -- which will 
      // lead to failures if the lambda body relies on the index being in range.
      requires Start < End;
   }
   constexpr void constexpr_for(Functor&& f) {
      f.template operator()<Start>();
      if constexpr (Start + Inc < End) {
         constexpr_for<Start + Inc, End, Inc>(f);
      }
   }
}