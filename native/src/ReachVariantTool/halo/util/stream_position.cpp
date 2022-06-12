#include "stream_position.h"

namespace halo::util {
   stream_position& stream_position::advance_by_bits(size_t bits) {
      if (this->bits) {
         size_t to_consume = 8 - this->bits;
         if (bits < to_consume) {
            this->bits += bits;
            return *this;
         }
         this->bits   = 0;
         this->bytes += 1;
         bits -= to_consume;
      }
      if (bits) {
         this->bytes += bits / 8;
         this->bits   = bits % 8;
      }
      // for a full_stream_position, caller will likely want to clamp_to_size at this point, too
      return *this;
   }
   stream_position& stream_position::advance_by_bytes(size_t bytes) {
      this->bits   = 0;
      this->bytes += bytes;
      return *this;
   }
   stream_position& stream_position::advance_to_next_byte() {
      this->bits = 0;
      ++this->bytes;
      // for a full_stream_position, caller will likely want to clamp_to_size at this point, too
      return *this;
   }

   stream_position& stream_position::rewind_by_bits(size_t bits) {
      if (this->bits > 0) {
         if (this->bits > bits) {
            this->bits -= bits;
            return *this;
         }
         bits -= this->bits;
         this->bits = 0;
      }
      this->bytes -= (bits / 8);
      bits = bits % 8;
      if (bits) {
         --this->bytes;
         this->bits = 8 - bits;
      }
      return *this;
   }
   stream_position& stream_position::rewind_by_bytes(size_t bytes) {
      this->bytes -= bytes;
      return *this;
   }

   void stream_position::set_in_bits(size_t bits) {
      this->bits  = bits % 8;
      this->bytes = bits / 8;
   }
   void stream_position::set_in_bytes(size_t bytes) {
      this->bits  = 0;
      this->bytes = bytes;
   }

   #pragma region full_stream_position
   //
   #pragma region overrides
   size_t full_stream_position::in_bits() const {
      return stream_position::in_bits() + overshoot.in_bits();
   }

   void full_stream_position::set_in_bits(size_t bits) {
      stream_position::set_in_bits(bits);
      this->overshoot = {};
   }
   void full_stream_position::set_in_bytes(size_t bytes) {
      stream_position::set_in_bytes(bytes);
      this->overshoot = {};
   }
   #pragma endregion

   void full_stream_position::clamp_to_size(size_t size) {
      if (this->bytes <= size)
         return;
      this->overshoot = {
         .bytes = (this->bytes - size),
         .bits = this->bits,
      };
      this->bytes = size;
      this->bits = 0;
   }

   void full_stream_position::add_overshoot_bits(size_t bits) {
      if (this->overshoot.bits > 0) {
         bits -= this->overshoot.bits;
         this->overshoot.bits = 0;
         ++this->overshoot.bytes;
      }
      this->overshoot.bytes += bits / 8;
      this->overshoot.bits += bits % 8;
   }
   void full_stream_position::set_overshoot_bits(size_t bits) {
      this->overshoot.bytes += bits / 8;
      this->overshoot.bits += bits % 8;
   }
   void full_stream_position::set_overshoot_bytes(size_t bytes) {
      this->overshoot.bytes = bytes;
      this->overshoot.bits = 0;
   }
   #pragma endregion
}