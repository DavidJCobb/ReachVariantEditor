#pragma once
#include <limits>
#include <tuple>
#include <type_traits>

namespace cobb {
   namespace impl {
      template<typename Needle, typename Haystack> struct tuple_index_of;
      //
      template<typename Needle, typename... Types> struct tuple_index_of<Needle, std::tuple<Types...>> {
         static constexpr size_t value = ([]() {
            size_t index = 0;
            (
               (std::is_same_v<Needle, Types> ?
                  false             // if match: set result to current index; use false to short-circuit the "and" operator and stop iteration
               :
                  ((++index), true) // no match: increment current index; use true to avoid short-circuiting, and continue iteration
               )
               && ...
            );
            return index < sizeof...(Types) ? index : std::numeric_limits<size_t>::max();
         })();
      };

      namespace tuple_index_of_matching {
         template<typename Haystack, auto Predicate> struct exec_tp;
         template<auto Predicate, typename... Types> struct exec_tp<std::tuple<Types...>, Predicate> {
            static constexpr size_t value = ([](){
               size_t index = 0;
               (
                  (Predicate.template operator()<Types>() ?
                     false             // if match: set result to current index; use false to short-circuit the "and" operator and stop iteration
                  :
                     ((++index), true) // no match: increment current index; use true to avoid short-circuiting, and continue iteration
                  )
                  && ...
               );
               return index < sizeof...(Types) ? index : std::numeric_limits<size_t>::max();
            })();
         };

         template<typename Haystack, typename Functor> struct exec_arg;
         //
         template<typename Functor, typename... Types> struct exec_arg<Functor, std::tuple<Types...>> {
            static constexpr size_t execute(Functor&& f) {
               size_t index = 0;
               (
                  (f.template operator()<Types>() ?
                     false             // if match: set result to current index; use false to short-circuit the "and" operator and stop iteration
                  :
                     ((++index), true) // no match: increment current index; use true to avoid short-circuiting, and continue iteration
                  )
                  && ...
               );
               return index < sizeof...(Types) ? index : std::numeric_limits<size_t>::max();
            }
         };
      }
   }

   template<typename Needle, typename Haystack>
   constexpr size_t tuple_index_of = impl::tuple_index_of<Needle, Haystack>::value;

   template<typename Tuple, auto Predicate> constexpr size_t tuple_index_of_matching() {
      return impl::tuple_index_of_matching::exec_tp<Tuple, Predicate>::value;
   }

   // TIP: Invoking a constexpr lambda within the definition of a templated class may fail using this overload. 
   //      If the lambda is non-capturing, try passing it as a template parameter instead!
   template<typename Tuple, typename Functor> constexpr size_t tuple_index_of_matching(Functor&& f) {
      return impl::tuple_index_of_matching::exec_arg<Tuple, Functor>::execute(f);
   }
}
