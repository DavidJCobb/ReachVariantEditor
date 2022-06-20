#include "format_string.h"
#include "halo/reach/bitstreams.h"

#include "variables/number.h"
#include "variables/object.h"
#include "variables/player.h"
#include "variables/team.h"
#include "variables/timer.h"

#include "../create_operand.h"

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
            this->variable = new variables::number;
            break;
         case token_type::object:
            this->variable = new variables::object;
            break;
         case token_type::player:
            this->variable = new variables::player;
            break;
         case token_type::team:
            this->variable = new variables::team;
            break;
         case token_type::timer:
            this->variable = new variables::timer;
            break;
         default:
            stream.throw_fatal_at<halo::reach::load_process_messages::megalo::operands::format_string::bad_token_type>(
               {
                  .type_value = (size_t)type,
               },
               stream.get_position().rewound_by_bits(decltype(type)::max_bitcount)
            );
            return;
      }
      if (this->variable)
         this->variable->read(stream);
   }
   void format_string::token::write(bitwriter& stream) const {
      token_type_bitnumber type;
      if (this->variable == nullptr) {
         type = token_type::none;
      } else {
         switch (this->variable->get_type()) {
            case variable_type::number:
               type = token_type::number;
               break;
            case variable_type::object:
               type = token_type::object;
               break;
            case variable_type::player:
               type = token_type::player;
               break;
            case variable_type::team:
               type = token_type::team;
               break;
            case variable_type::timer:
               type = token_type::timer;
               break;
         }
      }
      stream.write(type);
      if (this->variable) {
         this->variable->write(stream);
      }
   }
   void format_string::token::copy_from(const token& other) {
      this->variable = nullptr;
      if (other.variable) {
         this->variable = (variables::unknown_type*)clone_operand(*other.variable.get());
      }
   }

   void format_string::read(bitreader& stream) {
      stream.read(
         string,
         token_count
      );
      if (this->token_count > max_token_count) {
         stream.throw_fatal_at<halo::reach::load_process_messages::megalo::operands::format_string::token_count_too_large>(
            {
               .count     = (size_t)this->token_count,
               .max_count = max_token_count,
            },
            stream.get_position().rewound_by_bits(decltype(token_count)::max_bitcount)
         );
         return;
      }
      for (size_t i = 0; i < this->token_count; ++i)
         stream.read(this->tokens[i]);
   }
   void format_string::write(bitwriter& stream) const {
      stream.write(
         string,
         token_count
      );
      assert(this->token_count <= max_token_count);
      for (size_t i = 0; i < this->token_count; ++i)
         stream.write(this->tokens[i]);
   }

   format_string& format_string::operator=(const format_string& o) {
      this->string      = o.string;
      this->token_count = o.token_count;
      //
      size_t i = 0;
      for (; i < o.token_count; ++i)
         this->tokens[i].copy_from(o.tokens[i]);
      for (; i < max_token_count; ++i)
         this->tokens[i].variable = nullptr;
      //
      return *this;
   }
}