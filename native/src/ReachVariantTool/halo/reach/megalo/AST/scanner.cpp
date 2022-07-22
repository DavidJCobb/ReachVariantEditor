#include "./scanner.h"
#include <cstdint>

namespace halo::reach::megalo::AST {
   token_pos scanner::backup_stream_state() const {
      return this->pos;
   }
   void scanner::restore_stream_state(const token_pos& o) {
      this->pos = o;
   }

   std::optional<literal_number> scanner::try_extract_number_literal(bool immediate) {
      std::optional<literal_number> result;

      if (!immediate)
         this->skip_whitespace();
      //
      auto prior = this->backup_stream_state();
      //
      int8_t sign = 0;
      decltype(prior) after_sign;
      if (this->extract_specific_char('-', true)) {
         sign       = -1;
         after_sign = this->backup_stream_state();
      } else if (this->extract_specific_char('+', true)) {
         sign = 1;
         after_sign = this->backup_stream_state();
      } else {
         after_sign = prior;
      }
      size_t base_prefix = 0;
      if (this->extract_specific_char('0', true)) {
         if (this->extract_specific_char('b', true)) { // base-2 number literal
            base_prefix = 2;
            result = this->_try_extract_number_digits<2>();
         } else if (this->extract_specific_char('x', true)) { // base-16 number literal
            base_prefix = 16;
            result = this->_try_extract_number_digits<16>();
         } else {
            this->restore_stream_state(after_sign);
         }
      }
      if (!base_prefix) {
         result = this->_try_extract_number_digits<10>();
      }

      if (!result.has_value()) {
         this->restore_stream_state(prior);
         return result;
      }
      if (sign) {
         auto& v = result.value();
         switch (v.format) {
            case literal_number_format::none:
               break;
            case literal_number_format::integer:
               v.value.integer *= sign;
               break;
            case literal_number_format::decimal:
               v.value.decimal *= sign;
               break;
         }
      }
      return result;
   }
}