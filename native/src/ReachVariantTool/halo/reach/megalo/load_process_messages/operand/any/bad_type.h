#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"
#include "halo/reach/megalo/opcode.h"

namespace halo::reach::load_process_messages {
   namespace megalo::operands::any {
      class bad_type;
      class bad_type : public load_process_message_data<bad_type, load_process_message_data_base::message_type::error> {
         public:
            virtual QString to_string() const override;

            using opcode_type = halo::reach::megalo::opcode_type;

            struct message_content {
               size_t type_value;
            };
            message_content info;
      };
   }
}