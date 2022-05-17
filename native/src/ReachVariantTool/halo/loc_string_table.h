#pragma once
#include <bit>
#include <optional>
#include <QString>
#include "../helpers/stream.h"
#include "util/dirty.h"
#include "util/indexed_list.h"
#include "loc_string.h"

namespace halo {
   namespace impl::loc_string_table {
      extern void* load_buffer(size_t bytecount, bool is_compressed, size_t uncompressed_size, cobb::ibitreader&);
      extern void warn_on_loading_excess_strings(size_t claimed, size_t maximum);
   }

   struct loc_string_table_load_result {
      enum class failure_reason {
         none,
         //
         count_above_maximum, // mitigation: only load up to the maximum string count
         buffer_size_gte_maximum,
         last_buffer_byte_was_not_null,  // mitigation: ignore
         strings_are_not_null_separated, // mitigation: ignore
         string_offset_out_of_bounds,
         //
         unable_to_allocate_buffer,
         zlib_decompress_error,
      };

      failure_reason error = failure_reason::none;
      struct {
         int32_t index = -1;
         localization_language language = {};
      } affected_string;

      bool is_error() const { return error != failure_reason::none; }
   };

   template<size_t max_count, size_t max_buffer_size> class loc_string_table; // forward-declare for CRTP
   template<size_t max_count, size_t max_buffer_size> class loc_string_table : public util::dirtiable<loc_string_table<max_count, max_buffer_size>> {
      public:
         static constexpr size_t offset_bitlength      = std::bit_width(max_buffer_size - 1);
         static constexpr size_t buffer_size_bitlength = std::bit_width(max_buffer_size);
         static constexpr size_t count_bitlength       = std::bit_width(max_count);

         using entry_type  = loc_string<loc_string_table<max_count, max_buffer_size>>;
         using load_result = loc_string_table_load_result;

         enum class save_error {
            none,
            data_too_large,
            out_of_memory,
            zlib_memory_error,
            zlib_buffer_error,
         };

      protected:
         using offset_type = uint16_t; // same as used by the game internally
         using offset_list = std::array<offset_type, loc_string_base::translation_count>;
         struct sort_pair {
            entry_type* entry = nullptr;
            std::optional<localization_language> language;

            int compare(const sort_pair& other) const noexcept;
         };

      public:
         util::indexed_list<entry_type, max_count> strings;
      protected:
         struct {
            cobb::bitwriter raw;
            std::vector<sort_pair> pairs;
            uint32_t uncompressed_size = 0;
         } cached_export;

      public:
         loc_string_table_load_result read(cobb::ibitreader&);

         entry_type*       lookup(const QString& english, bool& matched_multiple);
         const entry_type* lookup(const QString& english, bool& matched_multiple) const;
         std::vector<entry_type*>       lookup(const QString& english);
         std::vector<const entry_type*> lookup(const QString& english) const;
   };
}

#include "loc_string_table.inl"