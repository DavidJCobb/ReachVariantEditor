#pragma once
#include <cstdint>
#include "../helpers/bitset.h"
#include "../helpers/bitstream.h"
#include "../helpers/bitnumber.h"

#if _DEBUG
   struct ReachGameVariantOptionToggleDebugInfo {
      std::vector<uint32_t> setBitIndices;
      //
      void registerBit(uint32_t index) noexcept {
         this->setBitIndices.push_back(index);
      }
   };
#endif

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
         #if _DEBUG
            this->build_debug_info();
         #endif
      }
      //
      #if _DEBUG
         mutable ReachGameVariantOptionToggleDebugInfo debugInfo;
         void build_debug_info() const noexcept {
            for (uint32_t i = 0; i < count; i++) {
               bool flag = this->options.test(i);
               if (flag)
                  this->debugInfo.registerBit(i);
            }
         };
      #endif
};
using ReachGameVariantEngineOptionToggles = ReachGameVariantOptionToggles<1272>;
using ReachGameVariantMegaloOptionToggles = ReachGameVariantOptionToggles<16>;