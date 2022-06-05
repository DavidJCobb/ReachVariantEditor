#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

#include "halo/localization_language.h"

namespace halo::common::load_process_messages {
   class string_table_entries_not_null_separated;
   class string_table_entries_not_null_separated : public load_process_message_data<string_table_entries_not_null_separated, load_process_message_data_base::message_type::error> {
      public:
         virtual QString to_string() const override;

         struct message_content {
            size_t string_index  = 0;
            size_t string_offset = 0;
            localization_language language = (localization_language)-1;
         };
         message_content info;
   };
}