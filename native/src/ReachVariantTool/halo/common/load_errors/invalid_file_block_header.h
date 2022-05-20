#pragma once
#include "halo/util/four_cc.h"

namespace halo::common::load_errors {
   struct invalid_file_block_header {
      struct {
         util::four_cc signature;
         size_t size = 0;
      } expected;
      struct {
         util::four_cc signature;
         size_t size = 0;
      } found;
   };
}
