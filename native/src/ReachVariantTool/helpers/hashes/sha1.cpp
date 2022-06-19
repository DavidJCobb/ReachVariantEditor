#include "sha1.h"
#include <string>

namespace cobb::hashes::tests::sha1 {
   constexpr std::array<uint8_t, 43> str = []() {
      auto* src = "The quick brown fox jumps over the lazy dog";
      std::array<uint8_t, 43> out = {};
      for (size_t i = 0; i < out.size(); ++i)
         out[i] = src[i];
      return out;
   }();

   constexpr auto test = sha1_hex(str.data(), str.size());
   constexpr auto test1 = cobb::hashes::sha1(str.data(), str.size());
   constexpr std::string_view testS = test.data();
}