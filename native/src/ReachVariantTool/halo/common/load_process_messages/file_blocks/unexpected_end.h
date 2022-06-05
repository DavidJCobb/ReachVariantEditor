#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

#include "halo/util/four_cc.h"

namespace halo::common::load_process_messages {
   class file_block_unexpected_end;
   class file_block_unexpected_end : public load_process_message_data<file_block_unexpected_end, load_process_message_data_base::message_type::error> {
      public:
         virtual QString to_string() const override;

         struct message_content {
            struct {
               util::four_cc signature;
               size_t size = 0;
            } block;
            size_t overshoot; // in bytes
         };
         message_content info;
   };
}