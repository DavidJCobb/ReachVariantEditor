#pragma once
#include <tuple>

namespace cobb {
   namespace impl::tuple_foreach {
      template<typename, auto Predicate> struct exec_tp;
      template<auto Predicate, typename... Types> struct exec_tp<std::tuple<Types...>, Predicate> {
         static constexpr void execute() {
            (Predicate.template operator()<Types>(), ...);
         }
      };
      template<auto Predicate> struct exec_tp<std::tuple<>, Predicate> {
         static constexpr void execute() {}
      };

      template<typename Tuple, typename Functor> struct exec_arg;
      template<typename Functor, typename... Types> struct exec_arg<std::tuple<Types...>, Functor> {
         static constexpr void execute(Functor&& f) {
            (f.template operator()<Types>(), ...);
         }
      };
      template<typename Functor> struct exec_arg<std::tuple<>, Functor> {
         static constexpr void execute(Functor&& f) {}
      };
   }

   template<typename Tuple, auto Predicate> extern constexpr void tuple_foreach() {
      impl::tuple_foreach::exec_tp<Tuple, Predicate>::execute();
   }

   template<typename Tuple, typename Functor> extern constexpr void tuple_foreach(Functor&& f) {
      impl::tuple_foreach::exec_arg<Tuple, Functor>::execute(std::forward<Functor&&>(f));
   }
}
