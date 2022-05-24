#pragma once
#include "helpers/bitset.h"
#include "halo/bitreader.h"

namespace halo::util {
   template<size_t Count>
   class dword_bitset {
      public:
         static constexpr size_t flag_count  = Count;
         static constexpr size_t dword_count = (Count + 31) / 32;
         
         cobb::bitset<flag_count> bits;

         template<bitreader_subclass Reader> void read(Reader& stream) {
            uint32_t dword;
            for (size_t i = 0; i < dword_count; ++i) {
               stream.read(dword);
               //
               size_t index = i * 32;
               for (uint8_t j = 0; j < 32; ++j) {
                  if (index + j >= flag_count)
                     break;
                  bool bit = (dword & (1 << j)) != 0;
                  if (bit)
                     this->bits.set(index + j);
               }
            }
         }
   };
}