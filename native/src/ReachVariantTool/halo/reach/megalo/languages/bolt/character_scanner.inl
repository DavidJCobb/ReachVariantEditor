#pragma once
#include "character_scanner.h"

namespace halo::reach::megalo::bolt {
   template<typename Subclass>
   void character_scanner<Subclass>::scan_characters(character_scan_functor_t functor) {
      for (; this->pos.offset < this->text.size(); ++this->pos.offset) {
         QChar c = this->text[this->pos.offset];
         //
         if (c == '\n') {
            //
            // Before we increment the line counter, we want to double-check that we haven't seen this 
            // specific line break before. This is for two reasons:
            // 
            //  - If a functor stops scanning at a newline, then that newline will be seen twice: once 
            //    when the functor sees it and decides to stop; and then again with the next call to 
            //    (scan_characters).
            // 
            //  - If a functor runs a nested character scan at a newline, then both the outer scan and 
            //    the inner scan will see that same newline.
            //
            if (this->pos.offset != this->pos.last_newline) {
               ++this->pos.line;
               this->pos.last_newline = this->pos.offset;
            }
         }
         //
         // Subclass-provided function for special logic (e.g. code comments, etc.).
         //
         if (((my_type*)this)->_on_before_character_scanned() == character_handler_result::skip_functor) {
            continue;
         }
         //
         // Run and handle the scan functor:
         //
         auto prior  = this->pos.offset;
         if (functor(c) != character_scan_result::proceed) {
            break;
         }
         if (prior < this->pos.offset) {
            --this->pos.offset;
         }
      }
   }
}