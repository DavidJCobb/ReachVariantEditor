#pragma once
#include <array>
#include <vector>
#include "../helpers/bitwise.h"
#include "../helpers/bitnumber.h"
#include "../helpers/indexed_list.h"
#include "../helpers/refcounting.h"
#include "../helpers/stream.h"
#include "../helpers/standalones/unique_pointer.h"
#include "indexed_lists.h"
#include <QString>

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
   //
   extern bool language_is_optional(language l) noexcept;
};

struct ReachStringFlags { // used for UI stuff
   ReachStringFlags() = delete;
   enum type {
      SingleLanguageString = 0x01,
      IsNotInStandardTable = 0x02,
      DisallowSaveAsNew    = 0x04, // UI flag
   };
};

class ReachString;
class ReachStringTable;

using MegaloStringIndex         = cobb::bitnumber<cobb::bitcount(112 - 1), uint8_t>;
using MegaloStringIndexOptional = cobb::bitnumber<cobb::bitcount(112), uint8_t, true>;
using MegaloStringRef           = cobb::refcount_ptr<ReachString>;

class ReachString : public indexed_list_item {
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
      ReachString(ReachStringTable& o) : owner(o) {}
      //
      ReachStringTable& owner;
      std::array<int, reach::language_count>         offsets;
      std::array<std::string, reach::language_count> strings; // UTF-8
      //
      void read_offsets(cobb::ibitreader&, ReachStringTable& table) noexcept;
      void read_strings(void* buffer) noexcept;
      void write_offsets(cobb::bitwriter& stream, const ReachStringTable& table) const noexcept;
      void write_strings(std::string& out) noexcept;
      //
      std::string& english() noexcept {
         return this->strings[(int)reach::language::english];
      }
      const std::string& english() const noexcept {
         return this->strings[(int)reach::language::english];
      }
      bool operator==(const ReachString& other) const noexcept {
         return this->strings == other.strings;
      }
      bool operator!=(const ReachString& other) const noexcept { return !(*this == other); }
      //
      bool can_be_forge_label() const noexcept;
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
      cobb::indexed_list<ReachString> strings;
      //
   protected:
      void* _make_buffer(cobb::ibitreader&) const noexcept;
   public:
      bool read(cobb::ibitreader&) noexcept;
      void write(cobb::bitwriter& stream) noexcept;
      //
      ReachString* get_entry(size_t index) const noexcept {
         if (index < this->strings.size())
            return const_cast<ReachString*>(&this->strings[index]); // this function does not, itself, modify the table, but the string returned can be modified
         return nullptr;
      }
      ReachString* lookup(const QString& english, bool& matched_multiple) const noexcept;
      //
      inline size_t capacity() const noexcept { return this->max_count; }
      inline size_t is_at_count_limit() const noexcept { return this->size() >= this->capacity(); }
      inline size_t size() const noexcept { return this->strings.size(); }
      //
      ReachString* add_new() noexcept; // returns nullptr if the table is full
      void remove(size_t index) noexcept;
      //
      uint32_t total_bytecount() noexcept; // number of bytes it would take to store all strings (accounting for overlapping and other optimizations)
};