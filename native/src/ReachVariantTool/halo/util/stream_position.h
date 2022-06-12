#pragma once
#include <cstdint>

namespace halo::util {
   struct stream_position {
      size_t  bytes = 0;
      uint8_t bits  = 0;

      size_t bytespan() const { return this->bytes + (this->bits ? 1 : 0); }
      size_t in_bits() const { return (this->bytes * 8) + this->bits; }

      stream_position& advance_by_bits(size_t bits);
      stream_position& advance_by_bytes(size_t bytes); // also forces bit-offset to zero
      stream_position& advance_to_next_byte();

      stream_position advanced_by_bits(size_t bits) const { return stream_position(*this).advance_by_bits(bits); }
      stream_position advanced_by_bytes(size_t bytes) const { return stream_position(*this).advance_by_bytes(bytes); }
      stream_position advanced_to_next_byte() const { return stream_position(*this).advance_to_next_byte(); }

      stream_position& rewind_by_bits(size_t bits);
      stream_position& rewind_by_bytes(size_t bytes);
      stream_position rewound_by_bits(size_t bits) const { return stream_position(*this).rewind_by_bits(bits); }
      stream_position rewound_by_bytes(size_t bits) const { return stream_position(*this).rewind_by_bytes(bytes); }

      void set_in_bits(size_t bits); // bytes = bits % 8; bits = bits / 8; overshoot = {};
      void set_in_bytes(size_t bytes); // bytes = bytes; bits = 0; overshoot = {};
   };

   struct full_stream_position : public stream_position {
      stream_position overshoot;

      size_t overshoot_in_bits()  const noexcept { return overshoot.in_bits(); }
      size_t overshoot_in_bytes() const noexcept { return overshoot.bytes; }

      void clamp_to_size(size_t size); // adjusts for overshoot

      size_t in_bits() const;
      void set_in_bits(size_t bits); // bytes = bits % 8; bits = bits / 8; overshoot = {};
      void set_in_bytes(size_t bytes); // bytes = bytes; bits = 0; overshoot = {};

      void add_overshoot_bits(size_t bits);
      void set_overshoot_bits(size_t bits);
      void set_overshoot_bytes(size_t bytes);
   };
}
