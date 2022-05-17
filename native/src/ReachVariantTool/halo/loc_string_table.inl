#pragma once
#include "loc_string_table.h"

#define CLASS_TEMPLATE_PARAMS template<size_t max_count, size_t max_buffer_size>
#define CLASS_NAME loc_string_table<max_count, max_buffer_size>

namespace halo {
   #pragma region sort_pair
   CLASS_TEMPLATE_PARAMS int CLASS_NAME::sort_pair::compare(const sort_pair& other) const noexcept {
      constexpr int before = -1;
      constexpr int equal  =  0;
      constexpr int after  =  1;
      //
      // -1 = put us before the other
      //  0 = we equal the other
      //  1 = put us after the other
      //
      // Goal: Sort strings alphabetically but with a reverse iterator over the string 
      // content; if one string is exactly equal to the end of another string, put the 
      // substring last.
      //
      if (!this->entry)
         return -1;
      if (!other.entry)
         return  1;
      auto&  a  = this->entry->get_translation(this->language.value());
      auto&  b  = other.entry->get_translation(other.language.value());
      size_t sa = a.size();
      size_t sb = b.size();
      if (sa == 0)
         return after;
      if (sb == 0)
         return before;
      size_t s = sa <= sb ? sa : sb;
      for (size_t i = 0; i < s; ++i) {
         QChar ca = a[sa - i - 1];
         QChar cb = b[sb - i - 1];
         if (ca < cb)
            return before;
         if (ca > cb)
            return after;
      }
      if (sa == sb)
         return equal;
      if (sa < sb)
         return after;
      return before;
   }
   #pragma endregion

   CLASS_TEMPLATE_PARAMS loc_string_table_load_result CLASS_NAME::read(cobb::ibitreader& stream) {
      //
      // The game engine retains the string table in memory as...
      // 
      //    struct StringTable {
      //       std::array<std::array<uint16_t, 12>, max_count> offsets;
      //       std::array<uint8_t, 0x4C00> buffer;
      //       uint32_t buffer_size;
      //       uint32_t count;
      //    };
      // 
      // So it basically just pastes data from the file directly into memory, ignoring the 
      // zlib decompress step. After the entire game variant is loaded, it has a "validate" 
      // member function which checks for correctness. That function checks the string table 
      // as follows:
      // 
      //    bool StringTable::validate() const {
      //       bool result = true;
      //       if (this->count > 0x70)
      //          result = false;
      //       if (this->buffer_size > 0x4BFF)
      //          result = false;
      //       if (this->buffer[this->buffer_size] != '\0')
      //          result = false;
      //       //   
      //       for(int i = 0; i < 12; ++i) {
      //          for(int j = 0; j < this->count; ++j) {
      //             auto offset = this->offsets[j][i];
      //             if ((uint32_t)(offset + 1) <= 1) // unsigned comparison
      //                continue;
      //             if (offset < 0) {
      //                // Offset must be in bounds
      //                result = false;
      //                continue;
      //             }
      //             if (offset >= this->buffer_size) {
      //                // Offset must be in bounds
      //                result = false;
      //                continue;
      //             }
      //             if (this->buffer[offset - 1] != '\0') {
      //                // Strings must be null-separated, not merely null-terminated
      //                result = false;
      //                continue;
      //             }
      //             
      //          }
      //       }
      //       if (!result)
      //          return false;
      //    }
      // 
      // We want to honor all of these error conditions, but we should not allow all of them 
      // to stop us from loading string data; after all, we may want to attempt repairs or 
      // otherwise allow salvage of a "barely bad" game variant.
      // 
      size_t count = stream.read_bits(count_bitlength);
      if (count > max_count) {
         impl::loc_string_table::warn_on_loading_excess_strings(count, max_count);
         count = max_count;
      }
      //
      std::vector<offset_list> offsets(count);
      for (size_t i = 0; i < count; i++) {
         auto& string = *this->strings.emplace_back(*this);
         //
         auto& current_offsets = offsets[i];
         for (size_t i = 0; i < current_offsets.size(); i++) {
            bool has = stream.read_bits(1) != 0;
            if (has)
               current_offsets[i] = stream.read_bits(offset_bitlength);
            else
               current_offsets[i] = -1;
         }
      }
      //
      if (count) {
         void*  buffer = nullptr;
         size_t uncompressed_size = stream.read_bits(buffer_size_bitlength);
         //
         // TODO: If uncompressed_size >= max_buffer_size, then the variant is invalid; that should 
         // trigger a warning here.
         //
         {
            bool   is_compressed   = stream.read_bits(1) != 0;
            size_t serialized_size = uncompressed_size;
            if (is_compressed)
               serialized_size = stream.read_bits(buffer_size_bitlength);
            //
            buffer = impl::loc_string_table::load_buffer(serialized_size, is_compressed, uncompressed_size, stream);
         }
         if (buffer) {
            for (size_t i = 0; i < count; i++) {
               auto& current_string  = this->strings[i];
               auto& current_offsets = offsets[i];
               for (size_t j = 0; j < current_offsets.size(); j++) {
                  auto off = current_offsets[j];
                  if (off < 0)
                     //
                     // TODO: Negative offsets are an error in-game; they should trigger a warning here.
                     //
                     continue;
                  if (off >= uncompressed_size) {
                     return load_result{
                        .error = load_result::failure_reason::string_offset_out_of_bounds,
                        .affected_string = {
                           .index    = (int32_t)i,
                           .language = (localization_language)j,
                        },
                     };
                  }
                  if (off > 0 && ((const char*)buffer)[off - 1] != '\0') {
                     //
                     // TODO: Strings that aren't null-separated trigger an error in-game; they should trigger 
                     // a warning here.
                     //
                  }
                  current_string.set_translation((localization_language)j, (const char*)((std::uintptr_t)buffer + off));
               }
            }
            free(buffer);
         } else
            return load_result{ load_result::failure_reason::unable_to_allocate_buffer };
      }
      this->set_dirty();
      return {};
   }

