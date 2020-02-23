#pragma once
#include <cstdint>
#include "bitwise.h"
#include "int128.h"

namespace cobb {

   //
   // TODO:
   //
   //  - Run unit tests to ensure that this works.
   //

   class bitarray128 {
      public:
         using storage_type = uint128_t;
         //static constexpr int storage_bits = bits_in<storage_type>; // doesn't work if using cobb::uint128_t which is a struct
         static constexpr int storage_bits = 128;
         //
         storage_type bits = 0;
         uint8_t      size = 0; // number of bits we're storing; bits are aligned to the righthand side
         //
         void clear() noexcept {
            this->bits = 0;
            this->size = 0;
         }
         void push_back(storage_type bits, uint8_t count) noexcept {
            //
            // Copy bits from the righthand side of the input value.
            //
            this->bits = (this->bits << count) | (bits & cobb::bitmax(count));
            this->size += count;
         }
         storage_type consume(storage_type& bits, uint8_t count) noexcept {
            //
            // Read bits off of the lefthand side of the input value. We modify the input value -- we're "taking" 
            // the bits, not simply copying them.
            //
            storage_type result = bits >> (128 - count);
            this->bits = (this->bits << count) | result;
            this->size += count;
            bits >>= count;
            return result;
         }
         uint64_t excerpt(uint8_t offset, uint8_t count) const noexcept {
            //
            // Read bits from our storage starting at (offset) bits from the lefthand side of our stored bits. So, 
            // if we've only stored 16 bits, then the offset would be measured from the lefthand side of the lowest 
            // word/short of our storage.
            //
            uint64_t result = (uint64_t) ((this->bits >> (this->size - offset - count)) & cobb::bitmax(count));
            return result;
         }
         void overwrite(uint8_t offset, uint8_t count, storage_type bits) {
            int max_bits = cobb::bitmax(count);
            int shift_by = this->size - offset - count - 1;
            storage_type mask = storage_type(max_bits) << shift_by;
            mask = ~mask;
            this->bits &= mask;
            this->bits |= (bits & max_bits) << shift_by;
         }
   };
}