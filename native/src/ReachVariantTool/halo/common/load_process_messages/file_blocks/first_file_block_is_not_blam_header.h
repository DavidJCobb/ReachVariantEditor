#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

#include "halo/util/four_cc.h"

namespace halo::common::load_process_messages {
   class first_file_block_is_not_blam_header;
   class first_file_block_is_not_blam_header : public load_process_message_data<first_file_block_is_not_blam_header, load_process_message_data_base::message_type::fatal> {
      public:
         virtual QString to_string() const override;

         struct message_content {
            util::four_cc signature;
            size_t size = 0;
         };
         message_content info;
   };
}