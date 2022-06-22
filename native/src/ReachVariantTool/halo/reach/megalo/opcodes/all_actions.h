#pragma once
#include <array>
#include "./action.h"
#include "../operand_typeinfo.h"

#include "../operands/all_enum_operands.h"
#include "../operands/all_flags_operands.h"
#include "../operands/all_icon_operands.h"
#include "../operands/constant_bool.h"
#include "../operands/fireteam_list.h"
#include "../operands/forge_label.h"
#include "../operands/format_string.h"
#include "../operands/hud_meter_parameters.h"
#include "../operands/hud_widget.h"
#include "../operands/incident.h"
#include "../operands/object_type.h"
#include "../operands/player_req_palette.h"
#include "../operands/player_set.h"
#include "../operands/player_traits.h"
#include "../operands/shape.h"
#include "../operands/sound.h"
#include "../operands/timer_rate.h"
#include "../operands/trigger.h"
#include "../operands/variant_string.h"
#include "../operands/vector3.h"
#include "../operands/variables/any.h"
#include "../operands/variables/number.h"
#include "../operands/variables/object.h"
#include "../operands/variables/player.h"
#include "../operands/variables/player_or_group.h"
#include "../operands/variables/relative_member.h"
#include "../operands/variables/team.h"
#include "../operands/variables/timer.h"
#include "../operands/variables/variable_and_member.h"
#include "../operands/waypoint_icon.h"

