#pragma once
#include "./variables/all_core_types.h"
#include "./variables/any.h"
#include "./variables/player_or_group.h"
#include "./variables/relative_member.h"
#include "./variables/variable_and_member.h"
#include "./all_enum_operands.h"
#include "./all_flags_operands.h"
#include "./all_icon_operands.h"
#include "./constant_bool.h"
#include "./fireteam_list.h"
#include "./forge_label.h"
#include "./format_string.h"
#include "./hud_meter_parameters.h"
#include "./hud_widget.h"
#include "./incident.h"
#include "./object_type.h"
#include "./player_req_palette.h"
#include "./player_set.h"
#include "./player_traits.h"
#include "./shape.h"
#include "./sound.h"
#include "./timer_rate.h"
#include "./trigger.h"
#include "./variant_string.h"
#include "./vector3.h"
#include "./waypoint_icon.h"

#include <tuple>

namespace halo::reach::megalo {
   using all_operands_tuple = std::tuple<
      operands::add_weapon_type,
      operands::assign_operator,
      operands::attach_position,
      operands::c_hud_destination,
      operands::compare_operator,
      operands::constant_bool,
      operands::create_object_flags,
      operands::drop_weapon_slot,
      operands::engine_icon,
      operands::fireteam_list,
      operands::forge_label,
      operands::format_string,
      operands::grenade_type,
      operands::hud_meter_parameters,
      operands::hud_widget,
      operands::hud_widget_icon,
      operands::incident,
      operands::killer_types,
      operands::loadout_palette,
      operands::object_type,
      operands::pickup_priority,
      operands::player_req_palette,
      operands::player_req_purchase_modes,
      operands::player_set,
      operands::player_traits,
      operands::shape,
      operands::sound,
      operands::team_alliance_status,
      operands::timer_rate,
      operands::trigger,
      operands::variables::any,
      operands::variables::number,
      operands::variables::object,
      operands::variables::object_and_player_member,
      operands::variables::object_timer_relative_member,
      operands::variables::player,
      operands::variables::player_or_group,
      operands::variables::team,
      operands::variables::timer,
      operands::variant_string,
      operands::vector3,
      operands::waypoint_icon,
      operands::waypoint_priority,
      operands::weapon_slot//,
   >;
}