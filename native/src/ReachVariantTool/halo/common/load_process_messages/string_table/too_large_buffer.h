#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

namespace halo::common::load_process_messages {
   class string_table_too_large_buffer;
   class string_table_too_large_buffer : public load_process_message_data<string_table_too_large_buffer, load_process_message_data_base::message_type::error> {
      public:
         virtual QString to_string() const override;

         struct message_content {
            size_t max_buffer_size   = 0;
            size_t uncompressed_size = 0;
         };
         message_content info;
   };
}