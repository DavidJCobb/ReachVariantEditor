#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

namespace halo::common::load_process_messages {
   class string_table_cannot_allocate_buffer;
   class string_table_cannot_allocate_buffer : public load_process_message_data<string_table_cannot_allocate_buffer, load_process_message_data_base::message_type::error> {
      public:
         virtual QString to_string() const override;

         struct message_content {
            size_t compressed_size   = 0;
            size_t uncompressed_size = 0;
            size_t max_buffer_size   = 0;
            bool   is_compressed     = false;
            //
            bool is_zlib   = false;
            int  zlib_code = 0;
         };
         message_content info;
   };
}