#include "color.h"
#include <array>
#include <cmath>
#include "string_scanner.h"

namespace cobb::qt {
   extern QColor parse_css_color(const QString& text, css_color_parse_error& error) {
      return parse_css_color(QStringRef(&text), error);
   }
   extern QColor parse_css_color(QStringRef view, css_color_parse_error& error) {
      error = css_color_parse_error::none;
      view  = view.trimmed();
      //
      auto values = std::array<int, 4>{ 0, 0, 0, 255 };
      if (view.startsWith("#")) {
         int len = view.size() - 1;
         if (view.indexOf(" ") > 0) {
            error = css_color_parse_error::unexpected_trailing_content;
            return QColor();
         }
         //
         int digits;
         switch (len) {
            case 3: // short RGB
            case 4: // short RGBA
               digits = 1;
               break;
            case 6: // long  RGB
            case 8: // long  RGBA
               digits = 2;
               break;
            default:
               error = css_color_parse_error::hex_color_wrong_digit_count;
               return QColor();
         }
         bool isnum;
         for (int i = 0; i < 4; ++i) {
            QString octet;
            {
               int j = (i * digits) + 1;
               if (j + digits > view.size())
                  break;
               octet = view.mid(j, digits).toString();
               if (digits == 1)
                  octet += octet[0];
            }
            values[i] = octet.toInt(&isnum, 16);
            if (!isnum) {
               error = css_color_parse_error::hex_color_bad_digit;
               return QColor();
            }
         }
         return QColor(values[0], values[1], values[2], values[3]);
      }
      //
      // Try CSS function syntax:
      //
      bool is_rgb = view.startsWith("rgb", Qt::CaseInsensitive);
      bool is_hsl = view.startsWith("hsl", Qt::CaseInsensitive);
      if (is_rgb || is_hsl) {
         bool alpha   = view[4] == 'a' || view[4] == 'A';
         bool comma   = false;
         bool percent = false;
         //
         // I wish I could use a loop to pull the color components. The problem is that for RGB(A) colors, 
         // each component is similar enough in syntax to tempt you into using a loop, but just different 
         // enough that you can't:
         // 
         //  - R differs from G and B in that it is the deciding factor in whether the components should 
         //    use percentage values or not.
         // 
         //  - B differs from R and G in that it might not be followed by a separator (e.g. ','); depending 
         //    on whether this is an RGB or RGBA color.
         // 
         //  - A differs from R, G, and B both in its range, [0, 1], and in its ability to use or not use 
         //    a percentage independently of R, G, and B.
         // 
         //  - Additionally, since we're using the same parsing for HSL, "R" doubles for "H" and so differs 
         //    in how it handles units of angle measurement.
         //
         cobb::qt::string_scanner scanner(view);
         scanner.skip(3); // "rgb" or "hsl"
         if (alpha)
            scanner.skip(1);
         if (!scanner.extract_specific_char('(')) {
            error = css_color_parse_error::function_color_expected_open_paren;
            return QColor();
         }
         //
         double v;
         if (!scanner.extract_double(v)) {
            error = css_color_parse_error::function_color_expected_number_1;
            return QColor();
         }
         if (is_hsl) {
            if (scanner.extract_specific_substring("deg", true)) {
               ; // degrees are the default for HSL "H" values
            } else if (scanner.extract_specific_substring("rad", true)) {
               v /= 57.295779513082320876798154814105; // 180 / PI
            } else if (scanner.extract_specific_substring("grad", true)) {
               v *= 0.9;
            } else if (scanner.extract_specific_substring("turn", true)) {
               v *= 360.0;
            }
         } else {
            if (scanner.extract_specific_char('%', true)) {
               percent = true;
               v *= 2.55;
            }
         }
         values[0] = std::round(v);
         //
         if (scanner.extract_specific_char(','))
            comma = true;
         //
         if (!scanner.extract_double(v)) {
            error = css_color_parse_error::function_color_expected_number_2;
            return QColor();
         }
         if (percent || is_hsl) {
            if (!scanner.extract_specific_char('%', true)) {
               if (is_hsl)
                  error = css_color_parse_error::hsl_must_use_percentages;
               else
                  error = css_color_parse_error::rgb_dont_mix_and_match_percentages;
               return QColor();
            }
            v *= 2.55;
         } else {
            if (scanner.extract_specific_char('%', true)) {
               error = css_color_parse_error::rgb_dont_mix_and_match_percentages;
               return QColor();
            }
         }
         values[1] = std::round(v);
         if (comma)
            if (!scanner.extract_specific_char(',')) {
               error = css_color_parse_error::function_color_expected_comma;
               return QColor();
            }
         //
         if (!scanner.extract_double(v)) {
            error = css_color_parse_error::function_color_expected_number_3;
            return QColor();
         }
         if (percent || is_hsl) {
            if (!scanner.extract_specific_char('%', true)) {
               if (is_hsl)
                  error = css_color_parse_error::hsl_must_use_percentages;
               else
                  error = css_color_parse_error::rgb_dont_mix_and_match_percentages;
               return QColor();
            }
            v *= 2.55;
         } else {
            if (scanner.extract_specific_char('%', true)) {
               error = css_color_parse_error::rgb_dont_mix_and_match_percentages;
               return QColor();
            }
         }
         values[2] = std::round(v);
         //
         if (alpha) {
            QChar desired = comma ? ',' : '/';
            if (!scanner.extract_specific_char(desired)) {
               if (comma)
                  error = css_color_parse_error::function_color_expected_comma;
               else
                  error = css_color_parse_error::function_color_expected_slash;
               return QColor();
            }
            //
            if (!scanner.extract_double(v)) {
               error = css_color_parse_error::function_color_expected_number_4;
               return QColor();
            }
            if (scanner.extract_specific_char('%', true)) // alpha can be a percentage even if the other values are not
               v *= 2.55;
            else
               v *= 255.0;
            values[3] = std::round(v);
         }
         //
         if (!scanner.extract_specific_char(')')) {
            error = css_color_parse_error::function_color_expected_close_paren;
            return QColor();
         }
         //
         if (!scanner.is_at_effective_end()) {
            error = css_color_parse_error::unexpected_trailing_content;
            return QColor();
         }
         //
         QColor out;
         if (is_hsl) {
            values[0] %= 360;
            for (int i = 1; i < 4; ++i)
               values[i] = std::clamp(values[i], 0, 255);
            out.setHsl(values[0], values[1], values[2], values[3]);
            return out;
         }
         for (int i = 0; i < 4; ++i)
            values[i] = std::clamp(values[i], 0, 255);
         out.setRgb(values[0], values[1], values[2], values[3]);
         return out;
      }
      //
      // Try named color:
      //
      QColor out;
      out.setNamedColor(view);
      if (!out.isValid())
         error = css_color_parse_error::unrecognized_color_name;
      return out;
   }

