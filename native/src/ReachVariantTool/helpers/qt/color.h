#pragma once
#include <QColor>
#include <QString>

namespace cobb::qt {
   enum class css_color_parse_error {
      none,
      hex_color_wrong_digit_count,
      hex_color_bad_digit,
      function_color_expected_open_paren,
      function_color_expected_comma,
      function_color_expected_slash,
      function_color_expected_close_paren,
      function_color_expected_number_1, // R or H
      function_color_expected_number_2, // G or S
      function_color_expected_number_3, // B or L
      function_color_expected_number_4, // alpha
      hsl_must_use_percentages,
      rgb_dont_mix_and_match_percentages,
      unexpected_trailing_content,
      unrecognized_color_name,
   };
   extern QColor parse_css_color(const QString&, css_color_parse_error&);
   extern QColor parse_css_color(QStringRef, css_color_parse_error&);

   enum class css_color_format {
      hex,
      rgb,
      rgba,
      hsl,
      hsla,
   };
   extern QString stringify_css_color(const QColor&, css_color_format);
}