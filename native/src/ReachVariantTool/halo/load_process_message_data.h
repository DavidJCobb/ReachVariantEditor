#pragma once
#include <QString>

namespace halo {
   class load_process_message_data {
      public:
         enum class message_type {
            notice,
            warning,
            error,
            fatal,
         };

      public:
         virtual ~load_process_message_data();

         virtual QString to_string() const = 0;
         virtual message_type type() const = 0;
   };
}