#pragma once

namespace halo::common::load_errors {
   struct string_table_mismatched_sizes {
      size_t bungie_size = 0;
      size_t zlib_size   = 0;
   };
}
