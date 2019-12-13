#pragma once
#include <array>
#include <vector>
#include "../helpers/bitwise.h"
#include "../helpers/bitnumber.h"
#include "../helpers/bitstream.h"

namespace reach {
   enum class language {
      english,
      japanese,
      german,
      french,
      spanish,
      mexican,
      italian,
      korean,
      chinese_traditional,
      chinese_simplified,
      portugese,
      polish, // optional
   };
   constexpr int language_count = (int)language::polish + 1;
};

using MegaloStringIndex = cobb::bitnumber<cobb::bitcount(112 - 1), uint8_t>;

class ReachStringTable;

class ReachString {
   public:
      //
      // (offsets) is the offset into the string table's raw content (i.e. the content 
      // after it has been decompressed, if it ever was compressed to begin with); this 
      // is used to seek into the buffer and load each string directly.
      //
      // If two strings (i.e. two languages) have identical content, then the shared 
      // text will be written to the table just once, and the languages will share an 
      // offset value (this would be another reason why offsets are used).
      //
      // If a string is not present, then it should have an offset of -1.
      //
      // Strings are encoded as UTF-8.
      //
      std::array<int, reach::language_count>         offsets;
      std::array<std::string, reach::language_count> strings; // UTF-8
      //
      void read_offsets(cobb::bitstream&, ReachStringTable& table) noexcept;
      void read_strings(void* buffer) noexcept;
      //
      const std::string& english() const noexcept {
         return this->strings[(int)reach::language::english];
      }
};

class ReachStringTable {
   public:
      const int max_count;
      const int max_buffer_size;
      const int offset_bitlength;
      const int buffer_size_bitlength;
      const int count_bitlength;
      //
      ReachStringTable(int mc, int mbs) :
         max_count(mc),
         max_buffer_size(mbs),
         offset_bitlength(cobb::bitcount(max_buffer_size - 1)),
         buffer_size_bitlength(cobb::bitcount(max_buffer_size)),
         count_bitlength(cobb::bitcount(max_count))
      {}
      ReachStringTable(int mc, int mbs, int obl, int bsbl) :
         max_count(mc),
         max_buffer_size(mbs),
         offset_bitlength(obl),
         buffer_size_bitlength(bsbl),
         count_bitlength(cobb::bitcount(max_count))
      {}
      //
      std::vector<ReachString> strings;
      //
   protected:
      void* _make_buffer(cobb::bitstream&) const noexcept;
   public:
      void read(cobb::bitstream&) noexcept;
};