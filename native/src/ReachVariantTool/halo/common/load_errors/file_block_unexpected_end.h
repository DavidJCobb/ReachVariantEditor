#pragma once
#include "halo/util/four_cc.h"

namespace halo::common::load_errors {
   struct file_block_unexpected_end {
      struct {
         util::four_cc signature;
         size_t size = 0;
      } block;
      size_t overshoot = 0;
   };
}
