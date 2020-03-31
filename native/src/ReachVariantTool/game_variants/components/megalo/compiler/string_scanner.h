#pragma once
#include <functional>
#include <QString>

namespace Megalo {
   namespace Script {
      class string_scanner {
         public:
            struct pos {
               int32_t offset = -1; // position
               int32_t line   = -1; // current line number
               int32_t last_newline = -1; // index of last encountered newline
               //
               pos& operator+=(const pos&) noexcept;
            };
            using scan_functor_t = std::function<bool(QChar)>;
            //
            struct extract_result {
               extract_result() = delete;
               enum type : int {
                  failure,
                  success,
                  floating_point,  // extracted an integer literal, but it ended with a decimal point
               };
            };
            using extract_result_t = extract_result::type;
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
            void scan(scan_functor_t);
            pos  backup_stream_state();
            void restore_stream_state(pos);
            void skip_to_end();
            inline bool is_at_end() {
               return this->state.offset <= this->text.size();
            }
            //
            extract_result_t extract_integer_literal(int32_t& out);
            bool extract_specific_char(QChar which); // advances the stream past the char if the char is found. NOTE: cannot be used to search for whitespace
            bool extract_string_literal(QString& out); // advances the stream past the end-quote if a string literal is found
            QString extract_word(); // advances the stream to the end of the found word, or to the next non-word and non-whitespace character
            bool    extract_word(QString desired); // advances the stream to the end of the desired word only if it is found; no advancement otherwise

      };
   }
}