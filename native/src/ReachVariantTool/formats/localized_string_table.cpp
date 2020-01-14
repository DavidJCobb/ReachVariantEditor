#include "localized_string_table.h"
extern "C" {
   #include "../../zlib/zlib.h" // interproject ref
}
#include "../game_variants/errors.h"

#define MEGALO_STRING_TABLE_COLLAPSE_METHOD_BUNGIE     1 // Only optimize single-language strings (Bungie approach)
#define MEGALO_STRING_TABLE_COLLAPSE_METHOD_DUPLICATES 2 // If two strings are exactly identical in their entirety, overlap them
#define MEGALO_STRING_TABLE_COLLAPSE_METHOD_OVERLAP    3 // If one string is exactly identical to the end of another, overlap them (my approach; needs revision for performance)
#define MEGALO_STRING_TABLE_USE_COLLAPSE_METHOD  MEGALO_STRING_TABLE_COLLAPSE_METHOD_DUPLICATES


//
// Notes on Bungie's string table compression:
//
//  - They use zlib with headers and best compression.
//
//  - The string table format theoretically allows any two strings to be overlapped 
//    in the data if they are identical, or if one is identical to the end of the 
//    other, i.e. the identical data can be stored in the file just once and the 
//    two strings can have their offset values pointed at it appropriately. Bungie 
//    only makes use of this when all localizations for a given ReachString are 
//    the same; if only some are identical, then all are serialized separately, 
//    producing duplicate data.
//

namespace reach {
   extern bool language_is_optional(language l) noexcept {
      return l == language::polish;
   }
};

int32_t ReachString::index() const noexcept {
   auto& list = this->owner.strings;
   auto  size = list.size();
   for (size_t i = 0; i < size; i++)
      if (this == list[i])
         return i;
   return -1;
}
void ReachString::read_offsets(cobb::ibitreader& stream, ReachStringTable& table) noexcept {
   for (size_t i = 0; i < this->offsets.size(); i++) {
      bool has = stream.read_bits(1) != 0;
      if (has)
         this->offsets[i] = stream.read_bits(table.offset_bitlength);
      else
         this->offsets[i] = -1;
   }
}
void ReachString::read_strings(void* buffer) noexcept {
   for (size_t i = 0; i < this->offsets.size(); i++) {
      auto off = this->offsets[i];
      if (off == -1)
         continue;
      this->strings[i].assign((const char*)((std::uintptr_t)buffer + off));
   }
}
void ReachString::write_offsets(cobb::bitwriter& stream, const ReachStringTable& table) const noexcept {
   for (auto offset : this->offsets) {
      if (offset == -1) {
         stream.write(0, 1);
         continue;
      }
      stream.write(1, 1);
      stream.write(offset, table.offset_bitlength);
   }
}
void ReachString::write_strings(std::string& out) noexcept {
   #if MEGALO_STRING_TABLE_USE_COLLAPSE_METHOD == MEGALO_STRING_TABLE_COLLAPSE_METHOD_BUNGIE
      bool allEqual = true;
      for (size_t i = 1; i < this->offsets.size(); i++) {
         auto& a = this->strings[i - 1];
         auto& b = this->strings[i];
         if (a != b) {
            allEqual = false;
            break;
         }
      }
      if (allEqual) {
         auto s = out.size();
         for (auto& offset : this->offsets)
            offset = s;
         out += this->strings[0];
         out += '\0';
         return;
      }
   #endif
   for (size_t i = 0; i < this->offsets.size(); i++) {
      auto& s = this->strings[i];
      //
      // NOTE: DO NOT serialize empty strings with a -1 offset; Bungie doesn't do that and I assume it's for a 
      // reason.
      //
      if (this->offsets[i] == -1) // string was absent when we read it, so keep it absent (in lieu of the above)
         continue;
      #if MEGALO_STRING_TABLE_USE_COLLAPSE_METHOD == MEGALO_STRING_TABLE_COLLAPSE_METHOD_DUPLICATES
         //
         // If the string table buffer already contains a string that is exactly identical to 
         // this string, then use that string's offset instead of adding a duplicate.
         //
         this->offsets[i] = -1;
         size_t j    = 0; // end   of existing string
         size_t k    = 0; // start of existing string
         size_t size = s.size();
         for (; j < out.size(); j++) {
            if (out[j] != '\0')
               continue;
            #if _DEBUG
               std::string existing = out.substr(k, j - k);
            #endif
            if (j - k == size) {
               if (strncmp(s.data(), out.data() + k, size) == 0) {
                  this->offsets[i] = k;
                  break;
               }
            }
            k = j + 1;
         }
         if (this->offsets[i] >= 0) {
            continue;
         }
      #endif
      #if MEGALO_STRING_TABLE_USE_COLLAPSE_METHOD == MEGALO_STRING_TABLE_COLLAPSE_METHOD_OVERLAP
         //
         // If one string is identical to the end of another string, then they can be overlapped: 
         // the longer string can be encoded in the string table, and the shorter string can 
         // reference its end.
         //
         // In practice, we can't do that from here; what we would have to do instead is gather 
         // every string into a pool with a reference to the (offsets) value in its owner, and 
         // then compare every string to see which strings are suffixes of other strings. This 
         // is necessary to avoid situations where the shorter string is written to the buffer 
         // first and therefore misses out on optimization.
         //
         // Moreover, the performance on this is really bad as-is precisely because we're just 
         // scanning into a buffer, blindly, and we can only even do a string comparison once 
         // we find a null char.
         //
         this->offsets[i] = -1;
         size_t j = 0; // current  null char index
         size_t k = 0; // previous null char index
         size_t size = s.size();
         for (; j < out.size(); j++) {
            if (out[j] != '\0')
               continue;
            if (j - k >= size) {
               const char* start = out.data() + j - size;
               if (strncmp(s.data(), start, size) == 0) {
                  this->offsets[i] = j - s.size();
                  break;
               }
            }
            k = j;
         }
         if (this->offsets[i] >= 0) {
            continue;
         }
      #endif
      this->offsets[i] = out.size();
      out += s;
      out += '\0';
   }
}
bool ReachString::can_be_forge_label() const noexcept {
   auto& first = this->strings[0];
   for (size_t i = 1; i < reach::language_count; i++) {
      if (this->strings[i] != first)
         return false;
   }
   return true;
}

