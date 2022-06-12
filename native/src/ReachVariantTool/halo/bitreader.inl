#pragma once
#include "bitreader.h"
#include "util/compressed_float.h"

#define CLASS_TEMPLATE_PARAMS template<typename Subclass, typename LoadProcess>
#define CLASS_NAME bitreader<Subclass, LoadProcess>
//
namespace halo {
   CLASS_TEMPLATE_PARAMS void CLASS_NAME::_advance_offset_by_bits(size_t bits) {
      this->_position.advance_by_bits(bits);
      this->_position.clamp_to_size(this->_size);
   }
   CLASS_TEMPLATE_PARAMS void CLASS_NAME::_advance_offset_by_bytes(size_t bytes) {
      this->_position.advance_by_bytes(bytes);
      this->_position.clamp_to_size(this->_size);
   }
   CLASS_TEMPLATE_PARAMS void CLASS_NAME::_byte_align() {
      this->_position.advance_to_next_byte();
      this->_position.clamp_to_size(this->_size);
   }
   CLASS_TEMPLATE_PARAMS void CLASS_NAME::_consume_byte(uint8_t& out, uint8_t bitcount, int& consumed) {
      auto bytepos = this->get_bytepos();
      if (bytepos >= this->_size) {
         out = 0;
         this->_position.add_overshoot_bits(bitcount);
         consumed = bitcount;
         return;
      }
      auto    shift = this->get_bitshift();
      uint8_t byte  = this->_buffer[bytepos] & (0xFF >> shift);
      int     bits_read = 8 - shift;
      if (bitcount < bits_read) {
         byte = byte >> (bits_read - bitcount);
         this->_advance_offset_by_bits(bitcount);
         consumed = bitcount;
      } else {
         this->_advance_offset_by_bits(bits_read);
         consumed = bits_read;
      }
      out = byte;
   }
   CLASS_TEMPLATE_PARAMS uint64_t CLASS_NAME::_read_bits(uint8_t bitcount) {
      uint64_t result = 0;
      uint8_t  bits;
      int      consumed;
      this->_consume_byte(bits, bitcount, consumed);
      result = bits;
      int remaining = bitcount - consumed;
      while (remaining) {
         this->_consume_byte(bits, remaining, consumed);
         result = (result << consumed) | bits;
         remaining -= consumed;
      }
      return result;
   }

   CLASS_TEMPLATE_PARAMS void CLASS_NAME::set_buffer(const uint8_t* b, size_t s) {
      this->_buffer   = b;
      this->_size     = s;
      this->_position = {};
   }
   CLASS_TEMPLATE_PARAMS inline void CLASS_NAME::set_bitpos(uint32_t bitpos) {
      uint32_t bytepos = bitpos / 8;
      this->_position.bytes = bytepos;
      this->_position.bits  = bitpos % 8;
      if (bytepos > this->size()) {
         this->_position.bytes = this->size();
         this->_position.bits  = 0;
         this->_position.set_overshoot_bits(bitpos - (this->size() * 8));
      } else
         this->_position.overshoot = {};
   }
   CLASS_TEMPLATE_PARAMS inline void CLASS_NAME::set_bytepos(uint32_t bytepos) {
      this->_position.bits  = 0;
      this->_position.bytes = bytepos;
      if (bytepos > this->size()) {
         this->_position.bytes = this->size();
         this->_position.set_overshoot_bytes(bytepos - this->size());
      } else
         this->_position.overshoot = {};
   }

   CLASS_TEMPLATE_PARAMS
   template<typename T>
   T CLASS_NAME::read_bits(uint8_t bitcount) {
      using uT = std::make_unsigned_t<cobb::strip_enum_t<T>>;
      //
      uT result = this->_read_bits(bitcount);
      return (T)result;
   }

   CLASS_TEMPLATE_PARAMS float CLASS_NAME::read_compressed_float(const int bitcount, float min, float max, bool is_signed, bool guarantee_exact_bounds) noexcept {
      uint8_t raw = this->read_bits<uint8_t>(bitcount);
      return util::decode_compressed_float(raw, bitcount, min, max, is_signed, guarantee_exact_bounds);
   }
   
   CLASS_TEMPLATE_PARAMS
   template<typename CharT>
   void CLASS_NAME::read_string(CharT* out, size_t maxlength) {
      for (uint32_t i = 0; i < maxlength; i++) {
         this->read(out[i]);
         if (!out[i])
            return;
      }
   }

   CLASS_TEMPLATE_PARAMS void CLASS_NAME::skip(uint32_t bitcount) {
      _advance_offset_by_bits(bitcount);
   }
}
//
#undef CLASS_TEMPLATE_PARAMS
#undef CLASS_NAME