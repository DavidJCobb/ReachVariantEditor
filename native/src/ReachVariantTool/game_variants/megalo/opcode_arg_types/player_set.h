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
   class OpcodeArgValuePlayerSet : public OpcodeArgValue {
      public:
         PlayerSetType        set_type = PlayerSetType::no_one;
         OpcodeArgValuePlayer player;
         OpcodeArgValueScalar addOrRemove;
         //
         virtual bool read(cobb::bitstream& stream) noexcept override {
            this->set_type = (PlayerSetType)stream.read_bits<uint8_t>(3);
            if (this->set_type == PlayerSetType::specific_player) {
               return this->player.read(stream) && this->addOrRemove.read(stream);
            }
            return true;
         }
         virtual void write(cobb::bitwriter& stream) const noexcept override {
            stream.write((uint8_t)this->set_type, 3);
            if (this->set_type == PlayerSetType::specific_player) {
               this->player.write(stream);
               this->addOrRemove.write(stream);
            }
         }
         virtual void to_string(std::string& out) const noexcept override {
            if (this->set_type == PlayerSetType::specific_player) {
               std::string temp;
               this->player.to_string(out);
               this->addOrRemove.to_string(temp);
               cobb::sprintf(out, "%s - add or remove: %s", out.c_str(), temp.c_str());
               return;
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