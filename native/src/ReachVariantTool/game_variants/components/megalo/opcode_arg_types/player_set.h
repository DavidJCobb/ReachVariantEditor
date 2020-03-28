#pragma once
#include "variables/player.h"
#include "variables/number.h"
#include "../../../../helpers/bitnumber.h"

namespace Megalo {
   enum class PlayerSetType {
      no_one,
      anyone,
      allies,
      enemies,
      specific_player,
      no_one_2,
   };
   class OpcodeArgValuePlayerSet : public OpcodeArgValue {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         cobb::bitnumber<3, PlayerSetType> set_type = PlayerSetType::no_one;
         OpcodeArgValuePlayer player;
         OpcodeArgValueScalar addOrRemove;
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
   };
}