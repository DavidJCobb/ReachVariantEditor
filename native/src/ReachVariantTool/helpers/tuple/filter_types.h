#pragma once
#include <tuple>
#include <type_traits>
#include "../tuple_prepend.h"

namespace cobb {
   namespace impl::tuple_filter_types {
      template<auto Predicate, typename...> struct variadic_filter;
      template<auto Predicate, typename... Types> using variadic_filter_t = typename variadic_filter<Predicate, Types...>::type;
      
      template<auto Predicate> struct variadic_filter<Predicate> {
         using type = std::tuple<>;
      };
      
      template<auto Predicate, typename First, typename... Next>
      struct variadic_filter<Predicate, First, Next...> {
         using type = std::conditional_t<
            (Predicate.template operator()<First>()),
            cobb::tuple_prepend<First, variadic_filter_t<Predicate, Next...>>,
            variadic_filter_t<Predicate, Next...>
         >;
      };

      // ---

      template<auto Predicate, typename> struct result;
      template<auto Predicate, typename... Types> struct result<Predicate, std::tuple<Types...>> {
         using type = variadic_filter_t<Predicate, Types...>;
      };
   }

   template<typename Tuple, auto Predicate> using tuple_filter_types = typename impl::tuple_filter_types::result<Predicate, Tuple>::type;
}