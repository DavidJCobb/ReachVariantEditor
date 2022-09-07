#pragma once
#include <tuple>
#include <type_traits>
#include <utility>
#include "./filter_types.h"
#include "./indices_of_matching_types.h"

namespace cobb {
   template<auto Predicate, typename Tuple>
   constexpr tuple_filter_types<std::decay_t<Tuple>, Predicate> tuple_filter_values_by_type(const Tuple& tuple) {
      using result_t = tuple_filter_types<std::decay_t<Tuple>, Predicate>;

      constexpr auto index_map = tuple_indices_of_matching_types<std::decay_t<Tuple>, Predicate>;
      result_t out = {};
      [&out]<std::size_t... I>(const Tuple& tuple, std::index_sequence<I...>) {
         (
            (std::get<I>(out) = std::get<index_map[I]>(tuple)),
            ...
         );
      }(
         tuple,
         std::make_index_sequence<std::tuple_size_v<std::decay_t<decltype(index_map)>>>{}
      );
      return out;
   }
}