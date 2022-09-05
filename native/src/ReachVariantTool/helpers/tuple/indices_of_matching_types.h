#pragma once
#include <array>
#include <tuple>
#include <type_traits>
#include "./filter_types.h"

namespace cobb {
   namespace impl::tuple_indices_of_matching_types {
      template<typename Tuple, auto Predicate>
      static constexpr size_t matching_count = std::tuple_size_v<cobb::tuple_filter_types<Tuple, Predicate>>;

      template<typename Tuple, auto Predicate>
      using match_list_t = std::array<size_t, matching_count<Tuple, Predicate>>;

      template<typename Tuple, auto Predicate>
      struct result;

      template<auto Predicate, typename... Types>
      struct result<std::tuple<Types...>, Predicate> {
         static constexpr auto value = [](){
            match_list_t<std::tuple<Types...>, Predicate> out = {};
            size_t i = 0;
            size_t o = 0;
            (
               (
                  Predicate.template operator()<Types>() ?
                     (out[o++] = i++)
                  :
                     i++
               ),
               ...
            );
            return out;
         }();
      };

      template<auto Predicate>
      struct result<std::tuple<>, Predicate> {
         static constexpr match_list_t<std::tuple<>, Predicate> value = {};
      };
   }

   template<typename Tuple, auto Predicate>
   constexpr impl::tuple_indices_of_matching_types::match_list_t<Tuple, Predicate> tuple_indices_of_matching_types
      = impl::tuple_indices_of_matching_types::result<Tuple, Predicate>::value;
}