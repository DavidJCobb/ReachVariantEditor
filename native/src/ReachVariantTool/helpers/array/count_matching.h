#pragma once
#include <array>
#include <type_traits>
#include "helpers/polyfills/msvc/const_reference_nttp_decltype.h"

namespace cobb::array {
   #pragma region run-time
      template<typename Functor, typename ValueType, size_t Size> requires (
         std::is_invocable_r<bool, Functor, std::decay_t<ValueType>>::value
      )
      constexpr size_t count_matching(const std::array<ValueType, Size>& src, Functor&& predicate) {
         size_t res = 0;
         for (const auto& item : src)
            if (predicate(item))
               ++res;
         return res;
      }

      // Variant with index argument
      template<typename Functor, typename ValueType, size_t Size> requires (
         std::is_invocable_r<bool, Functor, std::decay_t<ValueType>, size_t>::value
      )
      constexpr size_t count_matching(const std::array<ValueType, Size>& src, Functor&& predicate) {
         size_t res = 0;
         for (size_t n = 0; n < Size; ++n) {
            const auto& item = src[n];
            if (predicate(item, n))
               ++res;
         }
         return res;
      }
   #pragma endregion

   #pragma region compile-time
      template<const auto& Array, auto Functor>
      constexpr const auto count_of_matching = []() consteval {
         using value_type = typename std::decay_t<cobb::polyfills::msvc::nttp_decltype<Array>>::value_type;

         size_t count = 0;
         if constexpr (std::is_invocable_r<bool, cobb::polyfills::msvc::nttp_decltype<Functor>, value_type>::value) {
            for (const auto& item : Array)
               if (Functor(item))
                  ++count;
         } else if constexpr (std::is_invocable_r<bool, cobb::polyfills::msvc::nttp_decltype<Functor>, value_type, size_t>::value) {
            for (size_t n = 0; n < Array.size(); ++n) {
               const auto& item = Array[n];
               if (Functor(item, n))
                  ++count;
            }
         }
         return count;
      }();
   #pragma endregion
}