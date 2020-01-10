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
         int16_t  index = none; // which variable (i.e. if it's a number variable on a player, then which player); if it's a constant, then this is the constant value
         ref<cobb::reference_tracked_object> target = decltype(target)::make(*this); // if this argument is an option, a stat, etc., this points to that
         //
         virtual bool read(cobb::ibitreader&) noexcept override;
         virtual void postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         //
         static OpcodeArgValue* factory(cobb::ibitreader& stream) {
            return new OpcodeArgValueScalar();
         }
         virtual variable_type get_variable_type() const noexcept {
            return variable_type::scalar;
         }
   };
}