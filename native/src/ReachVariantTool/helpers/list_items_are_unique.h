#pragma once
#include <concepts>

namespace cobb {
   namespace impl::list_items_are_unique {
      template<typename T> concept is_indexable_list = requires (const T& x, size_t i) {
         { x.size() } -> std::same_as<size_t>;
         { x[i] };
      };

      template<typename T> concept has_range_loop_members = requires(const T& x) {
         { x.begin() };
         { x.end() };
         { x.begin() != x.end() }; // can check range
         { ++x.begin() }; // can advance iterator
         { *x.begin() }; // can access value
      };

      template<typename T> concept has_range_loop_non_members = requires(const T & x) {
         { begin(x) };
         { end(x) };
         { begin(x) != end(x) }; // can check range
         { ++begin(x) }; // can advance iterator
         { *begin(x) }; // can access value
      };

      template<typename T> concept is_compatible_list = is_indexable_list<T> || (has_range_loop_members<T> || has_range_loop_non_members<T>);

      template<typename List, typename Functor> concept is_usable_functor = (
         (is_indexable_list<List> && requires(const List &x, Functor f) {
            { f(x[0], x[1]) } -> std::same_as<bool>;
         }) || 
         (has_range_loop_members<List> && requires(const List & x, Functor f) {
            { f(*x.begin(), *x.begin()) } -> std::same_as<bool>;
         }) ||
         (has_range_loop_non_members<List> && requires(const List & x, Functor f) {
            { f(*begin(x), *begin(x)) } -> std::same_as<bool>;
         })
      );
   }

   // Test whether two list items are unique. The functor should take as arguments 
   // two list items (by value or by reference), and return true if they are unique.
   template<typename List, typename Functor> requires impl::list_items_are_unique::is_compatible_list<List> && impl::list_items_are_unique::is_usable_functor<List, Functor>
   constexpr bool list_items_are_unique(const List& list, Functor uniqueness_comparator) {
      using namespace impl::list_items_are_unique;

      if constexpr (is_indexable_list<List>) {
         const size_t size = list.size();
         for (size_t i = 0; i + 1 < size; ++i) {
            for (size_t j = i + 1; j < size; ++j) {
               if (!uniqueness_comparator(list[i], list[j]))
                  return false;
            }
         }
         return true;
      } else {
         //
         // The list doesn't support access by index. The naive approach would be 
         // to use a range-based for-loop, but those are just syntactic sugar for 
         // calls to begin/end functions. If we call those functions directly, we 
         // may be able to skip some steps.
         //
         if constexpr (has_range_loop_members<List>) {
            const auto end = list.end();
            for (auto it = list.begin(); it != end; ++it) {
               for (auto jt = it, ++jt; jt != end; ++jt) {
                  if (!uniqueness_comparator(*it, *jt))
                     return false;
               }
            }
         } else if constexpr (has_range_loop_non_members<List>) {
            const auto end = end(list);
            for (auto it = begin(list); it != end; ++it) {
               for (auto jt = it, ++jt; jt != end; ++jt) {
                  if (!uniqueness_comparator(*it, *jt))
                     return false;
               }
            }
         }
      }
      return true;
   }
}
