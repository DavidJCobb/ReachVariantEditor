#pragma once
#include <tuple>
#include <type_traits>
#include "./filter_types.h"

namespace cobb::tuple {
   namespace impl {
      template<typename Tuple, typename Desired>
      struct _contains_type;

      template<typename Desired, typename... Types>
      struct _contains_type<std::tuple<Types...>, Desired> {
         static constexpr bool value = [](){
            bool found = false;
            (
               (
                  std::is_same_v<Types, Desired> ?
                     (found = true)
                  :
                     false
               ) ||
               ...
            );
            return found;
         }();
      };
   }

   template<typename Tuple, typename T>
   constexpr bool contains_type = impl::_contains_type<std::decay_t<Tuple>, T>::value;
}