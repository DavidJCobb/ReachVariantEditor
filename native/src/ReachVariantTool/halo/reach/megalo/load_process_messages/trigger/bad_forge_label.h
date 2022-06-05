#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

namespace halo::reach::load_process_messages {
   namespace megalo {
      class trigger_bad_forge_label;
      class trigger_bad_forge_label : public load_process_message_data<trigger_bad_forge_label, load_process_message_data_base::message_type::error> {
         public:
            virtual QString to_string() const override;

            struct message_content {
               int16_t label_index;
            };
            message_content info;
      };
   }
}