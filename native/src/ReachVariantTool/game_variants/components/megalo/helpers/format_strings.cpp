#include "format_strings.h"
#include <QRegularExpression>

namespace Megalo {
   extern QVector<format_string_issue> check_format_string(const QString& text) {
      QVector<format_string_issue> out;
      //
      auto pattern = QRegularExpression(R"==(^%([\+\-\#0]*)(\d*)(\.\d|)([a-zA-Z]))==");
      auto size    = text.size();
      for (auto i = text.indexOf('%'); i >= 0; i = text.indexOf('%', i)) {
         if (i + 1 >= size) {
            out.push_back(format_string_issue{ .index = (size_t)i, .code = "%", .error = true });
            ++i;
            break;
         }
         QChar c = text[i + 1];
         if (c == '%') { // "%%" is legal
            i += 2;
            continue;
         }
         //
         // Validate the format code:
         //
         auto match = pattern.match(text.midRef(i));
         if (!match.hasMatch()) {
            QChar c = text[i + 1];
            if (c != ' ') {
               out.push_back(format_string_issue{ .index = (size_t)i, .code = QString("%") + c, .error = true });
               i += 2;
            } else {
               out.push_back(format_string_issue{ .index = (size_t)i, .code = "%", .error = true });
               ++i;
            }
            continue;
         }
         QString full = match.captured(0);
         QChar   code = match.captured(4)[0];
         switch (code.unicode()) {
            case 'n':
            case 'o':
            case 'p':
            case 's':
            case 't':
               //
               // These display their value in the most straightforward manner possible (player's 
               // Xbox LIVE gamertag; object's name, if any; integer's value in base-10; timer as 
               // M:SS; etc.).
               //
               if (full.size() > 2) { // parameters not allowed for these
                  out.push_back(format_string_issue{ .index = (size_t)i, .code = full, .error = true, .no_params_allowed = true });
                  i += full.size();
                  continue;
               }
               break;
            case 'c':
               //
               // Sometimes, this prevents the string from displaying; other times, it inserts a line 
               // break and the number 25 (i.e. (int)'%').
               //
               out.push_back(format_string_issue{ .index = (size_t)i, .code = full, .unusual_and_buggy = true });
               i += full.size();
               continue;
            case 'd':
            case 'i':
            case 'u':
            case 'x':
            case 'X':
               //
               // These "work," but are broken: they always act as if their parameter is the number 1.
               //
               out.push_back(format_string_issue{ .index = (size_t)i, .code = full, .always_one = true });
               i += full.size();
               continue;
            case 'a':
            case 'A':
            case 'e':
            case 'E':
            case 'f':
            case 'F':
            case 'g':
            case 'G':
               //
               // Double-precision codes.
               //
               out.push_back(format_string_issue{ .index = (size_t)i, .code = full, .is_floating_point = true });
               i += full.size();
               continue;
            case 'S':
            case 'Z':
               //
               // These crash on display.
               //
               out.push_back(format_string_issue{ .index = (size_t)i, .code = full, .crash = true });
               i += full.size();
               continue;
            default:
               out.push_back(format_string_issue{ .index = (size_t)i, .code = full, .error = true });
               i += full.size();
               continue;
         }
         ++i;
      }
      return out;
   }
}