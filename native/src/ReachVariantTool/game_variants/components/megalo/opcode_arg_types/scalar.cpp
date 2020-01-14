#include "scalar.h"
#include "../../../errors.h"
#include "../../../types/multiplayer.h"

namespace {
   enum class _scopes {
      constant,
      player_number,
      object_number,
      team_number,
      global_number,
      script_option,
      spawn_sequence,
      team_score,
      player_score,
      unknown_09, // unused in Reach? used in Halo 4?
      player_rating,
      player_stat,
      team_stat,
      current_round,
      symmetry_getter,
      symmetry,
      score_to_win,
      unkF7A6,
      teams_enabled,
      round_time_limit,
      round_limit,
      misc_unk0_bit3,
      rounds_to_win,
      sudden_death_time,
      grace_period,
      lives_per_round,
      team_lives_per_round,
      respawn_time,
      suicide_penalty,
      betrayal_penalty,
      respawn_growth,
      loadout_cam_time,
      respawn_traits_duration,
      friendly_fire,
      betrayal_booting,
      social_flags_bit_2,
      social_flags_bit_3,
      social_flags_bit_4,
      grenades_on_map,
      indestructible_vehicles,
      powerup_duration_red,
      powerup_duration_blue,
      powerup_duration_yellow,
      death_event_damage_type,
      //
      _count,
   };
}
namespace Megalo {
   /*virtual*/ bool OpcodeArgValueScalar::read(cobb::ibitreader& stream) noexcept /*override*/ {
      this->scope = stream.read_bits(cobb::bitcount((int)_scopes::_count - 1));
      int which_bits = 0;
      int index_bits = 0;
      const VariableScope* variable_scope = nullptr;
      switch ((_scopes)this->scope) {
         case _scopes::constant:
            stream.read(this->index);
            return true;
         case _scopes::player_number:
            variable_scope = &MegaloVariableScopePlayer;
            break;
         case _scopes::object_number:
            variable_scope = &MegaloVariableScopeObject;
            break;
         case _scopes::team_number:
            variable_scope = &MegaloVariableScopeTeam;
            break;
         case _scopes::global_number:
            variable_scope = &MegaloVariableScopeGlobal;
            break;
         case _scopes::script_option:
            index_bits = cobb::bitcount(16 - 1); // scripted options
            break;
         case _scopes::spawn_sequence:
            index_bits = MegaloVariableScopeObject.which_bits();
            break;
         case _scopes::team_score:
            index_bits = MegaloVariableScopeTeam.which_bits();
            break;
         case _scopes::player_score: // intentional fall-through
         case _scopes::unknown_09:
         case _scopes::player_rating:
            index_bits = MegaloVariableScopePlayer.which_bits();
            break;
         case _scopes::player_stat:
            which_bits = MegaloVariableScopePlayer.which_bits();
            index_bits = cobb::bitcount(4 - 1); // scripted stats
            break;
         case _scopes::team_stat:
            which_bits = MegaloVariableScopeTeam.which_bits();
            index_bits = cobb::bitcount(4 - 1); // scripted stats
            break;
         case _scopes::current_round:
         case _scopes::symmetry_getter:
         case _scopes::symmetry:
         case _scopes::score_to_win:
         case _scopes::unkF7A6:
         case _scopes::teams_enabled:
         case _scopes::round_time_limit:
         case _scopes::round_limit:
         case _scopes::misc_unk0_bit3:
         case _scopes::rounds_to_win:
         case _scopes::sudden_death_time:
         case _scopes::grace_period:
         case _scopes::lives_per_round:
         case _scopes::team_lives_per_round:
         case _scopes::respawn_time:
         case _scopes::suicide_penalty:
         case _scopes::betrayal_penalty:
         case _scopes::respawn_growth:
         case _scopes::loadout_cam_time:
         case _scopes::respawn_traits_duration:
         case _scopes::friendly_fire:
         case _scopes::betrayal_booting:
         case _scopes::social_flags_bit_2:
         case _scopes::social_flags_bit_3:
         case _scopes::social_flags_bit_4:
         case _scopes::grenades_on_map:
         case _scopes::indestructible_vehicles:
         case _scopes::powerup_duration_red:
         case _scopes::powerup_duration_blue:
         case _scopes::powerup_duration_yellow:
         case _scopes::death_event_damage_type:
            break; // these are all accessors to game state values and require no additional information other than the fact of their being used
         default:
            {
               auto& error = GameEngineVariantLoadError::get();
               error.state    = GameEngineVariantLoadError::load_state::failure;
               error.reason   = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
               error.detail   = GameEngineVariantLoadError::load_failure_detail::bad_variable_subtype;
               error.extra[0] = (int32_t)variable_type::scalar;
               error.extra[1] = this->scope;
            }
            return false;
      }
      if (variable_scope) {
         this->which = stream.read_bits<uint16_t>(variable_scope->which_bits());
         this->index = stream.read_bits<uint16_t>(variable_scope->index_bits(variable_type::scalar));
         if (!variable_scope->is_valid_which(this->which)) {
            auto& error = GameEngineVariantLoadError::get();
            error.state    = GameEngineVariantLoadError::load_state::failure;
            error.reason   = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
            error.detail   = GameEngineVariantLoadError::load_failure_detail::bad_variable_scope;
            error.extra[0] = (int32_t)getScopeConstantForObject(*variable_scope);
            error.extra[1] = this->which;
            error.extra[2] = this->index;
            error.extra[3] = (int32_t)variable_type::scalar;
            return false;
         }
         return true;
      }
      if (which_bits)
         this->which = stream.read_bits<uint16_t>(which_bits);
      if (index_bits)
         this->index = stream.read_bits<uint16_t>(index_bits);
      return true;
   }
   void OpcodeArgValueScalar::postprocess(GameVariantDataMultiplayer* mp) noexcept {
      auto& sd = mp->scriptData;
      auto& sc = mp->scriptContent;
      switch ((_scopes)this->scope) {
         case _scopes::player_stat:
         case _scopes::team_stat:
            if (this->index < 0 || this->index >= sc.stats.size())
               break;
            this->target = &sc.stats[this->index];
            break;
         case _scopes::script_option:
            if (this->index < 0 || this->index >= sd.options.size())
               break;
            this->target = &sd.options[this->index];
            break;
      }
   }
   void OpcodeArgValueScalar::write(cobb::bitwriter& stream) const noexcept {
      stream.write(this->scope, cobb::bitcount((int)_scopes::_count - 1));
      int which_bits = 0;
      int index_bits = 0;
      const VariableScope* variable_scope = nullptr;
      switch ((_scopes)this->scope) {
         case _scopes::constant:
            stream.write(this->index);
            return;
         case _scopes::player_number:
            variable_scope = &MegaloVariableScopePlayer;
            break;
         case _scopes::object_number:
            variable_scope = &MegaloVariableScopeObject;
            break;
         case _scopes::team_number:
            variable_scope = &MegaloVariableScopeTeam;
            break;
         case _scopes::global_number:
            variable_scope = &MegaloVariableScopeGlobal;
            break;
         case _scopes::script_option:
            index_bits = cobb::bitcount(Megalo::Limits::max_script_options - 1);
            if (this->target) {
               auto option = (ReachMegaloOption*)(void*)this->target; // TODO: make this less ugly
               stream.write(option->index, index_bits);
            } else {
               stream.write(0, index_bits);
            }
            return;
         case _scopes::spawn_sequence:
            index_bits = MegaloVariableScopeObject.which_bits();
            break;
         case _scopes::team_score:
            index_bits = MegaloVariableScopeTeam.which_bits();
            break;
         case _scopes::player_score: // intentional fall-through
         case _scopes::unknown_09:
         case _scopes::player_rating:
            index_bits = MegaloVariableScopePlayer.which_bits();
            break;
         case _scopes::player_stat:
            which_bits = MegaloVariableScopePlayer.which_bits();
            index_bits = cobb::bitcount(Megalo::Limits::max_script_stats - 1);
            stream.write(this->which, which_bits);
            if (this->target) {
               auto stat = (ReachMegaloGameStat*)(void*)this->target; // TODO: make this less ugly
               stream.write(stat->index, index_bits);
            } else {
               stream.write(0, index_bits);
            }
            return;
         case _scopes::team_stat:
            which_bits = MegaloVariableScopeTeam.which_bits();
            index_bits = cobb::bitcount(Megalo::Limits::max_script_stats - 1);
            stream.write(this->which, which_bits);
            if (this->target) {
               auto stat = (ReachMegaloGameStat*)(void*)this->target; // TODO: make this less ugly
               stream.write(stat->index, index_bits);
            } else {
               stream.write(0, index_bits);
            }
            return;
         case _scopes::current_round:
         case _scopes::symmetry_getter:
         case _scopes::symmetry:
         case _scopes::score_to_win:
         case _scopes::unkF7A6:
         case _scopes::teams_enabled:
         case _scopes::round_time_limit:
         case _scopes::round_limit:
         case _scopes::misc_unk0_bit3:
         case _scopes::rounds_to_win:
         case _scopes::sudden_death_time:
         case _scopes::grace_period:
         case _scopes::lives_per_round:
         case _scopes::team_lives_per_round:
         case _scopes::respawn_time:
         case _scopes::suicide_penalty:
         case _scopes::betrayal_penalty:
         case _scopes::respawn_growth:
         case _scopes::loadout_cam_time:
         case _scopes::respawn_traits_duration:
         case _scopes::friendly_fire:
         case _scopes::betrayal_booting:
         case _scopes::social_flags_bit_2:
         case _scopes::social_flags_bit_3:
         case _scopes::social_flags_bit_4:
         case _scopes::grenades_on_map:
         case _scopes::indestructible_vehicles:
         case _scopes::powerup_duration_red:
         case _scopes::powerup_duration_blue:
         case _scopes::powerup_duration_yellow:
         case _scopes::death_event_damage_type:
            break; // these are all accessors to game state values and require no additional information other than the fact of their being used
      }
      if (variable_scope) {
         stream.write(this->which, variable_scope->which_bits());
         stream.write(this->index, variable_scope->index_bits(variable_type::scalar));
         return;
      }
      if (which_bits)
         stream.write(this->which, which_bits);
      if (index_bits)
         stream.write(this->index, index_bits);
   }
   /*virtual*/ void OpcodeArgValueScalar::to_string(std::string& out) const noexcept /*override*/ {
      const char* which_scope = nullptr;
      switch ((_scopes)this->scope) {
         case _scopes::constant:
            cobb::sprintf(out, "%d", this->index);
            return;
         case _scopes::player_number:
            which_scope = megalo_players[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Number[%u]", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_PLAYER[%u].Number[%u]", this->which, this->index);
            return;
         case _scopes::object_number:
            which_scope = megalo_objects[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Number[%u]", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_OBJECT[%u].Number[%u]", this->which, this->index);
            return;
         case _scopes::team_number:
            which_scope = megalo_teams[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Number[%u]", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_TEAM[%u].Number[%u]", this->which, this->index);
            return;
         case _scopes::global_number:
            cobb::sprintf(out, "Global.Number[%u]", this->index);
            return;
         case _scopes::script_option:
            if (this->target) {
               auto option = (ReachMegaloOption*)(void*)this->target; // TODO: make this less ugly
               if (option->name)
                  cobb::sprintf(out, "Script Option \"%s\"", option->name->english().c_str());
               else
                  cobb::sprintf(out, "Script Option #%u", option->index);
               return;
            }
            cobb::sprintf(out, "Script Option #%u", this->index);
            return;
         case _scopes::spawn_sequence:
            which_scope = megalo_objects[this->index];
            if (which_scope)
               cobb::sprintf(out, "Spawn Sequence of %s", which_scope);
            else
               cobb::sprintf(out, "Spawn Sequence of INVALID_OBJECT[%u]", this->index);
            return;
         case _scopes::team_score:
            which_scope = megalo_teams[this->index];
            if (which_scope)
               cobb::sprintf(out, "Score of %s", which_scope);
            else
               cobb::sprintf(out, "Score of INVALID_TEAM[%u]", this->index);
            return;
         case _scopes::player_score:
            which_scope = megalo_players[this->index];
            if (which_scope)
               cobb::sprintf(out, "Score of %s", which_scope);
            else
               cobb::sprintf(out, "Score of INVALID_PLAYER[%u]", this->index);
            return;
         case _scopes::unknown_09: // unused in Reach? used in Halo 4?
            which_scope = megalo_players[this->index];
            if (which_scope)
               cobb::sprintf(out, "Unknown-09 of %s", which_scope);
            else
               cobb::sprintf(out, "Unknown-09 of INVALID_PLAYER[%u]", this->index);
            return;
         case _scopes::player_rating:
            which_scope = megalo_players[this->index];
            if (which_scope)
               cobb::sprintf(out, "Rating of %s", which_scope);
            else
               cobb::sprintf(out, "Rating of INVALID_PLAYER[%u]", this->index);
            return;
         case _scopes::player_stat:
            which_scope = megalo_players[this->which];
            if (this->target) {
               auto stat = (ReachMegaloGameStat*)(void*)this->target; // TODO: make this less ugly
               if (stat->name)
                  cobb::sprintf(out, "Stat \"%s\"", stat->name->english().c_str());
               else
                  cobb::sprintf(out, "Stat #%u", stat->index);
            } else {
               cobb::sprintf(out, "Stat #%u", this->index);
            }
            if (which_scope)
               cobb::sprintf(out, "%s for %s", out.c_str(), which_scope);
            else
               cobb::sprintf(out, "%s for INVALID_PLAYER[%u]", out.c_str(), this->which);
            return;
         case _scopes::team_stat:
            which_scope = megalo_teams[this->which];
            if (this->target) {
               auto stat = (ReachMegaloGameStat*)(void*)this->target; // TODO: make this less ugly
               if (stat->name)
                  cobb::sprintf(out, "Stat \"%s\"", stat->name->english().c_str());
               else
                  cobb::sprintf(out, "Stat #%u", stat->index);
            } else {
               cobb::sprintf(out, "Stat #%u", this->index);
            }
            if (which_scope)
               cobb::sprintf(out, "%s for %s", out.c_str(), which_scope);
            else
               cobb::sprintf(out, "%s for INVALID_TEAM[%u]", out.c_str(), this->which);
            return;
         case _scopes::current_round:
            out = "Current Round";
            return;
         case _scopes::symmetry_getter:
            out = "Symmetry (Read-Only)";
            return;
         case _scopes::symmetry:
            out = "Symmetry";
            return;
         case _scopes::score_to_win:
            out = "Score To Win";
            return;
         case _scopes::unkF7A6:
            out = "unkF7A6";
            return;
         case _scopes::teams_enabled:
            out = "Teams Enabled";
            return;
         case _scopes::round_time_limit:
            out = "Round Time Limit";
            return;
         case _scopes::round_limit:
            out = "Round Limit";
            return;
         case _scopes::misc_unk0_bit3:
            out = "Misc Options Unk0 Bit3";
            return;
         case _scopes::rounds_to_win:
            out = "Rounds To Win";
            return;
         case _scopes::sudden_death_time:
            out = "Sudden Death Time";
            return;
         case _scopes::grace_period:
            out = "Grace Period";
            return;
         case _scopes::lives_per_round:
            out = "Lives Per Round";
            return;
         case _scopes::team_lives_per_round:
            out = "Team Lives Per Round";
            return;
         case _scopes::respawn_time:
            out = "Respawn Time";
            return;
         case _scopes::suicide_penalty:
            out = "Suicide Penalty";
            return;
         case _scopes::betrayal_penalty:
            out = "Betrayal Penalty";
            return;
         case _scopes::respawn_growth:
            out = "Respawn Growth";
            return;
         case _scopes::loadout_cam_time:
            out = "Loadout Camera Time";
            return;
         case _scopes::respawn_traits_duration:
            out = "Respawn Traits Duration";
            return;
         case _scopes::friendly_fire:
            out = "Friendly Fire Enabled";
            return;
         case _scopes::betrayal_booting:
            out = "Betrayal Booting Enabled";
            return;
         case _scopes::social_flags_bit_2:
            out = "Social Flags Bit 2";
            return;
         case _scopes::social_flags_bit_3:
            out = "Social Flags Bit 3";
            return;
         case _scopes::social_flags_bit_4:
            out = "Social Flags Bit 4";
            return;
         case _scopes::grenades_on_map:
            out = "Grenades On Map";
            return;
         case _scopes::indestructible_vehicles:
            out = "Indestructible Vehicles";
            return;
         case _scopes::powerup_duration_red:
            out = "Red Powerup Duration";
            return;
         case _scopes::powerup_duration_blue:
            out = "Blue Powerup Duration";
            return;
         case _scopes::powerup_duration_yellow:
            out = "Yellow Powerup Duration";
            return;
         case _scopes::death_event_damage_type:
            out = "Death Event Damage Type";
            return;
      }
   }
}