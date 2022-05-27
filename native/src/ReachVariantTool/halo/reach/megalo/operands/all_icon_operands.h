#pragma once
#include "helpers/reflex_enum/reflex_enum.h"
#include "./base_icon_operand.h"

namespace halo::reach::megalo::operands {
   using engine_icon = base_icon_operand<
      cobb::cs("engine_icon"),
      7,
      cobb::reflex_enum<
         cobb::reflex_enum_member<cobb::cs("flag")>,            // 0
         cobb::reflex_enum_member<cobb::cs("slayer")>,
         cobb::reflex_enum_member<cobb::cs("oddball")>,
         cobb::reflex_enum_member<cobb::cs("koth")>,
         cobb::reflex_enum_member<cobb::cs("juggernaut")>,
         cobb::reflex_enum_member<cobb::cs("territories")>,     // 5
         cobb::reflex_enum_member<cobb::cs("assault")>,
         cobb::reflex_enum_member<cobb::cs("infection")>,
         cobb::reflex_enum_member<cobb::cs("vip")>,
         cobb::reflex_enum_member<cobb::cs("invasion")>,
         cobb::reflex_enum_member<cobb::cs("invasion_slayer")>, // 10
         cobb::reflex_enum_member<cobb::cs("stockpile")>,
         cobb::reflex_enum_member<cobb::cs("action_sack")>,
         cobb::reflex_enum_member<cobb::cs("race")>,
         cobb::reflex_enum_member<cobb::cs("rocket_race")>,
         cobb::reflex_enum_member<cobb::cs("grifball")>,        // 15
         cobb::reflex_enum_member<cobb::cs("soccer")>,
         cobb::reflex_enum_member<cobb::cs("headhunter")>,
         cobb::reflex_enum_member<cobb::cs("crosshair")>,
         cobb::reflex_enum_member<cobb::cs("wheel")>,
         cobb::reflex_enum_member<cobb::cs("swirl")>,           // 20
         cobb::reflex_enum_member<cobb::cs("bunker")>,
         cobb::reflex_enum_member<cobb::cs("health_pack")>,
         cobb::reflex_enum_member<cobb::cs("castle_defense")>,
         cobb::reflex_enum_member<cobb::cs("return")>,
         cobb::reflex_enum_member<cobb::cs("shapes")>,          // 25
         cobb::reflex_enum_member<cobb::cs("cartographer")>,
         cobb::reflex_enum_member<cobb::cs("eight_ball")>,
         cobb::reflex_enum_member<cobb::cs("noble")>,
         cobb::reflex_enum_member<cobb::cs("covenant")>,
         cobb::reflex_enum_member<cobb::cs("attack")>,          // 30
         cobb::reflex_enum_member<cobb::cs("defend")>,
         cobb::reflex_enum_member<cobb::cs("ordnance")>,
         cobb::reflex_enum_member<cobb::cs("circle")>,
         cobb::reflex_enum_member<cobb::cs("recon")>,
         cobb::reflex_enum_member<cobb::cs("recover")>,         // 35
         cobb::reflex_enum_member<cobb::cs("ammo")>,
         cobb::reflex_enum_member<cobb::cs("skull")>,
         cobb::reflex_enum_member<cobb::cs("forge")>,           // 38
         //
         // ... unused values ...
         //
         cobb::reflex_enum_member<cobb::cs("recent_games"), 50>,
         cobb::reflex_enum_member<cobb::cs("file_share"),   51>//,
      >
   >;
   using hud_widget_icon = base_icon_operand<
      cobb::cs("hud_widget_icon"),
      6,
      cobb::reflex_enum<
         cobb::reflex_enum_member<cobb::cs("flag")>,
         cobb::reflex_enum_member<cobb::cs("slayer")>,
         cobb::reflex_enum_member<cobb::cs("oddball")>,
         cobb::reflex_enum_member<cobb::cs("koth")>,
         cobb::reflex_enum_member<cobb::cs("juggernaut")>,
         cobb::reflex_enum_member<cobb::cs("territories")>,
         cobb::reflex_enum_member<cobb::cs("assault")>,
         cobb::reflex_enum_member<cobb::cs("infection")>,
         cobb::reflex_enum_member<cobb::cs("vip")>,
         cobb::reflex_enum_member<cobb::cs("invasion")>,
         cobb::reflex_enum_member<cobb::cs("invasion_slayer")>,
         cobb::reflex_enum_member<cobb::cs("stockpile")>,
         cobb::reflex_enum_member<cobb::cs("action_sack")>,
         cobb::reflex_enum_member<cobb::cs("race")>,
         cobb::reflex_enum_member<cobb::cs("rocket_race")>,
         cobb::reflex_enum_member<cobb::cs("grifball")>,
         cobb::reflex_enum_member<cobb::cs("soccer")>,
         cobb::reflex_enum_member<cobb::cs("headhunter")>,
         cobb::reflex_enum_member<cobb::cs("crosshair")>,
         cobb::reflex_enum_member<cobb::cs("wheel")>,
         cobb::reflex_enum_member<cobb::cs("swirl")>,
         cobb::reflex_enum_member<cobb::cs("bunker")>,
         cobb::reflex_enum_member<cobb::cs("health_pack")>,
         cobb::reflex_enum_member<cobb::cs("castle_defense")>,
         cobb::reflex_enum_member<cobb::cs("return")>,
         cobb::reflex_enum_member<cobb::cs("shapes")>,
         cobb::reflex_enum_member<cobb::cs("cartographer")>,
         cobb::reflex_enum_member<cobb::cs("eight_ball")>,
         cobb::reflex_enum_member<cobb::cs("noble")>,
         cobb::reflex_enum_member<cobb::cs("covenant")>,
         cobb::reflex_enum_member<cobb::cs("attack")>,
         cobb::reflex_enum_member<cobb::cs("defend")>,
         cobb::reflex_enum_member<cobb::cs("ordnance")>,
         cobb::reflex_enum_member<cobb::cs("circle")>,
         cobb::reflex_enum_member<cobb::cs("recon")>,
         cobb::reflex_enum_member<cobb::cs("recover")>,
         cobb::reflex_enum_member<cobb::cs("ammo")>,
         cobb::reflex_enum_member<cobb::cs("skull")>,
         cobb::reflex_enum_member<cobb::cs("forge")>//,
      >
   >;
}
