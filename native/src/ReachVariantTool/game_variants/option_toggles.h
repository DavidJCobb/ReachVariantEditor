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
         int wordcount = (count + 15) / 16;
         for (int i = 0; i < wordcount; i++) {
            uint16_t word;
            stream.read(word);
            //
            // for big-endian words (i.e. KSoft.Tool in-memory):
            //    words[bit / 16] & ((1 << 15) >> (bit % 16))
            //
            // for little-endian words (i.e. file format):
            //    words[bit / 16] & (1 << (bit % 16))
            //
            for (int j = 0; j < 16; j++) {
               int index = (i * 16) + j;
               if (index >= count)
                  break;
               bool bit = (word & (i << j)) != 0;
               if (bit)
                  this->options.set(index);
            }
         }
      }
};
using ReachGameVariantEngineOptionToggles = ReachGameVariantOptionToggles<1272>;
using ReachGameVariantMegaloOptionToggles = ReachGameVariantOptionToggles<16>;