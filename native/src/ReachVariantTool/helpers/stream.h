#pragma once
#include "bitreader.h"
#include "bytereader.h"
#include "bitwriter.h"
#include "bytewriter.h"

namespace cobb {
   class bit_or_byte_reader {
      public:
         bit_or_byte_reader(const uint8_t* b, uint32_t l) : bits(b, l), bytes(b, l) {};
         bit_or_byte_reader(const void* b, uint32_t l) : bits(b, l), bytes(b, l) {};
         //
         bitreader  bits;
         bytereader bytes;
         //
         inline void synchronize() noexcept {
            uint32_t o = this->bits.get_bytespan();
            uint32_t b = this->bytes.get_bytepos();
            if (o > b) {
               this->bytes.set_bytepos(o);
               return;
            }
            this->bits.set_bytepos(b);
         }
         inline bool overshot_eof() noexcept {
            this->synchronize();
            return this->bytes.get_overshoot_bytes() > 0;
         }
   };
   class bit_or_byte_writer {
      //
      // A class containing both a bitwriter and a bytewriter, synching them up to share a 
      // single buffer. After using one, you must manually call (synchronize) to bring the 
      // other up to speed on what changes you've made. (The alternative would've been for 
      // me to make a single class with bit- and byte-writing functionality, along with 
      // two interface classes to serve as accessors. I suppose that's not impossible and 
      // it may be better... I might see to it after I've dealt with other tasks.)
      //
      public:
         bitwriter  bits;
         bytewriter bytes;
         //
         bit_or_byte_writer() {
            this->bits.share_buffer(this->bytes);
         }
         //
         inline void synchronize() noexcept {
            uint32_t o = this->bits.get_bytespan();
            uint32_t b = this->bytes.get_bytepos();
            if (o > b) {
               this->bytes.set_bytepos(o);
               return;
            }
            this->bits.set_bytepos(b);
         }
         void dump_to_console() const noexcept;
         void save_to(FILE* file) const noexcept;
   };
}