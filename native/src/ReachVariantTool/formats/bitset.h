#pragma once
#include <cstdint>
#include <vector>
#include "../helpers/bitset.h"
#include "../helpers/stream.h"
#include "../helpers/bitwriter.h"

#include "../game_variants/components/megalo/limits.h"

#if _DEBUG
   struct ReachDwordBasedBitsetDebugInfo {
      std::vector<uint32_t> setBitIndices;
      //
      void registerBit(uint32_t index) noexcept {
         this->setBitIndices.push_back(index);
      }
   };
#endif

template<int count> class ReachDwordBasedBitset {
   public:
      static constexpr int flag_count  = count;
      static constexpr int dword_count = (count + 31) / 32;
      //
      cobb::bitset<count> bits;
      //
      void read(cobb::ibitreader& stream) noexcept {
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
                  this->bits.set(index);
            }
         }
         #if _DEBUG
            this->build_debug_info();
         #endif
      }
      void write(cobb::bitwriter& stream) const noexcept {
         auto c = this->bits.dword_count();
         for (uint8_t i = 0; i < c; i++)
            stream.write(this->bits.dword(i));
      }
      //
      #if _DEBUG
         mutable ReachDwordBasedBitsetDebugInfo debugInfo;
         void build_debug_info() const noexcept {
            for (uint32_t i = 0; i < count; i++) {
               bool flag = this->bits.test(i);
               if (flag)
                  this->debugInfo.registerBit(i);
            }
         };
      #endif
};
using ReachGameVariantEngineOptionToggles = ReachDwordBasedBitset<1273>; // 1280 bits (160 bytes) in the file
using ReachGameVariantMegaloOptionToggles = ReachDwordBasedBitset<16>;

using ReachGameVariantUsedMPObjectTypeList = ReachDwordBasedBitset<Megalo::Limits::max_object_types>;