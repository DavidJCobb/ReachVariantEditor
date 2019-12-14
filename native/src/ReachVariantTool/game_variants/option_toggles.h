#pragma once
#include <cstdint>
#include "../helpers/bitset.h"
#include "../helpers/bitstream.h"
#include "../helpers/bitnumber.h"

template<int count> class ReachGameVariantOptionToggles {
   public:
      cobb::bitset<count> options;
      //
      void read(cobb::bitstream& stream) noexcept {
         constexpr int dwordcount = (count + 31) / 32;
         for (int i = 0; i < dwordcount; i++) {
            uint32_t dword;
            stream.read(dword);
            //
            // for big-endian dwords (i.e. KSoft.Tool in-memory):
            //    dwords[bit / 32] & ((1 << 31) >> (bit % 32))
            //
            // for little-endian dwords (i.e. file format):
            //    dwords[bit / 32] & (1 << (bit % 32))
            //
            for (int j = 0; j < 32; j++) {
               int index = (i * 32) + j;
               if (index >= count)
                  break;
               bool bit = (dword & (1 << j)) != 0;
               if (bit)
                  this->options.set(index);
            }
         }
      }
};
using ReachGameVariantEngineOptionToggles = ReachGameVariantOptionToggles<1272>;
using ReachGameVariantMegaloOptionToggles = ReachGameVariantOptionToggles<16>;