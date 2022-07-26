#pragma once
#include <variant>
#include "./_base.h"
#include "./literal_data_identifier_or_word.h"
#include "./literal_data_number.h"
#include "./literal_data_string.h"

namespace halo::reach::megalo::AST {

   // i can't name this "literal" because visual studio's syntax highlighter is very, very dumb. 
   // it always highlights C++/CLI "keywords" even in C++/extremely-not-CLI files, and "literal" 
   // is one of microsoft's fake keywords, basically. using it as an identifier confuses the 
   // syntax highlighter, and i find that too annoying to be willing to deal with it.

   class literal_item : public item_base {
      public:
         using variant_type = std::variant<
            std::monostate,
            literal_data_identifier_or_word,
            literal_data_number,
            literal_data_string//,
         >;

      public:
         variant_type value;
   };
}