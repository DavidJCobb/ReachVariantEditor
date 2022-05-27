#pragma once
#include <tuple>
#include <type_traits>

namespace cobb {
   namespace impl::tuple_filter {
      template<typename, typename> struct prepend;
      template<typename Prepend, typename... Prior> struct prepend<Prepend, std::tuple<Prior...>> {
         using type = std::tuple<Prepend, Prior...>;
      };

      template<typename A, typename B> using prepend_t = typename prepend<A, B>::type;

      template<auto Predicate, typename...> struct variadic_filter;
      template<auto Predicate, typename... Types> using variadic_filter_t = typename variadic_filter<Predicate, Types...>::type;
      //
      template<auto Predicate> struct variadic_filter<Predicate> {
         using type = std::tuple<>;
      };
      //
      template<auto Predicate, typename First, typename... Next>
      struct variadic_filter<Predicate, First, Next...> {
         using type = std::conditional_t<
            (Predicate.template operator()<First>()),
            prepend_t<First, variadic_filter_t<Predicate, Next...>>,
            variadic_filter_t<Predicate, Next...>
         >;
      };
   }

   template<auto Predicate, typename> struct tuple_filter;
   template<auto Predicate, typename T> using tuple_filter_t = typename tuple_filter<Predicate, T>::type;

   template<auto Predicate, typename... Types> struct tuple_filter<Predicate, std::tuple<Types...>> {
      using type = impl::tuple_filter::variadic_filter_t<Predicate, Types...>;
   };
}