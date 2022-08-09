#pragma once
#include "character_scanner.h"

namespace halo::reach::megalo::bolt {
   template<typename Subclass>
   void character_scanner<Subclass>::_update_column_number() {
      //
      // Updating the column number with every character we scan isn't as simple as 
      // just incrementing it, because if we run a nested scan or run two consecutive 
      // scans, those scans will each see the same character (which would lead to 
      // extra increments). This is the same edge-case as with handling line breaks 
      // in the `scan_characters` function, and is explained there as well.
      //
      if (this->last_newline != no_newline_found)
         this->pos.col = this->pos.offset - this->last_newline - 1; // minus one to include the line break itself
      else
         this->pos.col = this->pos.offset;
   }

   template<typename Subclass>
   void character_scanner<Subclass>::scan_characters(character_scan_functor_t functor) {
      for (; this->pos.offset < this->text.size(); ++this->pos.offset) {
         this->_update_column_number();
         //
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
            if (this->pos.offset != this->last_newline) {
               this->last_newline = this->pos.offset;
               ++this->pos.line;
               this->pos.col = 0;
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
         auto prior    = this->pos.offset;
         auto prior_lb = this->last_newline;
         if (functor(c) != character_scan_result::proceed) {
            break;
         }
         if (prior < this->pos.offset) {
            //
            // The functor has consumed string content. Decrement to compensate for the increment at 
            // the end of our for loop's iterations and prevent the next unread glyph from being 
            // skipped.
            //
            --this->pos.offset; // rewind
            //
            if (this->last_newline != no_newline_found && this->pos.offset == this->last_newline - 1) {
               //
               // We rewound from (i.e. past) a line break, so we'll need to fix up the column number.
               //
               this->last_newline = prior_lb;
               this->_update_column_number();
            }
         }
      }
   }

   template<typename Subclass>
   character_scanner<Subclass>::state character_scanner<Subclass>::backup_stream_state() const {
      state out;
      out.pos          = this->pos;
      out.last_newline = this->last_newline;
      return out;
   }

   template<typename Subclass>
   void character_scanner<Subclass>::restore_stream_state(const state& s) {
      this->pos          = s.pos;
      this->last_newline = s.last_newline;
      //
      // TODO: Is this sanity check actually needed?
      //
      token_pos::value_type valid_col;
      if (s.last_newline == no_newline_found) {
         valid_col = s.pos.offset;
      } else {
         valid_col = s.pos.offset - s.last_newline - 1;
      }
      if (s.pos.col != valid_col) {
         #if _DEBUG
            __debugbreak();
         #endif
         this->last_newline = this->text.lastIndexOf('\n', this->pos.offset);
         this->_update_column_number();
      }
   }
}