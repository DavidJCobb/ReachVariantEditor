#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

namespace halo::reach::load_process_messages {
   namespace megalo::operands::format_string {
      class token_count_too_large;
      class token_count_too_large : public load_process_message_data<token_count_too_large, load_process_message_data_base::message_type::fatal> {
         public:
            virtual QString to_string() const override;

            struct message_content {
               size_t count;
               size_t max_count;
            };
            message_content info;
      };
   }
}