#include "namespaces.h"
#include "../opcode_arg_types/variables/number.h"
#include "../opcode_arg_types/variables/object.h"
#include "../opcode_arg_types/variables/player.h"
#include "../opcode_arg_types/variables/team.h"
#include "../opcode_arg_types/variables/timer.h"

namespace Megalo {
   namespace Script {
      namespace namespaces {
         Namespace unnamed = Namespace("game", true, false, {
            NamespaceMember::make_which_member("no_object",         OpcodeArgValueObject::typeinfo, 'none', NamespaceMember::flag::is_none),
            NamespaceMember::make_which_member("no_player",         OpcodeArgValuePlayer::typeinfo, 'none', NamespaceMember::flag::is_none),
            NamespaceMember::make_which_member("no_team",           OpcodeArgValueTeam::typeinfo,   'none', NamespaceMember::flag::is_none),
            NamespaceMember::make_which_member("current_object",    OpcodeArgValueObject::typeinfo, 'loop', NamespaceMember::flag::is_read_only),
            NamespaceMember::make_which_member("current_player",    OpcodeArgValuePlayer::typeinfo, 'loop', NamespaceMember::flag::is_read_only),
            NamespaceMember::make_which_member("current_team",      OpcodeArgValueTeam::typeinfo,   'loop', NamespaceMember::flag::is_read_only),
            NamespaceMember::make_which_member("neutral_team",      OpcodeArgValueTeam::typeinfo,   'neut', NamespaceMember::flag::is_read_only),
            NamespaceMember::make_which_member("hud_target_object", OpcodeArgValueObject::typeinfo, 'hudt', NamespaceMember::flag::is_read_only),
            NamespaceMember::make_which_member("hud_target_player", OpcodeArgValuePlayer::typeinfo, 'hudt', NamespaceMember::flag::is_read_only),
            NamespaceMember::make_which_member("hud_player",        OpcodeArgValuePlayer::typeinfo, 'hudp', NamespaceMember::flag::is_read_only),
            NamespaceMember::make_which_member("killed_object",     OpcodeArgValueObject::typeinfo, 'kild', NamespaceMember::flag::is_read_only),
            NamespaceMember::make_which_member("killer_object",     OpcodeArgValueObject::typeinfo, 'kilr', NamespaceMember::flag::is_read_only),
            NamespaceMember::make_which_member("killer_player",     OpcodeArgValuePlayer::typeinfo, 'kilr', NamespaceMember::flag::is_read_only),
            NamespaceMember::make_which_member("unk_14_team",       OpcodeArgValueTeam::typeinfo,   'u_14', NamespaceMember::flag::is_read_only),
            NamespaceMember::make_which_member("unk_15_team",       OpcodeArgValueTeam::typeinfo,   'u_14', NamespaceMember::flag::is_read_only),
         });
         Namespace global = Namespace("global", false, true);
         Namespace game = Namespace("game", false, false, {
            NamespaceMember("betrayal_booting", OpcodeArgValueScalar::typeinfo, NamespaceMember::no_which, OpcodeArgValueScalar::scope_indicator_id::betrayal_booting),
            NamespaceMember("betrayal_penalty", OpcodeArgValueScalar::typeinfo, NamespaceMember::no_which, OpcodeArgValueScalar::scope_indicator_id::betrayal_penalty),
            NamespaceMember("current_round",    OpcodeArgValueScalar::typeinfo, NamespaceMember::no_which, OpcodeArgValueScalar::scope_indicator_id::current_round),
            NamespaceMember::make_scope_member("death_event_damage_type", OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::death_event_damage_type),
            NamespaceMember::make_scope_member("friendly_fire",           OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::friendly_fire),
            NamespaceMember::make_scope_member("grace_period",            OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::grace_period),
            NamespaceMember::make_scope_member("grenades_on_map",         OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::grenades_on_map),
            NamespaceMember::make_scope_member("indestructible_vehicles", OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::indestructible_vehicles),
            NamespaceMember::make_scope_member("lives_per_round",         OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::lives_per_round),
            NamespaceMember::make_scope_member("loadout_cam_time",        OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::loadout_cam_time),
            NamespaceMember::make_scope_member("misc_unk0_bit3",          OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::misc_0_bit_3),
            NamespaceMember::make_scope_member("powerup_duration_red",    OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::powerup_time_r),
            NamespaceMember::make_scope_member("powerup_duration_blue",   OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::powerup_time_b),
            NamespaceMember::make_scope_member("powerup_duration_yellow", OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::powerup_time_y),
            NamespaceMember::make_scope_member("respawn_growth",          OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::respawn_growth),
            NamespaceMember::make_scope_member("respawn_time",            OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::respawn_time),
            NamespaceMember::make_scope_member("respawn_traits_duration", OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::respawn_traits_time),
            NamespaceMember::make_scope_member("round_limit",             OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::round_limit),
            NamespaceMember::make_scope_member("round_time_limit",        OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::round_time_limit),
            NamespaceMember::make_scope_member("rounds_to_win",           OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::rounds_to_win),
            NamespaceMember::make_scope_member("score_to_win",            OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::score_to_win),
            NamespaceMember::make_scope_member("social_flags_bit_2",      OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::social_flag_2),
            NamespaceMember::make_scope_member("social_flags_bit_3",      OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::social_flag_3),
            NamespaceMember::make_scope_member("social_flags_bit_4",      OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::social_flag_4),
            NamespaceMember::make_scope_member("sudden_death_time",       OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::sudden_death_time),
            NamespaceMember::make_scope_member("suicide_penalty",         OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::suicide_penalty),
            NamespaceMember::make_scope_member("symmetry",                OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::symmetry),
            NamespaceMember::make_scope_member("symmetry_getter",         OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::symmetry_get),
            NamespaceMember::make_scope_member("team_lives_per_round",    OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::team_lives_per_round),
            NamespaceMember::make_scope_member("teams_enabled",           OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::teams_enabled),
            NamespaceMember::make_scope_member("unkF7A6",                 OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::unkF7A6),
            //
            NamespaceMember::make_scope_member("round_timer",             OpcodeArgValueTimer::typeinfo,  OpcodeArgValueTimer::scope_indicator_id::round),
            NamespaceMember::make_scope_member("sudden_death_timer",      OpcodeArgValueTimer::typeinfo,  OpcodeArgValueTimer::scope_indicator_id::sudden),
            NamespaceMember::make_scope_member("grace_period_timer",      OpcodeArgValueTimer::typeinfo,  OpcodeArgValueTimer::scope_indicator_id::grace),
         });
      }
   }
}