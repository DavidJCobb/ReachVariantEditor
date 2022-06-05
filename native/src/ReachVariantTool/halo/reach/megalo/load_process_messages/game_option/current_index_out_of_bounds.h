#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

namespace halo::reach::load_process_messages {
   namespace megalo {
      class game_option_current_index_out_of_bounds;
      class game_option_current_index_out_of_bounds : public load_process_message_data<game_option_current_index_out_of_bounds, load_process_message_data_base::message_type::error> {
         public:
            virtual QString to_string() const override;

            struct message_content {
               int16_t index;
               size_t  count;
            };
            message_content info;
      };
   }
}