void* ReachStringTable::_make_buffer(cobb::ibitreader& stream) const noexcept {
   uint32_t uncompressed_size = stream.read_bits(this->buffer_size_bitlength);
   bool     is_compressed     = stream.read_bits(1) != 0;
   uint32_t size = uncompressed_size;
   if (is_compressed)
      size = stream.read_bits(this->buffer_size_bitlength);
   //
   void* buffer = malloc(size);
   for (uint32_t i = 0; i < size; i++)
      *(uint8_t*)((std::uintptr_t)buffer + i) = stream.read_bits<uint8_t>(8);
   if (is_compressed) {
      //
      // The buffer has four bytes indicating the length of the decompressed data, which is 
      // redundant. We'll read those four bytes just as a sanity check, but we need to skip 
      // them -- zlib needs to receive (buffer + 4) as its input.
      //
      uint32_t uncompressed_size_2 = *(uint32_t*)(buffer);
      if (uncompressed_size_2 != uncompressed_size)
         if (_byteswap_ulong(uncompressed_size_2) != uncompressed_size)
            printf("WARNING: String table sizes don't match: Bungie 0x%08X versus zlib 0x%08X.", uncompressed_size, uncompressed_size_2);
      void* final = malloc(uncompressed_size);
      //
      // It's normally pointless to check that an allocation succeeded because if it didn't, 
      // that means you're out of memory and there's basically nothing you can do about that 
      // anyway. However, if we screwed up and misread the size, then we may try to allocate 
      // a stupidly huge amount of memory. In that case, allocation will (hopefully) fail and 
      // (final) will be nullptr, and that's worth handling.
      //
      if (final) {
         Bytef* input = (Bytef*)((std::uintptr_t)buffer + sizeof(uncompressed_size_2));
         int resultCode = uncompress((Bytef*)final, (uLongf*)&uncompressed_size_2, input, size - sizeof(uncompressed_size_2));
         if (Z_OK != resultCode) {
            free(final);
            final = nullptr;
            auto& error_report = GameEngineVariantLoadError::get();
            error_report.state         = GameEngineVariantLoadError::load_state::failure;
            error_report.reason        = GameEngineVariantLoadError::load_failure_reason::zlib_decompress_error;
            error_report.failure_point = GameEngineVariantLoadError::load_failure_point::string_table;
            error_report.extra[0]      = resultCode;
         }
      }
      free(buffer);
      buffer = final;
   }
   return buffer;
}
bool ReachStringTable::read(cobb::ibitreader& stream) noexcept {
   size_t count = stream.read_bits(this->count_bitlength);
   this->strings.reserve(count);
   for (size_t i = 0; i < count; i++) {
      auto& string = *this->strings.emplace_back(new ReachString(*this));
      string.read_offsets(stream, *this);
   }
   if (count) {
      auto buffer = this->_make_buffer(stream);
      if (buffer) {
         for (size_t i = 0; i < count; i++)
            this->strings[i]->read_strings(buffer);
         free(buffer);
      } else
         return false;
   }
   return true;
}
void ReachStringTable::write(cobb::bitwriter& stream) const noexcept {
   stream.write(this->strings.size(), this->count_bitlength);
   if (this->strings.size()) {
      std::string combined; // UTF-8 buffer holding all string data including null terminators
      for (auto& ptr : this->strings) {
         auto& string = *ptr;
         string.write_strings(combined);
         string.write_offsets(stream, *this);
      }
      uint32_t uncompressed_size = combined.size();
      if (uncompressed_size > cobb::bitmax(this->buffer_size_bitlength)) {
         printf("WARNING: TOTAL STRING DATA IS TOO LARGE");
         #if _DEBUG
            __debugbreak();
         #endif
         assert(false && "String table cannot hold data this large");
      }
      stream.write(uncompressed_size, this->buffer_size_bitlength);
      //
      bool should_compress = uncompressed_size >= 0x80; // TODO: better logic
      stream.write(should_compress, 1);
      if (should_compress) {
         auto bound = compressBound(uncompressed_size);
         auto buffer = malloc(bound);
         if (!buffer) {
            printf("WARNING: FAILED TO ALLOCATE STORAGE FOR COMPRESSED STRING DATA!");
            __debugbreak();
            assert(false && "Insufficient memory to compress string data.");
         }
         uint32_t compressed_size = bound;
         int result = compress2((Bytef*)buffer, (uLongf*)&compressed_size, (const Bytef*)combined.data(), uncompressed_size, Z_BEST_COMPRESSION);
         switch (result) {
            case Z_OK:
               break;
            case Z_MEM_ERROR:
               assert(false && "Memory error occurred when trying to compress string table.");
               break;
            case Z_BUF_ERROR:
               assert(false && "Insufficient buffer space when trying to compress string table.");
               break;
         }
         stream.write(compressed_size + 4, this->buffer_size_bitlength); // this value in the file includes the size of the next uint32_t
         static_assert(sizeof(uncompressed_size) == sizeof(uint32_t), "The redundant uncompressed size stored in with the compressed data must be 4 bytes.");
         stream.write(uncompressed_size); // redundant uncompressed size
         for (size_t i = 0; i < compressed_size; i++)
            stream.write(*(uint8_t*)((std::intptr_t)buffer + i));
         free(buffer);
      } else {
         for (size_t i = 0; i < uncompressed_size; i++)
            stream.write((uint8_t)combined[i]);
      }
   }
}
ReachString* ReachStringTable::add_new() noexcept {
   if (this->is_at_count_limit())
      return nullptr;
   auto& string = *this->strings.emplace_back(new ReachString(*this));
   return &string;
}
void ReachStringTable::remove(size_t index) noexcept {
   if (index >= this->size())
      return;
   if (this->strings[index]->get_inbound_references().size())
      return;
   this->strings.erase(this->strings.begin() + index);
}
uint32_t ReachStringTable::total_bytecount() noexcept {
   std::string combined; // UTF-8 buffer holding all string data including null terminators
   for (auto& ptr : this->strings) {
      auto& string = *ptr;
      string.write_strings(combined);
   }
   return combined.size();
}