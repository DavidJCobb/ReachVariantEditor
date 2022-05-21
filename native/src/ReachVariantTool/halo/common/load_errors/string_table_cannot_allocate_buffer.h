#pragma once

namespace halo::common::load_errors {
   struct string_table_cannot_allocate_buffer {
      size_t compressed_size   = 0;
      size_t uncompressed_size = 0;
      size_t max_buffer_size   = 0;
      bool   is_compressed     = false;
      //
      bool is_zlib   = false;
      int  zlib_code = 0;
   };
}
