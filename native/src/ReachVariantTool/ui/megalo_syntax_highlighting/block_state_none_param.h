#pragma once
#include "./block_state.h"
#include "./keywords.h"

namespace ui::megalo_syntax_highlighting {
   //
   // When we aren't in a code comment, string, or similar parsing state, 
   // we want to keep track of "phrases:" sequences of keywords. Thus, in 
   // the "none" state, we track the last keyword we've seen.
   //
   // We could theoretically extend this struct to track anything else that 
   // we want to be aware of when not in a code comment or string.
   //
   struct block_state_none_param {
      public:
         using underlying_type = block_state::param_type;

         static constexpr const size_t bitcount = block_state::bitcount_for_token_param;
         static constexpr const size_t bitcount_for_keyword_index = std::bit_width(all_keywords.size());

      protected:
         static constexpr const underlying_type mask_for_keyword_index = (1 << bitcount_for_keyword_index) - 1;

      protected:
         underlying_type value = 0;

      public:
         constexpr block_state_none_param() {}
         constexpr block_state_none_param(underlying_type v) : value(v) {}

         constexpr const size_t last_keyword_index() const noexcept {
            return this->value & mask_for_keyword_index;
         }
         constexpr const keyword* last_keyword() const noexcept {
            auto i = this->last_keyword_index();
            if (i < all_keywords.size())
               return &all_keywords[i];
            return nullptr;
         }
         constexpr void set_last_keyword(const keyword* k) {
            size_t index = all_keywords.size(); // sentinel value for nullptr
            if (k) {
               index = k->index;
            }
            this->value &= ~mask_for_keyword_index;
            this->value |= (index & mask_for_keyword_index);
         }

         explicit constexpr operator underlying_type() const noexcept {
            return this->value;
         }
   };
}