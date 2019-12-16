#pragma once
#include "../opcode_arg.h"
#include "../variables_and_scopes.h"

namespace Megalo {
   class OpcodeArgValueScalar : public OpcodeArgValue {
      public:
         static constexpr uint16_t none = -1;
      public:
         uint16_t scope = none; // what kind of scalar this is (i.e. constant, variable, game state value)
         uint16_t which = none; // which scope (i.e. if it's a number variable on a player, then which player; if it's Spawn Sequence, then on which object)
         uint16_t index = none; // which variable (i.e. if it's a number variable on a player, then which player); if it's a constant, then this is the constant value
         //
         virtual bool read(cobb::bitstream&) noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         //
         static OpcodeArgValue* factory(cobb::bitstream& stream) {
            return new OpcodeArgValueScalar();
         }
         
   };
}