#pragma once
#include "../cs.h"

namespace cobb::cs_utils {
   template<size_t Size> constexpr size_t count_char_in(char c, const cobb::cs<Size>& str) {
      size_t result = 0;
      for (size_t i = 0; i < str.size(); ++i)
         if (str[i] == c)
            ++result;
      return result;
   }
}