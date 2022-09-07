#pragma once
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

namespace cobb {
   template<typename Tuple, typename Functor>
   extern constexpr size_t tuple_index_of_matching_value(Tuple&& tuple, Functor&& functor) {
      constexpr auto size = std::tuple_size_v<std::decay_t<Tuple>>;

      size_t index = 0;

      // IIFE to avoid the need for a helper function:
      [&index]<std::size_t... I>(Tuple&& tuple, Functor&& functor, std::index_sequence<I...>) {
         (
            (functor(std::get<I>(tuple)) ?
               false             // if match: set result to current index; use false to short-circuit the "and" operator and stop iteration
            :
               ((++index), true) // no match: increment current index; use true to avoid short-circuiting, and continue iteration
            )
            && ...
         );
      }(
         std::forward<Tuple>(tuple),
         std::forward<Functor>(functor),
         std::make_index_sequence<size>{}
      );

      return index < size ? index : std::numeric_limits<size_t>::max();
   }
}