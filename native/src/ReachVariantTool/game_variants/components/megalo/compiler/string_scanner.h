#pragma once
#include <functional>
#include <QString>

namespace Megalo {
   namespace Script {
      class string_scanner {
         public:
            struct pos {
               int32_t offset = 0; // position
               int32_t line   = 0; // current line number
               int32_t last_newline = -1; // index of last encountered newline
               //
               pos& operator+=(const pos&) noexcept;
               //
               [[nodiscard]] int32_t col() const noexcept { return this->offset - this->last_newline; }
            };
            using scan_functor_t = std::function<bool(QChar)>;
            //
         protected:
            QString text;
            pos     state;
            //
         public:
            string_scanner(QString t) : text(t) {}
            //
            static bool is_operator_char(QChar);
            static bool is_quote_char(QChar);
            static bool is_syntax_char(QChar);
            static bool is_whitespace_char(QChar);
            //
            static QString unescape(const QString&);
            static QString escape(const QString&, QChar delim);
            //
            inline QString trimmed() const noexcept { return this->text.trimmed(); }
            //
            void scan(scan_functor_t);
            pos  backup_stream_state();
            void restore_stream_state(pos);
            bool skip_to(QChar, bool even_if_in_string = false); // skip to and past the next instance of the desired character; returns false and does not move if the character isn't found
            void skip_to_end();
            inline bool is_at_end() const noexcept { // checks if nothing, not even whitespace, remains
               return this->state.offset <= this->text.size();
            }
            bool is_at_effective_end() const; // checks if only whitespace remains // TODO: how shoudl we handle comments?
            //
            bool extract_integer_literal(int32_t& out);
            bool extract_specific_char(QChar which, bool immediate = false); // advances the stream past the char if the char is found. NOTE: cannot be used to search for whitespace. if (immediate == true), then checks the immediate next character without skipping whitespace
            bool extract_string_literal(QString& out); // advances the stream past the end-quote if a string literal is found
            QString extract_word(); // advances the stream to the end of the found word, or to the next non-word and non-whitespace character
            bool    extract_word(QString desired); // advances the stream to the end of the desired word only if it is found; no advancement otherwise
            QString extract_up_to_any_of(QString charset, QChar& out); // extracts up to any character in (charset), and then moves the stream position after that character; does not move if no character is found

      };
   }
}