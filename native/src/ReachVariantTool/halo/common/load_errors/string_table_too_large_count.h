#pragma once

namespace halo::common::load_errors {
   struct string_table_too_large_count {
      size_t count     = 0;
      size_t max_count = 0;
   };
}