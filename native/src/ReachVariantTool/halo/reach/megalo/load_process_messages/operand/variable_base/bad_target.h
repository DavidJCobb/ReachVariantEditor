#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

namespace halo::reach::load_process_messages {
   namespace megalo::operands::variable_base {
      class bad_target;
      class bad_target : public load_process_message_data<bad_target, load_process_message_data_base::message_type::fatal> {
         public:
            virtual QString to_string() const override;

            struct message_content {
               size_t target_id;
            };
            message_content info;
      };
   }
}