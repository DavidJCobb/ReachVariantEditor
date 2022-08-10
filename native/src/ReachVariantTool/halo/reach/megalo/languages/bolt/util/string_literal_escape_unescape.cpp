#include "./string_literal_escape_unescape.h"
#include <array>
#include "helpers/qt/string/slice.h"

namespace halo::reach::megalo::bolt::util {
   namespace {
      struct _escape_map_entry {
         QChar seq; // character that was prefixed with a backslash
         QChar out; // character to produce

         constexpr _escape_map_entry(QChar a) : seq(a), out(a) {} // for symbols which require a backslash
         constexpr _escape_map_entry(QChar a, QChar b) : seq(a), out(b) {} // for symbols that can't always be entered directly in source code
      };

      constexpr auto _escapes = std::array{
         _escape_map_entry('\''),
         _escape_map_entry('"'),
         _escape_map_entry('`'),
         _escape_map_entry('\\'),
         _escape_map_entry('a', '\a'),
         _escape_map_entry('b', '\b'),
         _escape_map_entry('f', '\f'),
         _escape_map_entry('n', 0xA),
         _escape_map_entry('r', 0xD),
         _escape_map_entry('t', '\t'),
         _escape_map_entry('v', '\v'),
         // the code '\0' is intentionally omitted, as it will not be interpreted properly // TODO: this comment is from the original compiler. what does "not be interpreted properly" even mean?
      };
   }

   extern QString string_literal_escape(QStringView text, QChar delim) {
      QString out;
      uint    size = text.size();
      for (uint i = 0; i < size; ++i) {
         QChar c = text[i];
         if (c == '\\') {
            out += R"(\\)";
            continue;
         }
         if (!c.isPrint()) {
            if (c == '\0') // do not embed null characters; they will not be interpreted properly
               break;
            out += '\\';
            //
            // Is there a defined escape sequence for this glyph?
            //
            bool match = false;
            for (const auto& item : _escapes) {
               if (c == item.seq) {
                  out += item.out;
                  match = true;
                  break;
               }
            }
            if (match)
               continue;
            //
            // There is no defined escape sequence for this glyph, so use \x or \u as appropriate.
            //
            ushort code = c.unicode();
            if (code <= 0xFF) {
               out += 'x';
               out += QString("%1").arg(code, 2, 16, QChar('0'));
               continue;
            }
            out += 'u';
            out += QString("%1").arg(code, 4, 16, QChar('0'));
            continue;
         }
         if (c == delim)
            out += '\\';
         out += c;
      }
      return out;
   }
   extern QString string_literal_unescape(QStringView text) {
      QString out;

      qsizetype  i    = 0;
      const auto size = text.size();
      for (; i < size; ++i) {
         qsizetype j = text.indexOf(QChar('\\'), i);
         if (j < 0) {
            out += cobb::qt::string::view_all_after(text, i);
            break;
         }
         out += cobb::qt::string::view_slice_from_a_to_b(text, i, j); // copy from (i) up to, but not including, the backslash at (j)
         i = j + 1; // point (i) to the character after the backslash
         if (i >= size) { // terminating backslash should never happen, but if it does, just blindly copy it in and stop
            break;
         }
         //
         auto remaining = size - i;
         //
         QChar d = text[(uint)i];
         if (d == 'x' || d == 'u') {
            uint8_t digits = 2;
            if (d == 'u')
               digits = 4;
            //
            if (remaining < digits + 1) { // number of digits plus the 'u'/'x' itself
               //
               // The remainder of the string isn't long enough for this to be a valid character 
               // escape sequence. Just insert a literal 'u' or 'x' (whichever one was found).
               //
               out += d;
               continue;
            }
            auto  view = cobb::qt::string::view_slice_of_length(text, i + 1, digits);
            bool  ok   = false;
            short num  = view.toUShort(&ok, 16);
            if (!ok) {
               out += d;
               continue;
            }
            if (num) // do not embed null characters; they will not be interpreted properly
               out += QChar(num);
            i += digits;
            continue;
         }
         //
         bool match = false;
         for (const auto& item : _escapes) {
            if (d == item.seq) {
               out += item.out;
               match = true;
               break;
            }
         }
         if (match)
            continue;
         //
         // No matching escape sequence. Just produce the original character.
         //
         out += d;
      }
      return out;
   }
}