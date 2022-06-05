#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

#include "halo/player_trait.h"

namespace halo::common::load_process_messages {
   struct player_trait_out_of_bounds_message_content {
      player_trait trait = player_trait::unknown;
      int value       = 0;
      int allowed_min = 0;
      int allowed_max = 0;
   };

   class player_trait_out_of_bounds_warning;
   class player_trait_out_of_bounds_warning : public load_process_message_data<player_trait_out_of_bounds_warning, load_process_message_data_base::message_type::warning> {
      public:
         virtual QString to_string() const override;

         using message_content = player_trait_out_of_bounds_message_content;
         message_content info;
   };

   class player_trait_out_of_bounds_error;
   class player_trait_out_of_bounds_error : public load_process_message_data<player_trait_out_of_bounds_error, load_process_message_data_base::message_type::error> {
      public:
         virtual QString to_string() const override;

         using message_content = player_trait_out_of_bounds_message_content;
         message_content info;
   };
}