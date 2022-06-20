#pragma once
#include <vector>
#include "loc_string_table.h"
extern "C" {
   #include "../../zlib/zlib.h" // interproject ref
}
#include "halo/common/load_process_messages/string_table/cannot_allocate_buffer.h"
#include "halo/common/load_process_messages/string_table/entries_not_null_separated.h"
#include "halo/common/load_process_messages/string_table/entry_offset_out_of_bounds.h"
#include "halo/common/load_process_messages/string_table/mismatched_sizes.h"
#include "halo/common/load_process_messages/string_table/too_large_buffer.h"
#include "halo/common/load_process_messages/string_table/too_large_count.h"

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


   CLASS_TEMPLATE_PARAMS
   template<bitreader_subclass Reader>
   void* CLASS_NAME::_read_buffer(size_t serialized_size, bool is_compressed, size_t uncompressed_size, Reader& stream) {
      //
      void* buffer = malloc(serialized_size);
      for (uint32_t i = 0; i < serialized_size; i++)
         *(uint8_t*)((std::uintptr_t)buffer + i) = stream.read_bits<uint8_t>(8);
      if (is_compressed) {
         //
         // The buffer has four bytes indicating the length of the decompressed data, which is 
         // redundant. We'll read those four bytes just as a sanity check, but we need to skip 
         // them -- zlib needs to receive (buffer + 4) as its input.
         //
         uint32_t uncompressed_size_2 = *(uint32_t*)(buffer);
         if (uncompressed_size_2 != uncompressed_size) {
            if (_byteswap_ulong(uncompressed_size_2) != uncompressed_size) {
               stream.template emit_warning<halo::common::load_process_messages::string_table_mismatched_sizes>({
                  .bungie_size = uncompressed_size,
                  .zlib_size   = uncompressed_size_2,
               });
            }
         }
         void* result = malloc(uncompressed_size);
         if (!result) {
            stream.template emit_error<halo::common::load_process_messages::string_table_cannot_allocate_buffer>({
               .compressed_size   = serialized_size,
               .uncompressed_size = uncompressed_size,
               .max_buffer_size   = max_buffer_size,
               .is_compressed     = is_compressed,
               //
               .is_zlib = false,
            });
            return nullptr;
         }
         //
         // It's normally pointless to check that an allocation succeeded because if it didn't, 
         // that means you're out of memory and there's basically nothing you can do about that 
         // anyway. However, if we screwed up and misread the size, then we may try to allocate 
         // a stupidly huge amount of memory. In that case, allocation will (hopefully) fail and 
         // (final) will be nullptr, and that's worth handling.
         //
         if (result) {
            Bytef* input = (Bytef*)((std::uintptr_t)buffer + sizeof(uncompressed_size_2));
            int zlib_result = uncompress((Bytef*)result, (uLongf*)&uncompressed_size_2, input, serialized_size - sizeof(uncompressed_size_2));
            if (zlib_result != Z_OK) {
               free(result);
               result = nullptr;
               //
               stream.load_process().template emit_error<halo::common::load_process_messages::string_table_cannot_allocate_buffer>({
                  .compressed_size   = serialized_size,
                  .uncompressed_size = uncompressed_size,
                  .max_buffer_size   = max_buffer_size,
                  .is_compressed     = is_compressed,
                  //
                  .is_zlib   = true,
                  .zlib_code = zlib_result,
               });
            }
         }
         free(buffer);
         buffer = result;
      }
      return buffer;
   }

   CLASS_TEMPLATE_PARAMS
   template<bitreader_subclass Reader> loc_string_table_load_result CLASS_NAME::read(Reader& stream) {
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
         stream.template emit_error_at<halo::common::load_process_messages::string_table_too_large_count>(
            {
               .count     = count,
               .max_count = max_count,
            },
            stream.get_position().rewound_by_bits(count_bitlength)
         );
         count = max_count;
      }
      //
      const auto pos_before_offsets = stream.get_position();
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
         bool   is_compressed;
         size_t uncompressed_size = stream.read_bits(buffer_size_bitlength);
         size_t serialized_size   = uncompressed_size;
         if (uncompressed_size >= max_buffer_size) {
            stream.template emit_error_at<halo::common::load_process_messages::string_table_too_large_buffer>(
               {
                  .max_buffer_size   = max_buffer_size,
                  .uncompressed_size = uncompressed_size,
               },
               stream.get_position().rewound_by_bits(buffer_size_bitlength)
            );
         }
         is_compressed = stream.read_bits(1) != 0;
         if (is_compressed)
            serialized_size = stream.read_bits(buffer_size_bitlength);
         buffer = this->_read_buffer(serialized_size, is_compressed, uncompressed_size, stream);
         //
         // Read buffer:
         //
         if (buffer) {
            for (size_t i = 0; i < count; i++) {
               auto& current_string  = this->strings[i];
               auto& current_offsets = offsets[i];
               for (size_t j = 0; j < current_offsets.size(); j++) {
                  auto off = current_offsets[j];
                  if (off == -1) { // game skips this
                     continue;
                  }
                  if (off < 0) {
                     //
                     // Negative offsets (besides -1) fail validation in-game.
                     //
                     stream.template emit_error<halo::common::load_process_messages::string_table_entry_offset_out_of_bounds>(
                        {
                           .string_index  = (size_t)i,
                           .string_offset = (size_t)off,
                           .language      = (localization_language)j,
                        }
                     );
                     continue;
                  }
                  if (off >= uncompressed_size) {
                     stream.template emit_error<halo::common::load_process_messages::string_table_entry_offset_out_of_bounds>({
                        .string_index  = (size_t)i,
                        .string_offset = (size_t)off,
                        .language      = (localization_language)j,
                     });
                     continue;
                  }
                  if (off > 0 && ((const char*)buffer)[off - 1] != '\0') {
                     //
                     // Strings that aren't null-separated will fail validation in-game. You aren't allowed 
                     // to overlap strings unless they are exactly identical.
                     //
                     stream.template emit_error<halo::common::load_process_messages::string_table_entries_not_null_separated>({
                        .string_index  = (size_t)i,
                        .string_offset = (size_t)off,
                        .language      = (localization_language)j,
                     });
                  }
                  current_string.set_translation((localization_language)j, (const char*)((std::uintptr_t)buffer + off));
               }
            }
            free(buffer);
         } else {
            return load_result{ load_result::failure_reason::unable_to_allocate_buffer };
         }
      }
      this->set_dirty();
      return {};
   }

   CLASS_TEMPLATE_PARAMS void CLASS_NAME::generate_export_data() const {
      if (!this->is_dirty() && this->cached_export.raw.get_bitpos() > 0)
         return;
      //
      // The format for a string table in the file is as follows:
      //
      //  - First, the number of ReachStrings.
      //
      //  - Then, the offsets for each localization in each ReachString.
      //
      //     - Each offset is written as a presence bit followed, if the bit is 1, by the offset 
      //       value.
      //
      //  - Then, the size of the uncompressed data in the next bullet point.
      //
      //  - Then, a buffer containing all of the content of all localizations of all ReachStrings. 
      //    This buffer may optionally be zlib-compressed. The ReachString offsets mentioned above 
      //    are relative to the start of this buffer's uncompressed content.
      //
      auto& stream = this->cached_export.raw;
      stream.resize(0); // clear the cached data
      this->cached_export.uncompressed_size = 0;
      //
      stream.write_bits(count_bitlength, this->strings.size());
      std::string combined; // UTF-8 buffer holding all string data including null terminators
      if (!this->strings.size()) {
         this->_clear_dirty();
         return;
      }
      //
      {
         std::vector<offset_list> offsets(this->strings.size());
         for (size_t i = 0; i < this->strings.size(); ++i) {
            auto& current_string  = this->strings[i];
            auto& current_offsets = offsets[i];
            current_string.serialize_to_blob(combined, offsets[i]);
            //
            for (size_t i = 0; i < current_offsets.size(); i++) {
               if (current_offsets[i] == -1) {
                  stream.write_bits(1, false);
                  continue;
               }
               stream.write_bits(1, true);
               stream.write_bits(offset_bitlength, current_offsets[i]);
            }
         }
      }
      //
      uint32_t uncompressed_size = combined.size();
      this->cached_export.uncompressed_size = uncompressed_size;
      if (uncompressed_size > cobb::bitmax(this->buffer_size_bitlength)) {
         #if _DEBUG
            __debugbreak();
         #endif
         return;
      }
      stream.write(uncompressed_size, this->buffer_size_bitlength);
      //
      bool should_compress = uncompressed_size >= 0x80;
      stream.write(should_compress, 1);
      //
      if (!should_compress) {
         for (size_t i = 0; i < uncompressed_size; i++)
            stream.write((uint8_t)combined[i]);
         this->_clear_dirty();
         return;
      }
      //
      // Compress the data with zlib, and then write it to the stream.
      //
      auto bound  = compressBound(uncompressed_size);
      auto buffer = malloc(bound);
      uint32_t compressed_size = bound;
      int result = compress2((Bytef*)buffer, (uLongf*)&compressed_size, (const Bytef*)combined.data(), uncompressed_size, Z_BEST_COMPRESSION);
      assert(result != Z_MEM_ERROR); // malloc failed within zlib; if we're out of memory, there's not really anything we can do about that
      if (result == Z_BUF_ERROR) {
         //
         // Destination buffer not large enough to hold compressed data. This should not be possible here, 
         // but just to be sure, let's try it again.
         //
         bound *= 2;
         buffer = realloc(buffer, bound);
         assert(buffer != nullptr);
         result = compress2((Bytef*)buffer, (uLongf*)&compressed_size, (const Bytef*)combined.data(), uncompressed_size, Z_BEST_COMPRESSION);
         assert(result != Z_MEM_ERROR); // malloc failed within zlib; if we're out of memory, there's not really anything we can do about that
         assert(result != Z_BUF_ERROR); // destination buffer not large enough to hold input; this should not be possible here
      }
      stream.write(compressed_size + 4, this->buffer_size_bitlength); // this value in the file includes the size of the next uint32_t
      static_assert(sizeof(uncompressed_size) == sizeof(uint32_t), "The redundant uncompressed size stored in with the compressed data must be 4 bytes.");
      stream.write(uncompressed_size); // redundant uncompressed size
      for (size_t i = 0; i < compressed_size; i++)
         stream.write(*(uint8_t*)((std::intptr_t)buffer + i));
      free(buffer);
      this->_clear_dirty();
   }

   CLASS_TEMPLATE_PARAMS
   template<bitwriter_subclass Writer> void CLASS_NAME::write(Writer& stream) const {
      this->generate_export_data();
      stream.write_bitstream(this->cached_export.raw);
   }


   CLASS_TEMPLATE_PARAMS typename CLASS_NAME::entry_type* CLASS_NAME::string_by_index(size_t i) {
      return &this->strings[i];
   }
   CLASS_TEMPLATE_PARAMS const typename CLASS_NAME::entry_type* CLASS_NAME::string_by_index(size_t i) const {
      return &this->strings[i];
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