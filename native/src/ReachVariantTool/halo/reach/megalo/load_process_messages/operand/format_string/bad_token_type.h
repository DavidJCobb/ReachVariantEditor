#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

namespace halo::reach::load_process_messages {
   namespace megalo::operands::format_string {
      class bad_token_type;
      class bad_token_type : public load_process_message_data<bad_token_type, load_process_message_data_base::message_type::fatal> {
         public:
            virtual QString to_string() const override;

            struct message_content {
               size_t type_value;
            };
            message_content info;
      };
   }
}