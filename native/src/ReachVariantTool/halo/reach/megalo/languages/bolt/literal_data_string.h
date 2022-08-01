#pragma once
#include <cstdint>
#include <optional>
#include <QString>
#include "./literal_data_identifier_or_word.h"
#include "./literal_data_number.h"

namespace halo::reach::megalo::bolt {
   class literal_data_string {
      public:
         QString content;
         QChar   delimiter = '\0';
   };
}