namespace halo::reach::megalo {
   inline constexpr auto all_actions = std::array{
      opcode_function{ // 0
         .name     = "none",
         .operands = {}
      },
      opcode_function{ // 1
         .name     = "modify_score",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player_or_group::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::assign_operator::typeinfo,
               .name     = "operator",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "value",
            },
         }
      },
      opcode_function{ // 2
         .name     = "place_at_me",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::object_type::typeinfo,
               .name     = "type",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::object::typeinfo,
               .name         = "result",
               .is_out_param = true,
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "basis",
            },
            opcode_function::operand_info{
               .typeinfo = operands::forge_label::typeinfo,
               .name     = "label",
            },
            opcode_function::operand_info{
               .typeinfo = operands::create_object_flags::typeinfo,
               .name     = "flags",
            },
            opcode_function::operand_info{
               .typeinfo = operands::vector3::typeinfo,
               .name     = "offset",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variant_string::typeinfo,
               .name     = "variant",
            },
         }
      },
      opcode_function{ // 3
         .name     = "delete",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
         }
      },
      opcode_function{ // 4
         .name     = "set_waypoint_visibility",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::player_set::typeinfo,
               .name     = "who",
            },
         }
      },
      opcode_function{ // 5
         .name     = "set_waypoint_icon",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::waypoint_icon::typeinfo,
               .name     = "icon",
            },
         }
      },
      opcode_function{ // 6
         .name     = "set_waypoint_icon",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::waypoint_priority::typeinfo,
               .name     = "priority",
            },
         }
      },
      opcode_function{ // 7
         .name     = "set_waypoint_timer",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::object_timer_relative_member::typeinfo,
               .name     = "timer",
            },
         }
      },
      opcode_function{ // 8
         .name     = "set_waypoint_range",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "min",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "max",
            },
         }
      },
      opcode_function{ // 9
         .name     = "assign",
         .operands = {
            opcode_function::operand_info{
               .typeinfo     = operands::variables::any::typeinfo,
               .name         = "lhs",
               .is_out_param = true,
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::any::typeinfo,
               .name     = "rhs",
            },
            opcode_function::operand_info{
               .typeinfo = operands::assign_operator::typeinfo,
               .name     = "operator",
            },
         }
      },
      opcode_function{ // 10
         .name     = "set_shape",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::shape::typeinfo,
               .name     = "shape",
            },
         }
      },
      opcode_function{ // 11
         .name     = "apply_traits",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::player_traits::typeinfo,
               .name     = "traits",
            },
         }
      },
      opcode_function{ // 12
         .name     = "set_pickup_permissions",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "weapon",
            },
            opcode_function::operand_info{
               .typeinfo = operands::player_set::typeinfo,
               .name     = "who",
            },
         }
      },
      opcode_function{ // 13
         .name     = "set_spawn_location_permissions",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::player_set::typeinfo,
               .name     = "who",
            },
         }
      },
      opcode_function{ // 14
         .name     = "set_spawn_location_fireteams",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::fireteam_list::typeinfo,
               .name     = "fireteams",
            },
         }
      },
      opcode_function{ // 15
         .name     = "set_progess_bar",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::player_set::typeinfo,
               .name     = "who",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::object_timer_relative_member::typeinfo,
               .name     = "timer",
            },
         }
      },
      opcode_function{ // 16
         .name     = "show_message_to",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player_or_group::typeinfo,
               .name     = "who",
            },
            opcode_function::operand_info{
               .typeinfo = operands::sound::typeinfo,
               .name     = "sound",
            },
            opcode_function::operand_info{
               .typeinfo = operands::format_string::typeinfo,
               .name     = "text",
            },
         }
      },
      opcode_function{ // 17
         .name     = "set_rate",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::timer::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::timer_rate::typeinfo,
               .name     = "rate",
            },
         }
      },
      opcode_function{ // 18
         .name     = "debug_print",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::format_string::typeinfo,
               .name     = "text",
            },
         }
      },
      opcode_function{ // 19
         .name     = "get_carrier",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "object",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::player::typeinfo,
               .name         = "result",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 20
         .name     = "call",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::trigger::typeinfo,
               .name     = "callee",
            },
         }
      },
      opcode_function{ // 21
         .name     = "end_round",
         .operands = {}
      },
      opcode_function{ // 22
         .name     = "set_shape_visibility",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::player_set::typeinfo,
               .name     = "who",
            },
         }
      },
      opcode_function{ // 23
         .name     = "kill",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::constant_bool::typeinfo,
               .name     = "silent",
            },
         }
      },
      opcode_function{ // 24
         .name     = "set_invincibility",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "invincible",
            },
         }
      },
      opcode_function{ // 25
         .name     = "rand",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "cap",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::number::typeinfo,
               .name         = "result",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 26
         .name     = "debug_break",
         .operands = {}
      },
      opcode_function{ // 27
         .name     = "get_orientation",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::number::typeinfo,
               .name         = "result",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 28
         .name     = "get_speed",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::number::typeinfo,
               .name         = "result",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 29
         .name     = "get_killer",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "victim",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::player::typeinfo,
               .name         = "killer",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 30
         .name     = "get_death_damage_type",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "victim",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::number::typeinfo,
               .name         = "damage_type",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 31
         .name     = "get_death_damage_mod",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "victim",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::number::typeinfo,
               .name         = "damage_mod",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 32
         .name     = "debug_set_tracing_enabled",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::constant_bool::typeinfo,
               .name     = "enabled",
            },
         }
      },
      opcode_function{ // 33
         .name     = "attach_to",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "subject",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "target",
            },
            opcode_function::operand_info{
               .typeinfo = operands::vector3::typeinfo,
               .name     = "offset",
            },
            opcode_function::operand_info{
               .typeinfo = operands::attach_position::typeinfo,
               .name     = "relative",
            },
         }
      },
      opcode_function{ // 34
         .name     = "detach",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "subject",
            },
         }
      },
      opcode_function{ // 35
         .name     = "get_scoreboard_pos",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::number::typeinfo,
               .name         = "result",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 36
         .name     = "get_scoreboard_pos",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::team::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::number::typeinfo,
               .name         = "result",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 37
         .name     = "get_spree_count",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::number::typeinfo,
               .name         = "result",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 38
         .name     = "increment_req_money_by",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "number",
            },
         }
      },
      opcode_function{ // 39
         .name     = "set_req_purchase_modes",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "enable",
            },
            opcode_function::operand_info{
               .typeinfo = operands::player_req_purchase_modes::typeinfo,
               .name     = "modes",
            },
         }
      },
      opcode_function{ // 40
         .name     = "get_vehicle",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::object::typeinfo,
               .name         = "result",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 41
         .name     = "force_into_vehicle",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "vehicle",
            },
         }
      },
      opcode_function{ // 42
         .name     = "set_biped",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "biped",
            },
         }
      },
      opcode_function{ // 43
         .name     = "reset",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::timer::typeinfo,
               .name     = "context",
            },
         }
      },
      opcode_function{ // 44
         .name     = "set_weapon_pickup_priority",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::pickup_priority::typeinfo,
               .name     = "priority",
            },
         }
      },
      opcode_function{ // 45
         .name     = "push_upward",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
         }
      },
      opcode_function{ // 46
         .name     = "set_text",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::hud_widget::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::format_string::typeinfo,
               .name     = "text",
            },
         }
      },
      opcode_function{ // 47
         .name     = "set_value_text",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::hud_widget::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::format_string::typeinfo,
               .name     = "text",
            },
         }
      },
      opcode_function{ // 48
         .name     = "set_meter_params",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::hud_widget::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::hud_meter_parameters::typeinfo,
               .name     = "parameters",
            },
         }
      },
      opcode_function{ // 49
         .name     = "set_icon",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::hud_widget::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::hud_widget_icon::typeinfo,
               .name     = "icon",
            },
         }
      },
      opcode_function{ // 50
         .name     = "set_visibility",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::hud_widget::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "player",
            },
            opcode_function::operand_info{
               .typeinfo = operands::constant_bool::typeinfo,
               .name     = "visible",
            },
         }
      },
      opcode_function{ // 51
         .name     = "play_sound_for",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::sound::typeinfo,
               .name     = "sound",
            },
            opcode_function::operand_info{
               .typeinfo = operands::constant_bool::typeinfo,
               .name     = "immediately",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::player_or_group::typeinfo,
               .name     = "who",
            },
         }
      },
      opcode_function{ // 52
         .name     = "set_scale",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "scale",
            },
         }
      },
      opcode_function{ // 53
         .name     = "set_waypoint_text",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::format_string::typeinfo,
               .name     = "text",
            },
         }
      },
      opcode_function{ // 54
         .name     = "get_shields",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::number::typeinfo,
               .name         = "result",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 55
         .name     = "get_health",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::number::typeinfo,
               .name         = "result",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 56
         .name     = "set_round_card_description",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::format_string::typeinfo,
               .name     = "text",
            },
         }
      },
      opcode_function{ // 57
         .name     = "set_round_card_icon_caption",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::format_string::typeinfo,
               .name     = "text",
            },
         }
      },
      opcode_function{ // 58
         .name     = "set_round_card_icon",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::engine_icon::typeinfo,
               .name     = "icon",
            },
         }
      },
      opcode_function{ // 59
         .name     = "set_co_op_spawning",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::team::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::constant_bool::typeinfo,
               .name     = "enabled",
            },
         }
      },
      opcode_function{ // 60
         .name     = "set_primary_respawn_object",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::team::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "respawn",
            },
         }
      },
      opcode_function{ // 61
         .name     = "set_primary_respawn_object",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "respawn",
            },
         }
      },
      opcode_function{ // 62
         .name     = "get_fireteam",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::number::typeinfo,
               .name         = "index",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 63
         .name     = "set_fireteam",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "index",
            },
         }
      },
      opcode_function{ // 64
         .name     = "modify_shields",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::assign_operator::typeinfo,
               .name     = "operator",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "value",
            },
         }
      },
      opcode_function{ // 65
         .name     = "modify_health",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::assign_operator::typeinfo,
               .name     = "operator",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "value",
            },
         }
      },
      opcode_function{ // 66
         .name     = "get_distance_to",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "a",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "b",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::number::typeinfo,
               .name         = "result",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 67
         .name     = "modify_max_shields",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::assign_operator::typeinfo,
               .name     = "operator",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "value",
            },
         }
      },
      opcode_function{ // 68
         .name     = "modify_max_health",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::assign_operator::typeinfo,
               .name     = "operator",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "value",
            },
         }
      },
      opcode_function{ // 69
         .name     = "set_requisition_palette",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::player_req_palette::typeinfo,
               .name     = "palette",
            },
         }
      },
      opcode_function{ // 70
         .name     = "set_device_power",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "power",
            },
         }
      },
      opcode_function{ // 71
         .name     = "get_device_power",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::number::typeinfo,
               .name         = "power",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 72
         .name     = "set_device_position",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "power",
            },
         }
      },
      opcode_function{ // 73
         .name     = "get_device_position",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::number::typeinfo,
               .name         = "power",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 74
         .name     = "modify_grenades",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::grenade_type::typeinfo,
               .name     = "type",
            },
            opcode_function::operand_info{
               .typeinfo = operands::assign_operator::typeinfo,
               .name     = "operator",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "value",
            },
         }
      },
      opcode_function{ // 75
         .name     = "send_incident",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::incident::typeinfo,
               .name     = "incident",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::player_or_group::typeinfo,
               .name     = "cause",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::player_or_group::typeinfo,
               .name     = "target",
            },
         }
      },
      opcode_function{ // 76
         .name     = "send_incident",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::incident::typeinfo,
               .name     = "incident",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::player_or_group::typeinfo,
               .name     = "cause",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::player_or_group::typeinfo,
               .name     = "target",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "value",
            },
         }
      },
      opcode_function{ // 77
         .name     = "set_loadout_palette",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::loadout_palette::typeinfo,
               .name     = "palette",
            },
         }
      },
      opcode_function{ // 78
         .name     = "set_device_animation_position",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variant_string::typeinfo,
               .name     = "animation",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "position",
            },
         }
      },
      opcode_function{ // 79
         .name     = "animate_device_position",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "animation_target",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "duration_seconds",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "acceleration_seconds",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "deceleration_seconds",
            },
         }
      },
      opcode_function{ // 80
         .name     = "set_device_actual_position",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "position",
            },
         }
      },
      opcode_function{ // 81
         .name     = "insert_theater_film_marker",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "number",
            },
            opcode_function::operand_info{
               .typeinfo = operands::format_string::typeinfo,
               .name     = "text",
            },
         }
      },
      opcode_function{ // 82
         .name     = "enable_spawn_zone",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "enabled",
            },
         }
      },
      opcode_function{ // 83
         .name     = "get_weapon",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::weapon_slot::typeinfo,
               .name     = "which",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::object::typeinfo,
               .name         = "result",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 84
         .name     = "get_armor_ability",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::object::typeinfo,
               .name         = "result",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 85
         .name     = "set_garbage_collection_disabled",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "disabled",
            },
         }
      },
      opcode_function{ // 86
         .name     = "get_crosshair_target",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::object::typeinfo,
               .name         = "result",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 87
         .name     = "place_between_me_and",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "a",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "b",
            },
            opcode_function::operand_info{
               .typeinfo = operands::object_type::typeinfo,
               .name     = "type",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "radius?",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::object::typeinfo,
               .name         = "result",
               .is_out_param = true,
            },
         }
      },
      opcode_function{ // 88
         .name     = "debug_force_player_view_count",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "radius",
            },
         }
      },
      opcode_function{ // 89
         .name     = "add_weapon",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "weapon",
            },
         }
      },
      opcode_function{ // 90
         .name     = "set_co_op_spawning",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::constant_bool::typeinfo,
               .name     = "enabled",
            },
         }
      },
      opcode_function{ // 91
         .name     = "copy_rotation_from",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "a",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "b",
            },
            opcode_function::operand_info{
               .typeinfo = operands::constant_bool::typeinfo,
               .name     = "all_axes",
            },
         }
      },
      opcode_function{ // 92
         .name     = "face_toward",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "target",
            },
            opcode_function::operand_info{
               .typeinfo = operands::vector3::typeinfo,
               .name     = "offset",
            },
         }
      },
      opcode_function{ // 93
         .name     = "add_weapon",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "biped",
            },
            opcode_function::operand_info{
               .typeinfo = operands::object_type::typeinfo,
               .name     = "type",
            },
            opcode_function::operand_info{
               .typeinfo = operands::add_weapon_type::typeinfo,
               .name     = "mode",
            },
         }
      },
      opcode_function{ // 94
         .name     = "remove_weapon",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "biped",
            },
            opcode_function::operand_info{
               .typeinfo = operands::drop_weapon_slot::typeinfo,
               .name     = "which",
            },
            opcode_function::operand_info{
               .typeinfo = operands::constant_bool::typeinfo,
               .name     = "delete",
            },
         }
      },
      opcode_function{ // 95
         .name     = "set_scenario_interpolator_state",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "which",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "state",
            },
         }
      },
      opcode_function{ // 96
         .name     = "get_random_object",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "exclude",
            },
            opcode_function::operand_info{
               .typeinfo     = operands::variables::object::typeinfo,
               .name         = "result",
               .is_out_param = true,
            },
            opcode_function::operand_info{
               .typeinfo = operands::forge_label::typeinfo,
               .name     = "label",
            },
         }
      },
      opcode_function{ // 97
         .name     = "record_griefer_penalty",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::number::typeinfo,
               .name     = "number",
            },
         }
      },
      opcode_function{ // 98
         .name     = "apply_shape_color_from_player_member",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object_and_player_member::typeinfo,
               .name     = "member",
            },
         }
      },
   };
}
