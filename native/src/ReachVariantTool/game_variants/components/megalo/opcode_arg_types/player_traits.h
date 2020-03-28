#pragma once
#include "../opcode_arg.h"
#include "../limits.h"
#include "../../player_traits.h"
#include "../../../types/multiplayer.h"

namespace Megalo {
   class OpcodeArgValuePlayerTraits : public OpcodeArgValue {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         static constexpr int16_t max_index = Limits::max_script_traits;
         //
         int16_t index = 0;
         cobb::refcount_ptr<ReachMegaloPlayerTraits> value;
         bool postprocessed = false;
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
   };
}
