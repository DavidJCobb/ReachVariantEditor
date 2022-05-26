#pragma once
#include "helpers/reflex_enum/reflex_enum.h"
#include "./base_enum_operand.h"

namespace halo::reach::megalo::operands {
   using add_weapon_type = base_enum_operand<
      cobb::cs("add_weapon_type"),
      cobb::reflex_enum<
         cobb::reflex_enum_member<cobb::cs("primary")>,
         cobb::reflex_enum_member<cobb::cs("secondary")>,
         cobb::reflex_enum_member<cobb::cs("force")>
      >
   >;
   using assign_operator = base_enum_operand<
      cobb::cs("assign_operator"),
      cobb::reflex_enum<
         cobb::reflex_enum_member<cobb::cs("+=")>,
         cobb::reflex_enum_member<cobb::cs("-=")>,
         cobb::reflex_enum_member<cobb::cs("*=")>,
         cobb::reflex_enum_member<cobb::cs("/=")>,
         cobb::reflex_enum_member<cobb::cs("=")>,
         cobb::reflex_enum_member<cobb::cs("%=")>,
         cobb::reflex_enum_member<cobb::cs("&=")>,
         cobb::reflex_enum_member<cobb::cs("|=")>,
         cobb::reflex_enum_member<cobb::cs("^=")>,
         cobb::reflex_enum_member<cobb::cs("~=")>, // (a ~= b) == (a &= ~b) // TODO: VERIFY
         //
         cobb::reflex_enum_member<cobb::cs("+=?")> // KSoft calls this <<= but it acts like addition in MCC-Reach tests
      >
   >;
   using attach_position = base_enum_operand<
      cobb::cs("attach_position"),
      cobb::reflex_enum<
         cobb::reflex_enum_member<cobb::cs("absolute")>,
         cobb::reflex_enum_member<cobb::cs("relative")>
      >
   >;
   using c_hud_destination = base_enum_operand<
      cobb::cs("c_hud_destination"),
      cobb::reflex_enum<
         cobb::reflex_enum_member<cobb::cs("unk_0")>,
         cobb::reflex_enum_member<cobb::cs("unk_1")>
      >
   >;
   using compare_operator = base_enum_operand<
      cobb::cs("compare_operator"),
      cobb::reflex_enum<
         cobb::reflex_enum_member<cobb::cs("<")>,
         cobb::reflex_enum_member<cobb::cs(">")>,
         cobb::reflex_enum_member<cobb::cs("==")>,
         cobb::reflex_enum_member<cobb::cs("<=")>,
         cobb::reflex_enum_member<cobb::cs(">=")>,
         cobb::reflex_enum_member<cobb::cs("!=")>,
      >
   >;
   using drop_weapon_slot = base_enum_operand<
      cobb::cs("drop_weapon_slot"),
      cobb::reflex_enum<
         cobb::reflex_enum_member<cobb::cs("secondary")>,
         cobb::reflex_enum_member<cobb::cs("primary")>
      >
   >;
   using grenade_type = base_enum_operand<
      cobb::cs("grenade_type"),
      cobb::reflex_enum<
         cobb::reflex_enum_member<cobb::cs("frag_grenades")>,
         cobb::reflex_enum_member<cobb::cs("plasma_grenades")>
      >
   >;
   using loadout_palette = base_enum_operand<
      cobb::cs("loadout_palette"),
      cobb::reflex_enum<
         cobb::reflex_enum_member<cobb::cs("none")>,
         cobb::reflex_enum_member<cobb::cs("spartan_tier_1")>,
         cobb::reflex_enum_member<cobb::cs("elite_tier_1")>,
         cobb::reflex_enum_member<cobb::cs("spartan_tier_2")>,
         cobb::reflex_enum_member<cobb::cs("elite_tier_2")>,
         cobb::reflex_enum_member<cobb::cs("spartan_tier_3")>,
         cobb::reflex_enum_member<cobb::cs("elite_tier_3")>
      >
   >;
   using pickup_priority = base_enum_operand<
      cobb::cs("pickup_priority"),
      cobb::reflex_enum<
         cobb::reflex_enum_member<cobb::cs("normal")>,
         cobb::reflex_enum_member<cobb::cs("high")>,
         cobb::reflex_enum_member<cobb::cs("automatic")>,
      >
   >;
   using team_alliance_status = base_enum_operand<
      cobb::cs("team_alliance_status"),
      cobb::reflex_enum<
         cobb::reflex_enum_member<cobb::cs("neutral")>,
         cobb::reflex_enum_member<cobb::cs("friendly")>,
         cobb::reflex_enum_member<cobb::cs("enemy")>,
      >
   >;
   using waypoint_priority = base_enum_operand<
      cobb::cs("waypoint_priority"),
      cobb::reflex_enum<
         cobb::reflex_enum_member<cobb::cs("low")>,
         cobb::reflex_enum_member<cobb::cs("normal")>,
         cobb::reflex_enum_member<cobb::cs("high")>,
         cobb::reflex_enum_member<cobb::cs("blink")>,
      >
   >;
   using weapon_slot = base_enum_operand<
      cobb::cs("weapon_slot"),
      cobb::reflex_enum<
         cobb::reflex_enum_member<cobb::cs("secondary")>,
         cobb::reflex_enum_member<cobb::cs("primary")>,
      >
   >;
}
