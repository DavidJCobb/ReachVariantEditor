#pragma once

namespace halo::common::load_errors {
   struct string_table_too_large_buffer {
      size_t max_buffer_size   = 0;
      size_t uncompressed_size = 0;
   };
}