#pragma once
#include "../opcode_arg.h"
#include "../variables_and_scopes.h"

namespace Megalo {
   class OpcodeArgValueTeam : public OpcodeArgValue {
      public:
         static constexpr uint16_t none = -1;
      public:
         uint16_t scope = none; // what kind of team this is (i.e. variable, game state value)
         uint16_t which = none; // which scope (i.e. if it's a team variable on a player, then which team)
         uint16_t index = none; // which variable (i.e. if it's a team variable on a player, then which team)
         //
         virtual bool read(cobb::bitstream&) noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         //
         static OpcodeArgValue* factory(cobb::bitstream& stream) {
            return new OpcodeArgValueTeam();
         }
         
   };
}