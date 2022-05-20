#pragma once
#include "halo/localization_language.h"

namespace halo::common::load_errors {
   struct string_table_entry_offset_out_of_bounds {
      size_t string_index  = 0;
      size_t string_offset = 0;
      localization_language language;
   };
}
