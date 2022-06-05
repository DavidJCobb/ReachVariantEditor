#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

namespace halo::reach::load_process_messages {
   namespace megalo {
      class map_permissions_too_many_ids;
      class map_permissions_too_many_ids : public load_process_message_data<map_permissions_too_many_ids, load_process_message_data_base::message_type::error> {
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