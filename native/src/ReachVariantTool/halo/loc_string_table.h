#pragma once
#include <bit>
#include <optional>
#include <QString>
#include "helpers/stream.h"
#include "halo/util/dirty.h"
#include "halo/util/indexed_list.h"
#include "./bitreader.h"
#include "./bitwriter.h"
#include "./loc_string.h"
#include "./loc_string_table_details.h"

namespace halo {
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

   template<size_t MaxCount, size_t MaxBufferSize> class loc_string_table; // forward-declare for CRTP
   template<size_t MaxCount, size_t MaxBufferSize> class loc_string_table : public util::dirtiable<loc_string_table<MaxCount, MaxBufferSize>> {
      public:
         static constexpr size_t max_count       = MaxCount;
         static constexpr size_t max_buffer_size = MaxBufferSize;
         //
         static constexpr size_t offset_bitlength      = std::bit_width(max_buffer_size - 1);
         static constexpr size_t buffer_size_bitlength = std::bit_width(max_buffer_size);
         static constexpr size_t count_bitlength       = std::bit_width(max_count);

         using entry_type  = loc_string<loc_string_table<max_count, max_buffer_size>>;
         using load_result = loc_string_table_load_result;

         using offset_type = loc_string_table_details::offset_type;
         using offset_list = loc_string_table_details::offset_list;

      protected:
         struct sort_pair {
            entry_type* entry = nullptr;
            std::optional<localization_language> language;

            int compare(const sort_pair& other) const noexcept;
         };

      public:
         util::indexed_list<entry_type, max_count> strings;
      protected:
         mutable struct {
            basic_bitwriter raw;
            std::vector<sort_pair> pairs;
            uint32_t uncompressed_size = 0;
         } cached_export;

         template<bitreader_subclass Reader>
         void* _read_buffer(size_t serialized_size, bool is_compressed, size_t uncompressed_size, Reader&);

      public:
         template<bitreader_subclass Reader>
         loc_string_table_load_result read(Reader&);

         void generate_export_data() const;

         template<bitwriter_subclass Writer>
         void write(Writer&) const;

         entry_type* string_by_index(size_t);
         const entry_type* string_by_index(size_t) const;

         entry_type*       lookup(const QString& english, bool& matched_multiple);
         const entry_type* lookup(const QString& english, bool& matched_multiple) const;
         std::vector<entry_type*>       lookup(const QString& english);
         std::vector<const entry_type*> lookup(const QString& english) const;
   };
}

#include "loc_string_table.inl"