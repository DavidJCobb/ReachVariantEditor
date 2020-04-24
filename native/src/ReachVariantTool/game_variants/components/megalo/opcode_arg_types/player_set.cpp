#include "player_set.h"
#include "../../../../helpers/strings.h"
#include "../compiler/compiler.h"

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
   arg_compile_result OpcodeArgValuePlayerSet::compile(Compiler& compiler, Script::string_scanner& arg_text, uint8_t part) noexcept {
      if (part == 0) {
         auto word = arg_text.extract_word();
         if (word.isEmpty())
            return arg_compile_result::failure("Expected a player set type.");
         auto index = enums::player_set_group_name.lookup(word);
         if (index < 0)
            return arg_compile_result::failure("This argument is not a valid player set type.");
         this->set_type = (PlayerSetType)index;
         return arg_compile_result::success().set_needs_more(this->set_type == PlayerSetType::specific_player);
      }
      //
      auto arg = compiler.arg_to_variable(arg_text);
      if (!arg)
         return arg_compile_result::failure("This argument is not a variable.");
      auto result = this->compile(compiler, *arg, part);
      delete arg;
      return result;
   }
   arg_compile_result OpcodeArgValuePlayerSet::compile(Compiler& compiler, Script::VariableReference& arg, uint8_t part) noexcept {
      Variable* variable = nullptr;
      switch (part) {
         case 1: variable = &this->player; break;
         case 2: variable = &this->addOrRemove; break;
      }
      if (!variable)
         return arg_compile_result::failure();
      return variable->compile(compiler, arg, part).set_needs_more(part < 2);
   }
}