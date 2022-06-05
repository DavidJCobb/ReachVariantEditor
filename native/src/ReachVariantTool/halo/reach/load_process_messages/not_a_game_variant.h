#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

#include "halo/reach/ugc_file_type.h"

namespace halo::reach::load_process_messages {
   class not_a_game_variant;
   class not_a_game_variant : public load_process_message_data<not_a_game_variant, load_process_message_data_base::message_type::fatal> {
      public:
         virtual QString to_string() const override;

         struct message_content {
            ugc_file_type type;
         };
         message_content info;
   };
}