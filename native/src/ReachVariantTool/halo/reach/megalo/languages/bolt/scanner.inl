#pragma once
#include "./scanner.h"
#include "./literal_data_number.h"

namespace halo::reach::megalo::bolt {
   template<size_t Base> requires (Base > 1 && Base <= 36)
   literal_data_number scanner::_try_extract_number_digits() {
      constexpr int decimal_point_not_yet_seen = -1;

      literal_data_number result;
      bool ignore  = false; // we found an invalid char, so the result will not be a valid number
      int  decimal = decimal_point_not_yet_seen; // have we found a decimal point? if so (value >= 0), how many digits after it have we read?

      this->scan_characters([this, &decimal, &ignore, &result](QChar c) -> character_scan_result {
         if (c == '.') {
            if (decimal != decimal_point_not_yet_seen) {
               return character_scan_result::stop_here; // stop
            }
            decimal = 0;
            result.value.decimal = result.value.integer;
            result.format = literal_data_number::format_type::decimal;
            return character_scan_result::proceed; // continue
         }
         if (c.isSpace()) {
            return character_scan_result::stop_here; // stop
         }

         auto charcode = c.unicode();

         size_t digit   = 0;
         bool   valid   = false;
         bool   consume = false; // if true, we'll treat an invalid char as part of the number literal we've read
         if constexpr (Base <= 10) {
            //
            // For bases with 10 or fewer possible digits, any ASCII glyphs that represent 
            // base-10 digits should be consumed as part of the number; if they are out of 
            // range (e.g. "9" in a binary literal), they should be treated as invalid but 
            // number literal parsing should not stop.
            //
            if constexpr (Base == 2) {
               if (c == '0' || c == '1') {
                  valid = true;
                  digit = (c == '0' ? 0 : 1);
               }
            } else {
               if (charcode >= '0' && charcode <= ('0' + Base - 1)) {
                  valid = true;
                  digit = charcode - '0';
               }
            }
            if (!valid && charcode >= '0' && charcode <= '9') {
               consume = true;
            }
         } else {
            //
            // For bases with more than 10 possible digits, we'll start with the ASCII 
            // glyphs that represent base-10 digits first, and then move into ASCII 
            // letters.
            //
            if (charcode >= '0' && charcode <= '9') {
               valid = true;
               digit = charcode - '0';
            } else {
               if (charcode >= 'A' && charcode <= 'Z') // to lowercase
                  charcode -= 0x20;
               //
               if (charcode >= 'a' && charcode <= ('a' + Base - 11)) {
                  valid = true;
                  digit = charcode - 'a' + 10;
               }
            }
         }
         //
         // We have now either extracted a digit or found a non-digit.
         //
         if (valid) {
            if (!ignore) { // only bother computing a value if no previous chars were invalid
               if (decimal != decimal_point_not_yet_seen) {
                  //
                  // TODO: Is there a way to do this that reduces floating-point imprecision? 
                  // TODO: Check for precision loss and warn?
                  //
                  result.value.decimal += (digit * std::pow(Base, -(decimal + 1)));
                  ++decimal;
               } else {
                  //
                  // TODO: Check for overflow and warn.
                  //
                  result.value.integer *= Base;
                  result.value.integer += digit;
               }
            }
         } else {
            if (!consume)
               return character_scan_result::stop_here; // stop
            ignore = true;
         }
         return character_scan_result::proceed; // continue
      });
      if (ignore) {
         result.format = literal_data_number::format_type::none;
      } else if (decimal == decimal_point_not_yet_seen) {
         result.format = literal_data_number::format_type::integer;
      } else {
         result.format = literal_data_number::format_type::decimal;
      }
      return result;
   };
}
