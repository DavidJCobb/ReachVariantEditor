#pragma once
#include "../cs.h"
#include "./count_char_in.h"

namespace cobb::cs_utils {
   template<char C, const cobb::cs Str> constexpr std::array<size_t, count_char_in(C, Str)> all_indices_of_char() {
      std::array<size_t, count_char_in(C, Str)> out = {};
      size_t i    = 0;
      size_t last = 0;
      for (; i < str.size(); ++i) {
         if (str[i] == c) {
            out[item++] = last;
            last = i;
         }
      }
      return out;
   }
}
