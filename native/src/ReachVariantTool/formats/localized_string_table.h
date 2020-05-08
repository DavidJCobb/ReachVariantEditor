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
      //
      not_a_language = -1
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
   friend ReachStringTable;
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
   protected:
      ReachStringTable* owner = nullptr;
      std::array<int,         reach::language_count> offsets;
      std::array<std::string, reach::language_count> strings; // UTF-8
      bool dirty = true;
      //
      void _set_dirty() noexcept;
      //
   public:
      ReachString() {}
      ReachString(ReachStringTable& o) : owner(&o) {}
      //
      using language_list_t = std::array<std::string, reach::language_count>;
      //
      void read_offsets(cobb::ibitreader&, ReachStringTable& table) noexcept;
      void read_strings(void* buffer) noexcept;
      void write_offsets(cobb::bitwriter& stream, const ReachStringTable& table) const noexcept;
      void write_strings(std::string& out) noexcept;
      //
      inline const std::string& get_content(reach::language l) const noexcept {
         return this->strings[(int)l];
      }
      inline const language_list_t& languages() const noexcept { return this->strings; }
      //
      void set_content(reach::language, const std::string& text) noexcept;
      void set_content(reach::language, const char* text) noexcept;
      void set_content(reach::language, std::string&& text) noexcept;
      //
      inline std::string& get_write_access(reach::language l) noexcept {
         this->_set_dirty();
         return this->strings[(int)l];
      }
      //
      bool operator==(const ReachString& other) const noexcept {
         return this->strings == other.strings;
      }
      bool operator!=(const ReachString& other) const noexcept { return !(*this == other); }
      ReachString& operator=(const ReachString& other) noexcept;
      //
      bool can_be_forge_label() const noexcept;
      bool empty() const noexcept;
      inline bool is_dirty() const noexcept { return this->dirty; }
};

class ReachStringTable {
   public:
      enum class save_error {
         none,
         data_too_large,
         out_of_memory,
         zlib_memory_error,
         zlib_buffer_error,
      };
      //
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
      bool dirty = true;
      //
      bool  _check_dirty() const noexcept;
      void* _make_buffer(cobb::ibitreader&) const noexcept; // for reading
      void  _set_not_dirty() noexcept;
      //
      struct _sort_pair {
         ReachString*    entry    = nullptr;
         reach::language language = reach::language::not_a_language;
         //
         _sort_pair() {}
         _sort_pair(ReachString* s, reach::language l) : entry(s), language(l) {}
         int compare(const _sort_pair& other) const noexcept;
      };
      struct {
         cobb::bitwriter raw;
         std::vector<_sort_pair> pairs;
         uint32_t uncompressed_size = 0;
      } cached_export;
      //
      void _remove_from_sorted_pairs(ReachString*);
      void _ensure_sort_pairs_for(ReachString*);
      void _ensure_sort_pairs_for_all_strings();
      void _sort_all_pairs();
      //
   public:
      bool read(cobb::ibitreader&) noexcept;
      void write(cobb::bitwriter& stream) noexcept;
      //
      save_error generate_export_data() noexcept; // returns success bool
      uint32_t get_size_to_save() noexcept;
      ReachString* get_empty_entry() const noexcept;
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
      //
      inline bool is_dirty() const noexcept { return this->dirty; }
      inline void set_dirty() noexcept { this->dirty = true; }; // for ReachString
};