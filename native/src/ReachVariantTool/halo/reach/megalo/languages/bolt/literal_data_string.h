#pragma once
#include <QString>

namespace halo::reach::megalo::bolt {
   class literal_data_string {
      public:
         QString content;
         QChar   delimiter = '\0';
   };
}