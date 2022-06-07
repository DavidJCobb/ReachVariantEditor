#include "format_string.h"
#include "halo/reach/bitstreams.h"

#include "variables/number.h"
#include "variables/object.h"
#include "variables/player.h"
#include "variables/team.h"
#include "variables/timer.h"

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
               static_assert(false, "TODO: emit fatal error");
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
            static_assert(false, "TODO: emit fatal error");
         }
         return;
      }
      for (size_t i = 0; i < this->token_count; ++i)
         stream.read(this->tokens[i]);
   }
}