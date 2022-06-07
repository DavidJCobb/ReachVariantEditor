#include "format_string.h"
#include "halo/reach/bitstreams.h"

#include "variables/number.h"
#include "variables/object.h"
#include "variables/player.h"
#include "variables/team.h"
#include "variables/timer.h"

#include "halo/reach/megalo/load_process_messages/operand/format_string/bad_token_type.h"
#include "halo/reach/megalo/load_process_messages/operand/format_string/token_count_too_large.h"

namespace halo::reach::megalo::operands {
   void format_string::token::read(bitreader& stream) {
      token_type_bitnumber type;
      stream.read(type);
      switch (type) {
         case token_type::none:
            break;
         case token_type::number:
            this->variable.reset(new variables::number);
            break;
         case token_type::object:
            this->variable.reset(new variables::object);
            break;
         case token_type::player:
            this->variable.reset(new variables::player);
            break;
         case token_type::team:
            this->variable.reset(new variables::team);
            break;
         case token_type::timer:
            this->variable.reset(new variables::timer);
            break;
         default:
            if constexpr (bitreader::has_load_process) {
               stream.load_process().throw_fatal<halo::reach::load_process_messages::megalo::operands::format_string::bad_token_type>({
                  .type_value = (size_t)type,
               });
            }
            return;
      }
      if (this->variable.get())
         this->variable->read(stream);
   }

   void format_string::read(bitreader& stream) {
      stream.read(
         string,
         token_count
      );
      if (this->token_count > max_token_count) {
         if constexpr (bitreader::has_load_process) {
            stream.load_process().throw_fatal<halo::reach::load_process_messages::megalo::operands::format_string::token_count_too_large>({
               .count     = (size_t)this->token_count,
               .max_count = max_token_count,
            });
         }
         return;
      }
      for (size_t i = 0; i < this->token_count; ++i)
         stream.read(this->tokens[i]);
   }
}