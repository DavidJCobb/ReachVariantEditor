#include "opcode_arg.h"
#include "variables_and_scopes.h"

#include "opcode_arg_types/scalar.h"
#include "opcode_arg_types/player.h"
#include "opcode_arg_types/object.h"
#include "opcode_arg_types/team.h"
#include "opcode_arg_types/timer.h"

#include "../../errors.h"

namespace Megalo {
   extern OpcodeArgValue* OpcodeArgAnyVariableFactory(cobb::ibitreader& stream) {
      uint8_t type = stream.read_bits<uint8_t>(3);
      switch ((variable_type)type) {
         case variable_type::scalar:
            return OpcodeArgValueScalar::typeinfo.factory(stream);
         case variable_type::timer:
            return OpcodeArgValueTimer::factory(stream);
         case variable_type::team:
            return OpcodeArgValueTeam::factory(stream);
         case variable_type::player:
            return OpcodeArgValuePlayer::typeinfo.factory(stream);
         case variable_type::object:
            return OpcodeArgValueObject::typeinfo.factory(stream);
      }
      auto& error = GameEngineVariantLoadError::get();
      error.state  = GameEngineVariantLoadError::load_state::failure;
      error.reason = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
      error.detail = GameEngineVariantLoadError::load_failure_detail::bad_opcode_variable_type;
      error.extra[0] = type;
      return nullptr;
   }
   extern OpcodeArgValue* OpcodeArgTeamOrPlayerVariableFactory(cobb::ibitreader& stream) {
      uint8_t type = stream.read_bits<uint8_t>(2);
      switch (type) {
         case 0: // Team
            return OpcodeArgValueTeam::factory(stream);
         case 1: // Player
            return OpcodeArgValuePlayer::typeinfo.factory(stream);
         case 2: // All Players
            return OpcodeArgValueAllPlayers::factory(stream);
      }
      auto& error = GameEngineVariantLoadError::get();
      error.state  = GameEngineVariantLoadError::load_state::failure;
      error.reason = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
      error.detail = GameEngineVariantLoadError::load_failure_detail::bad_opcode_player_or_team_var_type;
      error.extra[0] = type;
      return nullptr;
   }
}