#pragma once
#include <bit>
#include "./block_type.h"

namespace ui::megalo_syntax_highlighting {
   //
   // Qt syntax highlighters are fed "blocks" to parse, and some parsing 
   // states that we're interested in (e.g. "are we in a code comment?") 
   // may spread across multiple blocks. Qt allows us to store a single 
   // `int` to track these states; this `block_state` class wraps that 
   // integer to let us more easily break it into multiple pieces of data.
   // 
   // See also: `block_state_none_param`.
   //
   struct block_state {
      public:
         using underlying_type = int;
         using param_type      = std::make_unsigned_t<underlying_type>;

         static constexpr const size_t bitcount = sizeof(underlying_type) * 8;
         static constexpr const size_t bitcount_for_block_type  = std::bit_width(num_block_types);
         static constexpr const size_t bitcount_for_token_param = bitcount - bitcount_for_block_type;

      protected:
         static constexpr const underlying_type mask_for_token_param = (1 << bitcount_for_token_param) - 1;

      public:
         static constexpr const underlying_type undefined = -1;

         static constexpr const auto max_supported_param = (param_type)mask_for_token_param;

      protected:
         underlying_type value = undefined;

      public:
         block_state() {}
         block_state(underlying_type i) : value(i) {}
         block_state(block_type t, param_type param = 0) {
            this->value = ((underlying_type)t << bitcount_for_token_param) | ((underlying_type)param & mask_for_token_param);
         }

         constexpr void clear() noexcept {
            this->value = 0;
         }
         constexpr bool is_none() const noexcept {
            return this->value == undefined || this->type() == block_type::none;
         }
         constexpr param_type param() const noexcept {
            return (param_type)this->value & mask_for_token_param;
         }
         constexpr block_type type() const noexcept {
            if (this->value == undefined)
               return block_type::none;
            return (block_type)((std::make_unsigned_t<underlying_type>)this->value >> bitcount_for_token_param);
         }

         explicit constexpr operator underlying_type() const noexcept {
            return this->value;
         }
   };
}