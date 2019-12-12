#include "bitstream.h"
#include <cassert>
#include "bitwise.h"

namespace cobb {
   uint8_t bitstream::_grab_byte(uint32_t bytepos) const noexcept {
      uint8_t result;
      if (this->buffer.read_from(bytepos, result) == sizeof(uint8_t))
         return result;
      return 0;
   }
   /*// Split into a templated function so we can support int64s:
   uint32_t bitstream::read_bits(int count, uint32_t flags) noexcept {
      auto pos   = this->get_bytepos();
      auto shift = this->get_bitshift();
      uint32_t bits = this->_grab_byte(pos) & (0xFF >> shift);
      int      bits_read = 8 - shift;
      if (count < bits_read) {
         bits = bits >> (bits_read - count);
         this->offset += count;
      } else {
         this->offset += bits_read;
         auto remaining = count - bits_read;
         if (remaining) {
            auto next = this->read_bits(remaining);
            bits = (bits << remaining) | next;
         }
      }
      if (flags & read_flags::swap) {
         bits = cobb::bitswap<uint32_t>(bits, count);
      }
      if (flags & read_flags::is_signed) {
         uint32_t m = 1 << (count - 1);
         bits = (bits ^ m) - m;
      }
      assert(this->offset == (pos * 8 + shift) + count && "Failed to advance by the correct number of bits.");
      return bits;
   }
   //*/
   //
   void bitstream::read_string(char* out, int maxlength) noexcept {
      int  i = 0;
      char c;
      this->read(c);
      while (c && i < maxlength) {
         out[i++] = c;
         this->read(c);
      }
   }
   void bitstream::read_wstring(wchar_t* out, int maxlength) noexcept {
      int     i = 0;
      wchar_t c;
      this->read(c);
      while (c && i < maxlength) {
         out[i++] = c;
         this->read(c);
      }
   }
   //
   void bytestream::read_string(char* out, int maxlength) noexcept {
      for (int i = 0; i < maxlength; i++)
         this->read(out[i]);
   }
   void bytestream::read_wstring(wchar_t* out, int maxlength) noexcept {
      for (int i = 0; i < maxlength; i++)
         this->read(out[i]);
   }
}