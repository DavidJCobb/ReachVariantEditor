#pragma once
#include "player.h"

namespace Megalo {
   enum class PlayerSetType {
      no_one,
      anyone,
      allies,
      enemies,
      specific_player,
      no_one_2,
   };
   class OpcodeArgValuePlayerSet : public OpcodeArgValuePlayer {
      public:
         PlayerSetType set_type = PlayerSetType::no_one;
         bool specific_player_add_or_remove = false;
         //
         virtual bool read(cobb::bitstream& stream) noexcept override {
            this->set_type = (PlayerSetType)stream.read_bits<uint8_t>(3);
            if (this->set_type == PlayerSetType::specific_player) {
               if (OpcodeArgValuePlayer::read(stream)) {
                  stream.read(this->specific_player_add_or_remove);
                  return true;
               }
               return false;
            }
            return true;
         }
         virtual void to_string(std::string& out) const noexcept override {
            if (this->set_type == PlayerSetType::specific_player) {
               OpcodeArgValuePlayer::to_string(out);
               const char* verb = "add";
               if (!this->specific_player_add_or_remove)
                  verb = "remove";
               cobb::sprintf(out, "%s %s", verb, out.c_str());
            }
            switch (this->set_type) {
               case PlayerSetType::no_one:
                  out = "no one";
                  return;
               case PlayerSetType::anyone:
                  out = "anyone";
                  return;
               case PlayerSetType::allies:
                  out = "allies";
                  return;
               case PlayerSetType::enemies:
                  out = "enemies";
                  return;
               case PlayerSetType::no_one_2:
                  out = "no one (?)";
                  return;
            }
            cobb::sprintf(out, "unknown:%u", (uint32_t)this->set_type);
         }
         //
         static OpcodeArgValue* factory(cobb::bitstream& stream) {
            return new OpcodeArgValuePlayerSet();
         }
         
   };

}