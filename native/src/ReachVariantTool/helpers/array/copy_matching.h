#pragma once
#include <array>
#include <type_traits>

namespace cobb::array {
   template<typename Functor, typename ValueType, size_t SizeSrc, size_t SizeDst> requires (
      std::is_invocable_r<bool, Functor, std::decay_t<ValueType>>::value
   )
   consteval auto& copy_matching(const std::array<ValueType, SizeSrc>& src, std::array<ValueType, SizeDst>& dst, Functor&& predicate) {
      size_t i = 0;
      for (const auto& item : src)
         if (predicate(item))
            dst[i++] = item;
      return dst;
   }

   // Variant with index argument
   template<typename Functor, typename ValueType, size_t SizeSrc, size_t SizeDst> requires (
      std::is_invocable_r<bool, Functor, std::decay_t<ValueType>, size_t>::value
   )
   consteval auto& copy_matching(const std::array<ValueType, SizeSrc>& src, std::array<ValueType, SizeDst>& dst, Functor&& predicate) {
      size_t i = 0;
      for (size_t n = 0; n < SizeSrc; ++n) {
         const auto& item = src[n];
         if (predicate(item, n))
            dst[i++] = item;
      }
      return dst;
   }
}