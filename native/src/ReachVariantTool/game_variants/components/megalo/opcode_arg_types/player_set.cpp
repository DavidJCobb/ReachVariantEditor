#include "player_set.h"
#include "../../../../helpers/strings.h"

namespace Megalo {
   namespace enums {
      auto player_set_group_name = DetailedEnum({ // currently only needed so we can import names via OpcodeArgTypeinfo
         DetailedEnumValue("no_one"),
         DetailedEnumValue("everyone"),
         DetailedEnumValue("allies"),
         DetailedEnumValue("enemies"),
         DetailedEnumValue("mod_player"),
         DetailedEnumValue("default"),
      });
   }
   OpcodeArgTypeinfo OpcodeArgValuePlayerSet::typeinfo = OpcodeArgTypeinfo(
      "_player_set",
      "Player Set",
      "A group of players.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValuePlayerSet>,
      enums::player_set_group_name
   );
   //
   bool OpcodeArgValuePlayerSet::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      this->set_type.read(stream);
      if (this->set_type == PlayerSetType::specific_player) {
         return this->player.read(stream, mp) && this->addOrRemove.read(stream, mp);
      }
      return true;
   }
   void OpcodeArgValuePlayerSet::write(cobb::bitwriter& stream) const noexcept {
      this->set_type.write(stream);
      if (this->set_type == PlayerSetType::specific_player) {
         this->player.write(stream);
         this->addOrRemove.write(stream);
      }
   }
   void OpcodeArgValuePlayerSet::to_string(std::string& out) const noexcept {
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
         case PlayerSetType::everyone:
            out = "everyone";
            return;
         case PlayerSetType::allies:
            out = "allies";
            return;
         case PlayerSetType::enemies:
            out = "enemies";
            return;
         case PlayerSetType::default:
            out = "default";
            return;
      }
      cobb::sprintf(out, "unknown:%u", (uint32_t)this->set_type);
   }
   void OpcodeArgValuePlayerSet::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      switch (this->set_type) {
         case PlayerSetType::no_one:
            out.write("no_one");
            return;
         case PlayerSetType::everyone:
            out.write("everyone");
            return;
         case PlayerSetType::allies:
            out.write("allies");
            return;
         case PlayerSetType::enemies:
            out.write("enemies");
            return;
         case PlayerSetType::default:
            out.write("default");
            return;
         case PlayerSetType::specific_player:
            out.write("mod_player, ");
            this->player.decompile(out, flags);
            out.write(", ");
            this->addOrRemove.decompile(out, flags);
            return;
      }

   }
}