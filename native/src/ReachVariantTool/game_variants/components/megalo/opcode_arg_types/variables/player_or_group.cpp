#include "player_or_group.h"
#include "all_core.h"
#include "../../../../errors.h"
#include "../../compiler/compiler.h"

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
   arg_compile_result OpcodeArgValuePlayerOrGroup::compile(Compiler& compiler, Script::string_scanner& arg_text, uint8_t part) noexcept {
      auto arg = compiler.arg_to_variable(arg_text);
      if (!arg)
         return arg_compile_result::failure("This argument is not a variable.");
      auto result = this->compile(compiler, *arg, part);
      delete arg;
      return result;
   }
   arg_compile_result OpcodeArgValuePlayerOrGroup::compile(Compiler& compiler, Script::VariableReference& arg, uint8_t part) noexcept {
      if (this->variable) {
         delete this->variable;
         this->variable = nullptr;
      }
      //
      auto type = arg.get_type();
      if (type == &OpcodeArgValuePlayer::typeinfo) {
         this->variable = new OpcodeArgValuePlayer;
      } else if (type == &OpcodeArgValueTeam::typeinfo) {
         this->variable = new OpcodeArgValueTeam;
      } else if (type == &OpcodeArgValueScalar::typeinfo || type == &OpcodeArgValueObject::typeinfo || type == &OpcodeArgValueTimer::typeinfo) {
         return arg_compile_result::failure("This variable is of the wrong type.");
      } else if (type == &OpcodeArgValuePlayerOrGroup::typeinfo) {
         //
         // This will happen if we're compiling the NamespaceMember specifically intended for 
         // the "all_players" value.
         //
         return arg_compile_result::success();
      } else {
         return arg_compile_result::failure("This argument is not a variable.");
      }
      return this->variable->compile(compiler, arg, part);
   }
   void OpcodeArgValuePlayerOrGroup::copy(const OpcodeArgValue* other) noexcept {
      auto cast = dynamic_cast<const OpcodeArgValuePlayerOrGroup*>(other);
      assert(cast);
      if (this->variable)
         delete this->variable;
      if (cast->variable)
         this->variable = (Variable*)cast->variable->clone();
      else
         this->variable = nullptr;
   }
   //
   variable_type OpcodeArgValuePlayerOrGroup::get_variable_type() const noexcept {
      if (!this->variable)
         return variable_type::not_a_variable;
      return this->variable->get_variable_type();
   }

}