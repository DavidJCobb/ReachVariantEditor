#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

namespace halo::reach::load_process_messages {
   namespace megalo::operands::player_traits {
      class index_out_of_max_bounds;
      class index_out_of_max_bounds : public load_process_message_data<index_out_of_max_bounds, load_process_message_data_base::message_type::error> {
         public:
            virtual QString to_string() const override;

            struct message_content {
               size_t maximum; // == highest valid index in a game variant with the max trigger count (i.e. == max trigger count - 1)
               size_t value;
            };
            message_content info;
      };
   }
}