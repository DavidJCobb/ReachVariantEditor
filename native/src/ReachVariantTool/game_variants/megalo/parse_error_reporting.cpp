#include "parse_error_reporting.h"
#include <stdio.h>
#include <type_traits>
#include "variables_and_scopes.h"

namespace Megalo {
   void ParseState::_reset() noexcept {
      this->signalled = false;
      this->opcode           = opcode_type::none;
      this->opcode_index     = 0;
      this->opcode_arg_index = 0;
      this->cause            = what::none;
      //
      for (size_t i = 0; i < std::extent<decltype(this->extra)>::value; i++)
         this->extra[i] = 0;
   }
   void ParseState::_print() const noexcept {
      if (!this->signalled)
         return;
      if (this->opcode != opcode_type::none) {
         const char* optype = "unknown";
         switch (this->opcode) {
            case opcode_type::condition: optype = "condition"; break;
            case opcode_type::action:    optype = "action"; break;
         }
         printf("Parse error detected while reading %ss!\n", optype);
         printf("Argument #%d in %s #%d failed.\n", this->opcode_arg_index, optype, this->opcode_index);
         switch (this->cause) {
            case what::bad_player_or_team_var_type:
               printf("Bad player-or-team-variable type: %u.\n", this->extra[0]);
               break;
            case what::bad_variable_type:
               printf("Bad variable type: %u.\n", this->extra[0]);
               break;
            case what::bad_variable_subtype:
               {
                  const char* type = "unknown";
                  switch ((variable_type)this->extra[0]) {
                     case variable_type::object: type = "object"; break;
                     case variable_type::player: type = "player"; break;
                     case variable_type::scalar: type = "scalar"; break;
                     case variable_type::team:   type = "team";   break;
                     case variable_type::timer:  type = "timer";  break;
                  }
                  printf("Bad variable subtype: %s subtype #%u.\n", type, this->extra[1]);
               }
               break;
            default:
               printf("Cause not specified.\n");
         }
      }
      #if _DEBUG
         __debugbreak();
      #endif
   }
}