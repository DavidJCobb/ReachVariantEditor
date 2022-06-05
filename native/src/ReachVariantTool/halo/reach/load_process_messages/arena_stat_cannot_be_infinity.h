#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

namespace halo::reach::load_process_messages {
   class arena_stat_cannot_be_infinity;
   class arena_stat_cannot_be_infinity : public load_process_message_data<arena_stat_cannot_be_infinity, load_process_message_data_base::message_type::error> {
      public:
         virtual QString to_string() const override;

         struct message_content {
            size_t stat_index;
         };
         message_content info;
   };
}