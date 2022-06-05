#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

namespace halo::reach::load_process_messages {
   namespace megalo {
      class game_option_value_out_of_bounds;
      class game_option_value_out_of_bounds : public load_process_message_data<game_option_value_out_of_bounds, load_process_message_data_base::message_type::error> {
         public:
            virtual QString to_string() const override;

            struct message_content {
               int16_t value;
            };
            message_content info;
      };
   }
}