   extern QString stringify_css_color(const QColor& color, css_color_format format) {
      QString out;
      bool has_alpha = color.alpha() != 255;
      switch (format) {
         case css_color_format::hex:
            return QString("#%1%2%3%4")
               .arg(QString("%1").arg(color.red(), 2, 16, QChar('0')).toUpper())
               .arg(QString("%1").arg(color.green(), 2, 16, QChar('0')).toUpper())
               .arg(QString("%1").arg(color.blue(), 2, 16, QChar('0')).toUpper())
               .arg(has_alpha ? QString("%1").arg(color.alpha(), 2, 16, QChar('0')).toUpper() : "");
         case css_color_format::rgb:
            return QString("rgb(%1, %2, %3)")
               .arg(color.red())
               .arg(color.green())
               .arg(color.blue());
         case css_color_format::rgba:
            return QString("rgb(%1, %2, %3, %4)")
               .arg(color.red())
               .arg(color.green())
               .arg(color.blue())
               .arg(color.alphaF());
         case css_color_format::hsl:
            return QString("rgb(%1deg, %2%%, %3%%)")
               .arg(color.hslHue())
               .arg(std::round(color.hslSaturationF() / 2.55))
               .arg(std::round(color.lightness() / 2.55));
         case css_color_format::hsla:
            return QString("rgb(%1deg, %2%%, %3%%, %4)")
               .arg(color.hslHue())
               .arg(std::round(color.hslSaturationF() / 2.55))
               .arg(std::round(color.lightness() / 2.55))
               .arg(color.alphaF());
      }
      return out;
   }
}