#pragma once
#include <QString>

namespace halo::reach::megalo::AST {
   class literal_data_identifier_or_word {
      public:
         literal_data_identifier_or_word() {}

      public:
         QString content;
   };
}