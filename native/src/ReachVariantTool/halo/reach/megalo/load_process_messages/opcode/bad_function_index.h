#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"
#include "halo/reach/megalo/opcode.h"

namespace halo::reach::load_process_messages {
   namespace megalo {
      class opcode_bad_function_index;
      class opcode_bad_function_index : public load_process_message_data<opcode_bad_function_index, load_process_message_data_base::message_type::fatal> {
         public:
            virtual QString to_string() const override;

            using opcode_type = halo::reach::megalo::opcode_type;

            struct message_content {
               opcode_type opcode_type = opcode_type::unknown;
               size_t      function_id;
            };
            message_content info;
      };
   }
}