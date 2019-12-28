#pragma once
#include "player.h"
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
         cobb::bitnumber<3, PlayerSetType> set_type = PlayerSetType::no_one;
         OpcodeArgValuePlayer player;
         OpcodeArgValueScalar addOrRemove;
         //
         virtual bool read(cobb::bitreader& stream) noexcept override {
            this->set_type.read(stream);
            if (this->set_type == PlayerSetType::specific_player) {
               return this->player.read(stream) && this->addOrRemove.read(stream);
            }
            return true;
         }
         virtual void write(cobb::bitwriter& stream) const noexcept override {
            this->set_type.write(stream);
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
         static OpcodeArgValue* factory(cobb::bitreader& stream) {
            return new OpcodeArgValuePlayerSet();
         }
   };
}