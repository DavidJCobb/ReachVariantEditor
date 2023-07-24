#pragma once
#include "base.h"

namespace Megalo {
   class OpcodeArgValueScalar : public Variable {
      megalo_opcode_arg_value_make_create_override;
      public:
         static OpcodeArgTypeinfo typeinfo;
         OpcodeArgValueScalar();
         //
         virtual arg_compile_result compile(Compiler&, Script::VariableReference&, uint8_t part) noexcept override;
         virtual const OpcodeArgTypeinfo& get_variable_typeinfo() const noexcept override { return typeinfo; }
         //
         bool is_const_zero() const noexcept;
         void set_to_const_zero() noexcept;
         virtual Variable* create_zero_or_none() const noexcept override;
         virtual bool set_to_zero_or_none() noexcept override;
   };
   namespace variable_scope_indicators {
      namespace number {
         extern VariableScopeIndicatorValueList& as_list();
         //
         extern VariableScopeIndicatorValue constant;
         extern VariableScopeIndicatorValue player_number;
         extern VariableScopeIndicatorValue object_number;
         extern VariableScopeIndicatorValue team_number;
         extern VariableScopeIndicatorValue global_number;
         extern VariableScopeIndicatorValue option;
         extern VariableScopeIndicatorValue spawn_sequence;
         extern VariableScopeIndicatorValue team_score;
         extern VariableScopeIndicatorValue player_score;
         extern VariableScopeIndicatorValue player_money;
         extern VariableScopeIndicatorValue player_rating;
         extern VariableScopeIndicatorValue player_stat;
         extern VariableScopeIndicatorValue team_stat;
         extern VariableScopeIndicatorValue current_round;
         extern VariableScopeIndicatorValue symmetry;
         extern VariableScopeIndicatorValue symmetry_pregame;
         extern VariableScopeIndicatorValue score_to_win;
         extern VariableScopeIndicatorValue fireteams_enabled;
         extern VariableScopeIndicatorValue teams_enabled;
         extern VariableScopeIndicatorValue round_time_limit;
         extern VariableScopeIndicatorValue round_limit;
         extern VariableScopeIndicatorValue perfection_enabled;
         extern VariableScopeIndicatorValue rounds_to_win;
         extern VariableScopeIndicatorValue sudden_death_time;
         extern VariableScopeIndicatorValue grace_period;
         extern VariableScopeIndicatorValue lives_per_round;
         extern VariableScopeIndicatorValue team_lives_per_round;
         extern VariableScopeIndicatorValue respawn_time;
         extern VariableScopeIndicatorValue suicide_penalty;
         extern VariableScopeIndicatorValue betrayal_penalty;
         extern VariableScopeIndicatorValue respawn_growth;
         extern VariableScopeIndicatorValue loadout_cam_time;
         extern VariableScopeIndicatorValue respawn_traits_time;
         extern VariableScopeIndicatorValue friendly_fire;
         extern VariableScopeIndicatorValue betrayal_booting;
         extern VariableScopeIndicatorValue proximity_voice;
         extern VariableScopeIndicatorValue dont_team_restrict_chat;
         extern VariableScopeIndicatorValue dead_players_can_talk;
         extern VariableScopeIndicatorValue grenades_on_map;
         extern VariableScopeIndicatorValue indestructible_vehicles;
         extern VariableScopeIndicatorValue powerup_duration_r;
         extern VariableScopeIndicatorValue powerup_duration_b;
         extern VariableScopeIndicatorValue powerup_duration_y;
         extern VariableScopeIndicatorValue death_event_damage_type;
         extern VariableScopeIndicatorValue temporary_number; // added with MCC (backported from H4)
      }
   }
}