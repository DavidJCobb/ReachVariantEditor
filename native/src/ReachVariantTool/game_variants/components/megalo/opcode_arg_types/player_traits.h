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
         cobb::refcount_ptr<ReachMegaloPlayerTraits> value;
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
   };
}
