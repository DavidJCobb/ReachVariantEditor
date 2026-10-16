#include "syntax_highlight_option.h"
#include "../helpers/qt/color.h"

namespace {
   static QStringView _extractFontRule(const QStringView text, bool& error) {
      int size  = text.size();
      int i;
      int paren = 0;
      for (i = 0; i < size; ++i) {
         if (i == 0 && text[i] == '#') // allow hex color literals
            continue;
         if (text[i] == '(') {
            ++paren;
            continue;
         }
         if (text[i] == ')') {
            --paren;
            if (paren < 0) { // more closing parentheses than opening ones
               error = true;
               return {};
            }
            continue;
         }
         if (!paren && !text[i].isLetter())
            break;
      }
      if (paren) { // unterminated parenthetical
         error = true;
         return {};
      }
      return text.left(i);
   }
}

namespace ReachINI {
   /*static*/ syntax_highlight_option syntax_highlight_option::fromString(const QString& text, bool& error) {
      syntax_highlight_option out;
      error = false;
      //
      int size = text.size();
      for (int i = 0; i < size; ++i) {
         auto  forward = QStringView(text).mid(i);
         QChar c = text[i];
         if (c.isSpace())
            continue;
         if (c.isLetter()) {
            auto rule = _extractFontRule(forward, error);
            if (error)
               return out;
            i += rule.size() - 1; // subtract 1 because our loop will increment it
            if (rule.compare(QLatin1StringView("bold"), Qt::CaseInsensitive) == 0) {
               out.bold = true;
               continue;
            }
            if (rule.compare(QLatin1StringView("italic"), Qt::CaseInsensitive) == 0) {
               out.italic = true;
               continue;
            }
            if (rule.compare(QLatin1StringView("underline"), Qt::CaseInsensitive) == 0) {
               out.underline = true;
               continue;
            }
            if (rule.startsWith('#') || rule.startsWith(QLatin1StringView("rgb"), Qt::CaseInsensitive) || rule.startsWith(QLatin1StringView("hsl"), Qt::CaseInsensitive)) {
               cobb::qt::css_color_parse_error pe;
               auto color = cobb::qt::parse_css_color(rule, pe);
               if (pe != cobb::qt::css_color_parse_error::none) {
                  error = true;
                  return out;
               }
               out.colors.text = color;
               continue;
            }
            continue;
         }
         //
         // Unrecognized symbol:
         //
         error = true;
         break;
      }
      return out;
   }
   /*static*/ syntax_highlight_option syntax_highlight_option::fromString(const QString& text) {
      bool dummy;
      auto r = fromString(text, dummy);
      if (dummy)
         return syntax_highlight_option();
      return r;
   }

   QTextCharFormat syntax_highlight_option::toFormat() const noexcept {
      QTextCharFormat out;
      out.setFontWeight(this->bold ? QFont::Weight::Bold : QFont::Weight::Normal);
      out.setFontItalic(this->italic);
      out.setFontUnderline(this->underline);
      out.setForeground(this->colors.text);
      return out;
   }

   extern QTextCharFormat parse_syntax_highlight_option(const QString& text, bool& error) {
      auto data = syntax_highlight_option::fromString(text, error);
      if (!error)
         return data.toFormat();
      return QTextCharFormat();
   }
   extern QTextCharFormat parse_syntax_highlight_option(const std::string& text, bool& error) {
      return parse_syntax_highlight_option(QString::fromUtf8(text.c_str()), error);
   }

   extern QString stringify_syntax_highlight_option(const syntax_highlight_option& format) {
      QString out;
      {
         const auto& c = format.colors.text;
         out += QString("rgb(%1, %2, %3)")
            .arg(c.red())
            .arg(c.green())
            .arg(c.blue());
      }
      if (format.bold) {
         if (!out.isEmpty())
            out += ' ';
         out += "bold";
      }
      if (format.italic) {
         if (!out.isEmpty())
            out += ' ';
         out += "italic";
      }
      if (format.underline) {
         if (!out.isEmpty())
            out += ' ';
         out += "underline";
      }
      return out;
   }
}