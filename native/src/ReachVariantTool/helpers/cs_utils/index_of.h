#pragma once
#include "../cs.h"

namespace cobb::cs_utils {
   template<size_t Size> constexpr size_t index_of(const cobb::cs<Size>& str, char c, size_t from = 0) {
      for (size_t i = from; i < str.size(); ++i)
         if (str[i] == c)
            return i;
      return size_t{ -1 };
   }

   template<size_t Size> constexpr size_t last_index_of(const cobb::cs<Size>& str, char c, size_t from = Size - 1) {
      size_t i = from;
      do {
         if (str[i] == c)
            return i;
      } while (i--);
      return size_t{ -1 };
   }
}
