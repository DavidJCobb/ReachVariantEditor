#pragma once
#include <vector>
#include "halo/load_process_message_data.h"

namespace halo::reach::load_process_messages {
   namespace megalo {
      class referenced_undefined_indexed_data;
      class referenced_undefined_indexed_data : public load_process_message_data<referenced_undefined_indexed_data, load_process_message_data_base::message_type::error> {
         public:
            virtual QString to_string() const override;

            enum class data_type {
               unspecified,
               forge_label,
               game_option,
               game_stat,
               hud_widget,
               player_trait_set,
            };

            struct message_content {
               std::vector<size_t> indices;
               size_t max_count;
               data_type type = data_type::unspecified;
            };
            message_content info;
      };
   }
}