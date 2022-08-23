#pragma once

namespace halo::reach::megalo::bolt {

   //
   // Code interpretation works in two passes: scanning and parsing. The 
   // scanner identifies the most basic possible tokens, and the parser 
   // then consumes those tokens to identify situational constructs and 
   // generate an AST.
   // 
   // For example, the character sequence "+=" is always identifiable as 
   // the add-assign operator; however, a lone "+" is only situationally 
   // identifiable (as a unary prefix operator or an infix operator used 
   // between two terms). As such, we have a token for "add-assign," but 
   // we don't have a token for "addition" or "unary positive;" instead 
   // there is a "plus" token.
   //
   enum class token_type {
      none,
      eof,

      // Single characters.
      paren_l,
      paren_r,
      angle_bracket_l,
      angle_bracket_r,
      square_bracket_l,
      square_bracket_r,
      ampersand,   // '&'
      asterisk,    // '*'
      backslash,   // 
      caret,       // '^'
      colon,       // ':'
      comma,       // ','
      equal,       // '='
      exclamation, // '!'
      hash,        // '#'
      minus,       // '-'
      null_char,   // character code 0
      percent,     // '%'
      period,      // '.'
      pipe,        // '|'
      plus,        // '+'
      semicolon,   // ';'
      slash_fwd,   // '/'
      tilde,       // '~'

      // "Semantic" tokens are below. Values should only be defined for 
      // character sequences that are uniquely recognizable without any 
      // intelligent parsing. By definition, that means multi-character 
      // tokens only.

      // operators
      operator_compare_eq, // "=="
      operator_compare_ne, // "!="
      operator_compare_le, // "<="
      operator_compare_ge, // ">="

      operator_assign_add,  // "+="
      operator_assign_and,  // "&="
      operator_assign_div,  // "/="
      operator_assign_mod,  // "%="
      operator_assign_mul,  // "*="
      operator_assign_not,  // "~="
      operator_assign_or,   // "|="
      operator_assign_shl,  // "<<=" // Megalo has no bitshifts, but we can just multiply by a hardcoded power of 2
      operator_assign_shr,  // ">>=" // Megalo has no bitshifts, but we can do a division by a hardcoded power of 2
      operator_assign_sub,  // "-="
      operator_assign_xor,  // "^="

      operator_binary_shl, // "<<" // Megalo has no bitshifts, but we can just multiply by a hardcoded power of 2
      operator_binary_shr, // ">>" // Megalo has no bitshifts, but we can do a division by a hardcoded power of 2

      // literals
      identifier_or_word, // identifiers e.g. variables, or words e.g. subkeywords like "with" in "declare <variable> with network priority <priority> = <value>"
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
      keyword_not,
      keyword_on,
      keyword_or,
      keyword_pragma,
      keyword_then,
   };
}