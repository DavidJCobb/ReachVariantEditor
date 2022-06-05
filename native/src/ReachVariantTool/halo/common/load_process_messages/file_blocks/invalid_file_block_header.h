#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

#include "halo/util/four_cc.h"

namespace halo::common::load_process_messages {
   class invalid_file_block_header;
   class invalid_file_block_header : public load_process_message_data<invalid_file_block_header, load_process_message_data_base::message_type::error> {
      public:
         virtual QString to_string() const override;

         struct message_content {
            struct {
               util::four_cc signature;
               size_t size = 0;
            } expected;
            struct {
               util::four_cc signature;
               size_t size;
            } found;
         };
         message_content info;
   };
}