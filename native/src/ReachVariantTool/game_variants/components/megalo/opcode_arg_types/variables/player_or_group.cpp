#include "player_or_group.h"
#include "player.h"
#include "team.h"
#include "../../../../errors.h"

namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValuePlayerOrGroup::typeinfo = OpcodeArgTypeinfo(
      "_player_or_group",
      "Player-or-Group",
      "A reference to either a team, a player, or all players.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValuePlayerOrGroup>
   ).set_accessor_proxy_types({
      &OpcodeArgValuePlayer::typeinfo,
      &OpcodeArgValueTeam::typeinfo
   });
   //
   bool OpcodeArgValuePlayerOrGroup::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      uint8_t type = stream.read_bits(2);
      switch (type) {
         case 0: this->variable = new OpcodeArgValueTeam;   break; // team
         case 1: this->variable = new OpcodeArgValuePlayer; break; // player
         case 2: // All Players
            return true;
      }
      if (this->variable)
         return this->variable->read(stream, mp);
      auto& error = GameEngineVariantLoadError::get();
      error.state  = GameEngineVariantLoadError::load_state::failure;
      error.reason = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
      error.detail = GameEngineVariantLoadError::load_failure_detail::bad_opcode_player_or_team_var_type;
      error.extra[0] = type;
      return false;
   }
   void OpcodeArgValuePlayerOrGroup::write(cobb::bitwriter& stream) const noexcept {
      if (!this->variable) { // All Players
         stream.write(2, 2);
         return;
      }
      switch (this->variable->get_variable_type()) {
         case variable_type::team:
            stream.write(0, 2);
            break;
         case variable_type::player:
            stream.write(1, 2);
            break;
         default:
            return;
      }
      this->variable->write(stream);
   }
   void OpcodeArgValuePlayerOrGroup::to_string(std::string& out) const noexcept {
      if (this->variable)
         this->variable->to_string(out);
      else
         out = "all players";
   }
   void OpcodeArgValuePlayerOrGroup::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      if (this->variable)
         this->variable->decompile(out, flags);
      else
         out.write("all_players");
   }
   //
   variable_type OpcodeArgValuePlayerOrGroup::get_variable_type() const noexcept {
      if (!this->variable)
         return variable_type::not_a_variable;
      return this->variable->get_variable_type();
   }

}