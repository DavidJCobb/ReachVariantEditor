#pragma once
#include <tuple>
#include <type_traits>

namespace cobb::template_parameters {
   namespace impl::tuple {
      struct dummy {};

      template<typename T> struct item {
         T value = {};

         constexpr item() {}
         constexpr item(T v) : value(v) {}
      };

      template<typename...> struct link;
      template<typename First, typename... Next> struct link<First, Next...> {
         using next_link = std::conditional_t<
            (sizeof...(Next) > 0),
            link<Next...>,
            dummy
         >;

         constexpr link() {}
         constexpr link(First f, Next... n) : member(f), next(n...) {}

         item<First> member;
         next_link   next;
      };
   }

   template<typename... Types> struct tuple {
      using as_std = std::tuple<Types...>;
      static constexpr size_t size = sizeof...(Types);
      //
      template<size_t Index> using nth_type = std::tuple_element_t<Index, std::tuple<Types...>>;

      impl::tuple::link<Types...> data;

      constexpr tuple() {}
      constexpr tuple(Types&&... v) : data(v...) {}

      template<size_t Index> constexpr auto& _nth_link() {
         if constexpr (Index == 0) {
            return this->data;
         } else {
            return _nth_link<Index - 1>().next;
         }
      }
      template<size_t Index> constexpr const auto& _nth_link() const {
         if constexpr (Index == 0) {
            return this->data;
         } else {
            return _nth_link<Index - 1>().next;
         }
      }

      template<size_t Index> constexpr nth_type<Index> item() {
         return _nth_link<Index>().member.value;
      }
      template<size_t Index> constexpr const nth_type<Index>& item() const {
         return _nth_link<Index>().member.value;
      }
   };

   template<typename... Types> constexpr tuple<Types...> make_tuple(Types&&... args) {
      return tuple<Types...>(std::forward<Types&&>(args)...);
   }
}