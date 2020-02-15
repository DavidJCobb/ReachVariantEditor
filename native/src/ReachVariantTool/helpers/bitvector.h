#pragma once
#include <cstdint>
#include "bitwise.h"

namespace cobb {
   class bitvector64 {
      public:
         using storage_type = uint64_t;
         static constexpr int storage_bits = bits_in<storage_type>;
         //
         storage_type bits = 0;
         uint8_t      size = 0;
         //
         void clear() noexcept {
            this->bits = 0;
            this->size = 0;
         }
         //
         // TODO: (push_back) and (consume) read bits from the righthand side of the input, and (consume) shears 
         // the read bits off of the righthand side. However, I intend to pass 64 bits' worth of data that we've 
         // peeked from a bitstream, so we should be reading from the lefthand side and shearing off the left-
         // hand side. In fact, everything should be left-side-aligned.
         //
         void push_back(storage_type bits, uint8_t count) noexcept {
            this->bits = (this->bits << count) | (bits & cobb::bitmax(count));
            this->size += count;
         }
         storage_type consume(storage_type& bits, uint8_t count) noexcept {
            storage_type result = bits & cobb::bitmax(count);
            this->bits = (this->bits << count) | result;
            this->size += count;
            bits >>= count;
            return result;
         }
         uint64_t excerpt(uint8_t offset, uint8_t count) const noexcept {
            return (this->bits >> (this->size - offset - count)) & cobb::bitmax(count);
         }


         // TODO: operator<<
   };
}