#pragma once
#include <functional>
#include <limits>
#include <QString>
#include "./token_pos.h"

namespace halo::reach::megalo::bolt {
   //
   // A basic class which handles the logic for scanning through a string character 
   // by character, keeping track of line numbers. You can pass a character scan 
   // functor to it to extract tokens.
   //
   template<typename Subclass>
   class character_scanner {
      public:
         using base_type = character_scanner<Subclass>;
         using my_type   = Subclass;

         // Return codes for functors passed to scanner::scan_characters
         enum class character_scan_result {
            proceed,   // continue scanning characters
            stop_here, // stop here; the current character will be seen again by the next scan_characters call
         };
         using character_scan_functor_t = std::function<character_scan_result(QChar)>;

         struct state {
            friend base_type;
            friend my_type;
            public:
               token_pos pos;
               size_t    last_newline = no_newline_found;

            protected:
               state() = default;
         };

      private:
         void _update_column_number();

         static constexpr size_t no_newline_found = std::numeric_limits<size_t>::max();

      protected:
         const QString text;
         token_pos pos;
         size_t    last_newline = no_newline_found;

         enum class character_handler_result {
            proceed,
            skip_functor,
         };

         // Subclasses are expected to override this. Return false to skip running the 
         // provided character scan functor for the current iteration.
         character_handler_result _on_before_character_scanned() {};

      public:
         character_scanner(const QString& s) : text(s) {}
         character_scanner(QString&& s) : text(s) {}

         // If you're using a character scan functor to extract some sort of token, 
         // you should stop iteration upon reaching the end of that token, i.e. when 
         // the stream position is at the next glyph after the token's end.
         void scan_characters(character_scan_functor_t);

         state backup_stream_state() const;
         void restore_stream_state(const state&);
   };
}

#include "character_scanner.inl"