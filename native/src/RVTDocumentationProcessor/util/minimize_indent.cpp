#include "minimize_indent.h"

namespace util {
   extern uint get_minimum_indent(const QString& text, uint* out_lns) {
      if (text.isEmpty()) {
         if (out_lns)
            *out_lns = 0;
         return 0;
      }
      uint lowest  = std::numeric_limits<uint>::max();
      uint current = 0;
      //
      uint last_non_space = text.size() - 1;
      for (uint i = 0; i < text.size(); ++i) {
         QChar c = text[i];
         if (c == '\r')
            continue;
         if (c == '\n') {
            current = 0;
            continue;
         }
         if (!c.isSpace()) {
            last_non_space = i;
            if (current < lowest)
               lowest = current;
            continue;
         }
         ++current;
      }
      if (out_lns)
         *out_lns = last_non_space;
      return lowest;
   }
   extern QString minimize_indent(const QString& text) {
      QString out;
      if (text.isEmpty())
         return out;
      uint last_non_space;
      uint lowest  = get_minimum_indent(text, &last_non_space);
      uint current = 0;
      bool is_line_start = true;
      for (uint i = 0; i <= last_non_space; ++i) {
         QChar c = text[i];
         if (c == '\r' || c == '\n' || !c.isSpace() || !is_line_start) {
            out += c;
            current = 0;
            if (!c.isSpace())
               is_line_start = false;
            else if (c == '\r' || c == '\n')
               is_line_start = true;
            continue;
         }
         ++current;
         if (current > lowest)
            out += c;
      }
      return out;
   }
}