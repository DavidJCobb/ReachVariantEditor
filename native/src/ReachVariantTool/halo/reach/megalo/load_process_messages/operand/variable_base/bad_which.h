#pragma once
#include <cstdint>
#include "halo/load_process_message_data.h"

#include "halo/reach/megalo/variable_scope.h"

namespace halo::reach::load_process_messages {
   namespace megalo::operands::variable_base {
      class bad_which;
      class bad_which : public load_process_message_data<bad_which, load_process_message_data_base::message_type::error> {
         public:
            virtual QString to_string() const override;

            struct message_content {
               size_t index;
               size_t maximum;
               halo::reach::megalo::variable_scope type;
            };
            message_content info;
      };
   }
}