#pragma once
#include <array>

namespace cobb::array {
   template<typename ValueType, size_t SizeA, size_t SizeB>
   consteval std::array<ValueType, SizeA + SizeB> concat(const std::array<ValueType, SizeA>& a, const std::array<ValueType, SizeB>& b) {
      std::array<ValueType, SizeA + SizeB> out = {};
      for (size_t i = 0; i < SizeA; ++i)
         out[i] = a[i];
      for (size_t i = 0; i < SizeB; ++i)
         out[i + SizeA] = b[i];
      return out;
   }
}