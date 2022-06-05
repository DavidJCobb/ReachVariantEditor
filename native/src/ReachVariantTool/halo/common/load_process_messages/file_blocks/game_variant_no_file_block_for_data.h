#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

#include "halo/util/four_cc.h"

namespace halo::common::load_process_messages {
   class game_variant_no_file_block_for_data;
   class game_variant_no_file_block_for_data : public load_process_message_data<game_variant_no_file_block_for_data, load_process_message_data_base::message_type::fatal> {
      public:
         virtual QString to_string() const override;

         struct message_content {
         };
         message_content info;
   };
}