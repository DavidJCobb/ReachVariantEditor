#pragma once
#include <cstdint>
#include <optional>
#include <QString>

namespace halo::reach::megalo::AST {
   class literal_data_string {
      public:
         literal_data_string() {}

      public:
         QChar   delimiter = '\0';
         QString content;
         std::optional<int32_t> index; // for literals like "My String"#0 // TODO: what about "My String"#alias?
   };
}