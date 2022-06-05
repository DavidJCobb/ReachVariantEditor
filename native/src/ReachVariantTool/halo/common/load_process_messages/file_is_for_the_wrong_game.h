#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

#include "halo/game.h"

namespace halo::common::load_process_messages {
   class file_is_for_the_wrong_game;
   class file_is_for_the_wrong_game : public load_process_message_data<file_is_for_the_wrong_game, load_process_message_data_base::message_type::fatal> {
      public:
         virtual QString to_string() const override;

         struct message_content {
            game expected;
            game found;
         };
         message_content info;
   };
}