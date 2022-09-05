#pragma once
#include <tuple>
#include <type_traits>

namespace cobb {
   template<typename Tuple, typename Functor>
   extern constexpr void tuple_for_each_value(Tuple&& tuple, Functor&& functor) {
      // IIFE to avoid the need for a helper function:
      []<std::size_t... I>(Tuple&& tuple, Functor&& functor, std::index_sequence<I...>) {
         (functor(std::get<I>(tuple)), ...);
      }(
         std::forward<Tuple>(tuple),
         std::forward<Functor>(functor),
         std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>{}
      );
   }
}