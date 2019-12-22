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
   };
   class bit_or_byte_writer {
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