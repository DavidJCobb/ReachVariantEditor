#pragma once
#include "player.h"
#include "scalar.h"
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
         static OpcodeArgValue* factory(cobb::ibitreader& stream) {
            return new OpcodeArgValuePlayerSet;
         }
         //
      public:
         cobb::bitnumber<3, PlayerSetType> set_type = PlayerSetType::no_one;
         OpcodeArgValuePlayer player;
         OpcodeArgValueScalar addOrRemove;
         //
         virtual bool read(cobb::ibitreader& stream) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
   };
}