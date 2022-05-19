#pragma once
#include "halo/localization_language.h"

namespace halo::common::load_errors {
   struct string_table_entries_not_null_separated {
      size_t string_index  = 0; // string that doesn't follow a null byte
      size_t string_offset = 0;
      localization_language language;
   };
}