   CLASS_TEMPLATE_PARAMS const typename CLASS_NAME::entry_type* CLASS_NAME::lookup(const QString& english, bool& matched_multiple) const {
      matched_multiple = false;
      entry_type* match = nullptr;
      for (auto& string : this->strings) {
         QString current = QString::fromUtf8(string.get_translation(localization_language::english).c_str());
         if (current == english) {
            if (match) {
               matched_multiple = true;
               break;
            }
            match = &string;
         }
      }
      return match;
   }
   CLASS_TEMPLATE_PARAMS typename CLASS_NAME::entry_type* CLASS_NAME::lookup(const QString& english, bool& matched_multiple) {
      matched_multiple = false;
      entry_type* match = nullptr;
      for (auto& string : this->strings) {
         QString current = QString::fromUtf8(string.get_translation(localization_language::english).c_str());
         if (current == english) {
            if (match) {
               matched_multiple = true;
               break;
            }
            match = &string;
         }
      }
      return match;
   }

   CLASS_TEMPLATE_PARAMS std::vector<typename CLASS_NAME::entry_type*> CLASS_NAME::lookup(const QString& english) {
      std::vector<entry_type*> out;
      for (auto& string : this->strings) {
         QString current = QString::fromUtf8(string.get_translation(localization_language::english).c_str());
         if (current == english)
            out.push_back(&string);
      }
      return out;
   }
   CLASS_TEMPLATE_PARAMS std::vector<const typename CLASS_NAME::entry_type*> CLASS_NAME::lookup(const QString& english) const {
      std::vector<const entry_type*> out;
      for (auto& string : this->strings) {
         QString current = QString::fromUtf8(string.get_translation(localization_language::english).c_str());
         if (current == english)
            out.push_back(&string);
      }
      return out;
   }
}

#undef CLASS_TEMPLATE_PARAMS
#undef CLASS_NAME