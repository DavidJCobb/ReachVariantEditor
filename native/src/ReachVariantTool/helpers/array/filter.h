#pragma once
#include <array>
#include <type_traits>
#include "helpers/array/count_matching.h"
#include "helpers/polyfills/msvc/const_reference_nttp_decltype.h"
#include "helpers/type_traits/is_std_array.h"

namespace cobb::array {
   template<const auto& Src, auto Functor>
   constexpr const auto filter = []() consteval {
      using value_type = typename std::decay_t<cobb::polyfills::msvc::nttp_decltype<Src>>::value_type;

      std::array<value_type, count_of_matching<Src, Functor>> dst = {};
      size_t i = 0;
      if constexpr (std::is_invocable_r<bool, cobb::polyfills::msvc::nttp_decltype<Functor>, value_type>::value) {
         for (const auto& item : Src)
            if (Functor(item))
               dst[i++] = item;
      } else if constexpr (std::is_invocable_r<bool, cobb::polyfills::msvc::nttp_decltype<Functor>, value_type, size_t>::value) {
         for (size_t n = 0; n < Src.size(); ++n) {
            const auto& item = Src[n];
            if (Functor(item, n))
               dst[i++] = item;
         }
      }
      return dst;
   }();
}