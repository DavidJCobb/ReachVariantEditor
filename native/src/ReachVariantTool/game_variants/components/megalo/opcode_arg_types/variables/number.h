#pragma once
#include "base.h"

namespace Megalo {
   class OpcodeArgValueScalar : public Variable {
      public:
         static OpcodeArgTypeinfo typeinfo;
         OpcodeArgValueScalar();
         //
         bool is_const_zero() const noexcept;
         //
         struct scope_indicator_id {
            scope_indicator_id() = delete;
            enum type : uint8_t {
               constant,
               p_n,
               o_n,
               t_n,
               g_n,
               option,
               spawn_sequence,
               t_score,
               p_score,
               p_unk09,
               p_rating,
               p_stat,
               t_stat,
               current_round,
               symmetry_get,
               symmetry,
               score_to_win,
               unkF7A6,
               teams_enabled,
               round_time_limit,
               round_limit,
               misc_0_bit_3,
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
               respawn_traits_time,
               friendly_fire,
               betrayal_booting,
               social_flag_2,
               social_flag_3,
               social_flag_4,
               grenades_on_map,
               indestructible_vehicles,
               powerup_time_r,
               powerup_time_b,
               powerup_time_y,
               death_event_damage_type,
            };
         };
   };
}