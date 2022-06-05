#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"
#include "halo/reach/megalo/opcode.h"

namespace halo::reach::load_process_messages {
   namespace megalo {
      class trigger_bad_opcode_slice;
      class trigger_bad_opcode_slice : public load_process_message_data<trigger_bad_opcode_slice, load_process_message_data_base::message_type::error> {
         public:
            virtual QString to_string() const override;

            using opcode_type = halo::reach::megalo::opcode_type;

            struct message_content {
               opcode_type opcode_type = opcode_type::unknown;
               size_t start;
               size_t end;
               size_t count_of_all;
            };
            message_content info;
      };
   }
}