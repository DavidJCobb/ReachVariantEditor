#pragma once

namespace ui::megalo_syntax_highlighting {
   enum class block_type {
      none,
      comment_line,
      comment_block,
      string_simple,
      string_block,

      __COUNT
   };
   constexpr const size_t num_block_types = (size_t)block_type::__COUNT;
}