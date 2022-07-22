#pragma once

namespace halo::reach::megalo::AST {
   enum class token_type {
      none,
      eof,

      pragma,

      // single character
      paren_l,
      paren_r,
      square_bracket_l,
      square_bracket_r,

      // operators
      operator_compare_eq, // "=="
      operator_compare_ne, // "!="
      operator_compare_l,  // '<'
      operator_compare_le, // "<="
      operator_compare_g,  // '>'
      operator_compare_ge, // ">="

      operator_binary_add, // '+'
      operator_binary_div, // '/'
      operator_binary_mod, // '%'
      operator_binary_mul, // '*'
      operator_binary_shl, // "<<"
      operator_binary_shr, // ">>"
      operator_binary_sub, // '-'

      operator_unary_negate, // '-'
      operator_unary_not,    // '!'
      operator_unary_plus,   // '+'

      // literals
      identifier,
      number,
      string,

      // keywords
      keyword_alias,
      keyword_alt,
      keyword_altif,
      keyword_and,
      keyword_declare,
      keyword_do,
      keyword_end,
      keyword_enum,
      keyword_for,
      keyword_function,
      keyword_if,
      keyword_on,
      keyword_or,
   };
}