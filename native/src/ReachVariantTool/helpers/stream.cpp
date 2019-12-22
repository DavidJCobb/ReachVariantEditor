#include "stream.h"
#include <cctype>
#include <cstdio>

namespace cobb {
   void bit_or_byte_writer::dump_to_console() const noexcept {
      const uint8_t* data = this->bytes.data();
      //
      printf("\nWritten stream:\n");
      //
      constexpr int column_width = 16;
      uint32_t pos = this->bytes.get_bytepos();
      //
      uint32_t rows    = pos / column_width;
      uint32_t partial = pos % column_width;
      if (partial)
         ++rows;
      for (uint32_t r = 0; r < rows; r++) {
         printf("%08X | ", r * column_width);
         //
         int length = r == rows - 1 ? partial : column_width;
         int c = 0;
         for (; c < length; c++)
            printf("%02X ", data[r * column_width + c]);
         while (c++ < column_width)
            printf("   ");
         printf("| "); // separator between bytes and chars
         c = 0;
         for (; c < length; c++) {
            unsigned char glyph = data[r * column_width + c];
            if (!isprint(glyph))
               glyph = '.';
            printf("%c", glyph);
         }
         while (c++ < column_width)
            printf(" ");
         //
         printf("\n");
      }
   }
   void bit_or_byte_writer::save_to(FILE* file) const noexcept {
      assert(file && "Cannot save to a nullptr file handle.");
      fwrite(this->bytes.data(), 1, this->bytes.get_bytespan(), file);
   }
}