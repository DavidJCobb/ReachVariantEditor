#include "opcode_arg.h"
#include "variables_and_scopes.h"

#include "opcode_arg_types/scalar.h"
#include "opcode_arg_types/player.h"
#include "opcode_arg_types/object.h"
#include "opcode_arg_types/team.h"
#include "opcode_arg_types/timer.h"

namespace Megalo {
   extern OpcodeArgValue* OpcodeArgAnyVariableFactory(cobb::bitstream& stream) {
      uint8_t type = stream.read_bits<uint8_t>(3);
      switch ((variable_type)type) {
         case variable_type::scalar:
            return OpcodeArgValueScalar::factory(stream);
         case variable_type::timer:
            return OpcodeArgValueTimer::factory(stream);
         case variable_type::team:
            return OpcodeArgValueTeam::factory(stream);
         case variable_type::player:
            return OpcodeArgValuePlayer::factory(stream);
         case variable_type::object:
            return OpcodeArgValueObject::factory(stream);
      }
      assert(false && "Any-value had an invalid type.");
   }
}