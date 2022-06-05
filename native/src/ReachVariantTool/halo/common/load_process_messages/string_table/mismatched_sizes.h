#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

namespace halo::common::load_process_messages {
   class string_table_mismatched_sizes;
   class string_table_mismatched_sizes : public load_process_message_data<string_table_mismatched_sizes, load_process_message_data_base::message_type::warning> {
      public:
         virtual QString to_string() const override;

         struct message_content {
            size_t bungie_size = 0;
            size_t zlib_size   = 0;
         };
         message_content info;
   };
}