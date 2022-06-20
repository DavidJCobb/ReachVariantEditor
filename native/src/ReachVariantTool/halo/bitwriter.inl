#pragma once
#include "bitwriter.h"
#include "helpers/bitwise.h" // cobb::bitmax
#include "util/compressed_float.h"

#define CLASS_TEMPLATE_PARAMS template<typename Subclass>
#define CLASS_NAME bitwriter<Subclass>
//
namespace halo {
   CLASS_TEMPLATE_PARAMS CLASS_NAME::~bitwriter() {
      if (this->_buffer) {
         delete this->_buffer;
         this->_buffer = nullptr;
      }
      this->_size = 0;
      this->_position = {};
   }

   CLASS_TEMPLATE_PARAMS uint8_t& CLASS_NAME::_access_byte(size_t bytepos) const noexcept {
      return *(this->_buffer + bytepos);
   }
   CLASS_TEMPLATE_PARAMS void CLASS_NAME::_ensure_room_for(unsigned int bitcount) {
      size_t bitsize = (size_t)this->_size * 8;
      size_t target  = (size_t)this->get_bitpos() + bitcount;
      if (target > bitsize) {
         target += 8 - (target % 8);
         this->resize(target / 8);
      }
   }
   
   CLASS_TEMPLATE_PARAMS
   template<typename T>
   void CLASS_NAME::_write(T value, int bits, int& remaining) {
      this->_ensure_room_for(bits);
      uint8_t& target = this->_access_byte(this->get_bytepos());
      int shift = this->get_bitshift();
      //
      value &= cobb::bitmax(bits); // needed for when signed values are sign-extended into an int64_t on the way here
      if (!shift) {
         if (bits < 8) {
            target = value << (8 - bits);
            this->_position.advance_by_bytes(1);
            return;
         }
         target = (value >> (bits - 8));
         this->_position.advance_by_bytes(1);
         remaining -= 8;
      } else {
         int extra = 8 - shift;
         if (bits <= extra) {
            target |= (value << (extra - bits));
            this->_position.advance_by_bits(bits);
            return;
         }
         target &= ~(uint8_t(0xFF) >> shift); // clear the bits we're about to write
         target |= ((value >> (bits - extra)) & 0xFF);
         this->_position.advance_by_bits(extra);
         remaining -= extra;
      }
      if (remaining > 0)
         this->_write(value, remaining, remaining);
   }


   CLASS_TEMPLATE_PARAMS void CLASS_NAME::enlarge_by(size_t bytes) {
      this->resize(this->_size + bytes);
   }
   CLASS_TEMPLATE_PARAMS void CLASS_NAME::resize(size_t size) {
      if (this->_size == size)
         return;
      this->_buffer = (uint8_t*)realloc(this->_buffer, size);
      assert(this->_buffer != nullptr); // resize failed; old memory not freed
      this->_size = size;
      this->_position.bytes = size;
      this->_position.bits  = 0;
   }

   CLASS_TEMPLATE_PARAMS
   template<typename T> requires impl::bitwriter::is_bitwriteable<T>
   void CLASS_NAME::write_bits(size_t bitcount, T value) {
      this->_ensure_room_for(bitcount);

      // For signed values, the extra bits will always be 1 (i.e. sign-extended), so we 
      // need to shear those off for the write to work properly.
      value &= cobb::bitmax(bitcount);
      
      while (bitcount > 0) {
         uint8_t& target = this->_access_byte(this->get_bytepos());
         int shift = this->get_bitshift();
         //
         if (!shift) {
            if (bitcount < 8) {
               target = value << (8 - bitcount);
               this->_position.advance_by_bytes(1);
               return;
            }
            target = (value >> (bitcount - 8));
            this->_position.advance_by_bytes(1);
            bitcount -= 8;
            continue;
         }
         int extra = 8 - shift;
         if (bitcount <= extra) {
            target |= (value << (extra - bitcount));
            this->_position.advance_by_bits(bitcount);
            return;
         }
         target &= ~(uint8_t(0xFF) >> shift); // clear the bits we're about to write
         target |= ((value >> (bitcount - extra)) & 0xFF);
         this->_position.advance_by_bits(extra);
         bitcount -= extra;
      }
   }

   CLASS_TEMPLATE_PARAMS void CLASS_NAME::write_compressed_float(float value, const int bitcount, float min, float max, bool is_signed, bool guarantee_exact_bounds) {
      auto result = halo::util::encode_compressed_float(value, bitcount, min, max, is_signed, guarantee_exact_bounds);
      this->write_bits(bitcount, result);
   }

   CLASS_TEMPLATE_PARAMS
   template<typename CharT>
   void CLASS_NAME::write_string(const CharT* value, size_t max_length) {
      for (size_t i = 0; i < max_length; i++) {
         this->write(value[i]);
         if (!value[i])
            break;
      }
   }

   CLASS_TEMPLATE_PARAMS
   template<typename T>
   void CLASS_NAME::write_bitstream(const bitwriter<T>& other) {
      const uint8_t* src      = other.data();
      const size_t   bytesize = other.size();
      const size_t   bitshift = other.get_bitshift();
      this->_ensure_room_for(other.get_bitpos());
      //
      size_t copy = bytesize;
      if (bitshift)
         --copy;
      //
      if (this->get_bitshift() == 0) {
         //
         // We're byte-aligned, so we can just memcpy most or maybe even all 
         // of the input.
         //
         memcpy(this->data() + this->get_bytepos(), src, copy);
         if (bitshift) {
            //
            // If the input isn't byte-aligned, then the last byte will be a 
            // partial one.
            //
            this->write_bits(bitshift, src[bytesize - 1]);
         }
         return;
      }
      //
      // We're not byte-aligned, though the input may be.
      //
      for (size_t i = 0; i < copy - 1; ++i) {
         this->write_bits(8, src[i]);
      }
      if (bitshift) {
         this->write_bits(bitshift, src[bytesize - 1]);
      }
   }
}
//
#undef CLASS_TEMPLATE_PARAMS
#undef CLASS_NAME