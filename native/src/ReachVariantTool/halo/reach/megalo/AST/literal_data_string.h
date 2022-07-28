#pragma once
#include <cstdint>
#include <optional>
#include <QString>
#include "./literal_data_identifier_or_word.h"
#include "./literal_data_number.h"

namespace halo::reach::megalo::AST {
   class literal_data_string {
      public:
         QString content;
         QChar   delimiter = '\0';
         std::variant<
            std::monostate,
            literal_data_identifier_or_word,
            literal_data_number//,
         > index; // for literals like "My String"#0 or "My String"#some_alias
   };
}