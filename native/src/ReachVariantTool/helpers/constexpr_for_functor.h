#pragma once
#include <type_traits>

namespace cobb {
   template<auto Start, auto End, auto Inc, typename Functor> requires requires(Functor&& f) {
      requires std::is_same_v<decltype(Start), decltype(End)>;
      requires std::is_same_v<decltype(Start), decltype(Inc)>;
      { f.template operator()<Start>() };
   }
   void constexpr_for(Functor&& f) {
      if constexpr (Start < End) {
         f.template operator()<Start>();
         constexpr_for<Start + Inc, End, Inc>(f);
      }
   }
}