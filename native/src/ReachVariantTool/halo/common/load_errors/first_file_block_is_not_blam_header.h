#pragma once
#include "halo/util/four_cc.h"

namespace halo::common::load_errors {
   struct first_file_block_is_not_blam_header {
      struct {
         util::four_cc signature;
         size_t size = 0;
      } found;
   };
}
