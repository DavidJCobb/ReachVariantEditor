#pragma once
#include "../cs.h"

namespace cobb::cs_utils {
   template<size_t A, size_t B> constexpr size_t count_char_in(const cobb::cs<A>& a, const cobb::cs<B>& b) {
      constexpr size_t Min = (A < B) ? A : B;

      size_t result = 0;
      for (size_t i = 0; i < Min; ++i) {
         if (a[i] == b[i])
            ++result;
         else
            break;
      }
      return result;
   }
}