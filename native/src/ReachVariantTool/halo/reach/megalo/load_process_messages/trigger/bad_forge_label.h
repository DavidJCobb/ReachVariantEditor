#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

namespace halo::reach::load_process_messages {
   class trigger_bad_forge_label : public load_process_message_data {
      public:
         virtual QString to_string() const override;
         virtual message_type type() const override { return message_type::error; };

         int16_t label_index;
   };
}