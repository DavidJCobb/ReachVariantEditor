#pragma once
#include "../opcode_arg.h"
#include "../variables_and_scopes.h"

namespace Megalo {
   class OpcodeArgValuePlayer : public OpcodeArgValue {
      public:
         static constexpr uint16_t none = -1;
      public:
         uint16_t scope = none; // what kind of scalar this is (i.e. constant, variable, game state value)
         uint16_t which = none; // which scope (i.e. if it's a player variable on a team, then which team)
         uint16_t index = none; // which variable (i.e. if it's a player variable on a team, then which player)
         //
         virtual bool read(cobb::bitstream&) noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         //
         static OpcodeArgValue* factory(cobb::bitstream& stream) {
            return new OpcodeArgValuePlayer();
         }
         
   };
}