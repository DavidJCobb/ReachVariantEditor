#pragma once
#include "helpers/reflex/enumeration.h"
#include "./base_icon_operand.h"

namespace halo::reach::megalo::operands {
   using engine_icon = base_icon_operand<
      cobb::cs("engine_icon"),
      7,
      cobb::reflex::enumeration<
         cobb::reflex::member<cobb::cs("flag")>,            // 0
         cobb::reflex::member<cobb::cs("slayer")>,
         cobb::reflex::member<cobb::cs("oddball")>,
         cobb::reflex::member<cobb::cs("koth")>,
         cobb::reflex::member<cobb::cs("juggernaut")>,
         cobb::reflex::member<cobb::cs("territories")>,     // 5
         cobb::reflex::member<cobb::cs("assault")>,
         cobb::reflex::member<cobb::cs("infection")>,
         cobb::reflex::member<cobb::cs("vip")>,
         cobb::reflex::member<cobb::cs("invasion")>,
         cobb::reflex::member<cobb::cs("invasion_slayer")>, // 10
         cobb::reflex::member<cobb::cs("stockpile")>,
         cobb::reflex::member<cobb::cs("action_sack")>,
         cobb::reflex::member<cobb::cs("race")>,
         cobb::reflex::member<cobb::cs("rocket_race")>,
         cobb::reflex::member<cobb::cs("grifball")>,        // 15
         cobb::reflex::member<cobb::cs("soccer")>,
         cobb::reflex::member<cobb::cs("headhunter")>,
         cobb::reflex::member<cobb::cs("crosshair")>,
         cobb::reflex::member<cobb::cs("wheel")>,
         cobb::reflex::member<cobb::cs("swirl")>,           // 20
         cobb::reflex::member<cobb::cs("bunker")>,
         cobb::reflex::member<cobb::cs("health_pack")>,
         cobb::reflex::member<cobb::cs("castle_defense")>,
         cobb::reflex::member<cobb::cs("return")>,
         cobb::reflex::member<cobb::cs("shapes")>,          // 25
         cobb::reflex::member<cobb::cs("cartographer")>,
         cobb::reflex::member<cobb::cs("eight_ball")>,
         cobb::reflex::member<cobb::cs("noble")>,
         cobb::reflex::member<cobb::cs("covenant")>,
         cobb::reflex::member<cobb::cs("attack")>,          // 30
         cobb::reflex::member<cobb::cs("defend")>,
         cobb::reflex::member<cobb::cs("ordnance")>,
         cobb::reflex::member<cobb::cs("circle")>,
         cobb::reflex::member<cobb::cs("recon")>,
         cobb::reflex::member<cobb::cs("recover")>,         // 35
         cobb::reflex::member<cobb::cs("ammo")>,
         cobb::reflex::member<cobb::cs("skull")>,
         cobb::reflex::member<cobb::cs("forge")>,           // 38
         //
         // ... unused values ...
         //
         cobb::reflex::member<cobb::cs("recent_games"), 50>,
         cobb::reflex::member<cobb::cs("file_share"),   51>//,
      >
   >;
   using hud_widget_icon = base_icon_operand<
      cobb::cs("hud_widget_icon"),
      6,
      cobb::reflex::enumeration<
         cobb::reflex::member<cobb::cs("flag")>,
         cobb::reflex::member<cobb::cs("slayer")>,
         cobb::reflex::member<cobb::cs("oddball")>,
         cobb::reflex::member<cobb::cs("koth")>,
         cobb::reflex::member<cobb::cs("juggernaut")>,
         cobb::reflex::member<cobb::cs("territories")>,
         cobb::reflex::member<cobb::cs("assault")>,
         cobb::reflex::member<cobb::cs("infection")>,
         cobb::reflex::member<cobb::cs("vip")>,
         cobb::reflex::member<cobb::cs("invasion")>,
         cobb::reflex::member<cobb::cs("invasion_slayer")>,
         cobb::reflex::member<cobb::cs("stockpile")>,
         cobb::reflex::member<cobb::cs("action_sack")>,
         cobb::reflex::member<cobb::cs("race")>,
         cobb::reflex::member<cobb::cs("rocket_race")>,
         cobb::reflex::member<cobb::cs("grifball")>,
         cobb::reflex::member<cobb::cs("soccer")>,
         cobb::reflex::member<cobb::cs("headhunter")>,
         cobb::reflex::member<cobb::cs("crosshair")>,
         cobb::reflex::member<cobb::cs("wheel")>,
         cobb::reflex::member<cobb::cs("swirl")>,
         cobb::reflex::member<cobb::cs("bunker")>,
         cobb::reflex::member<cobb::cs("health_pack")>,
         cobb::reflex::member<cobb::cs("castle_defense")>,
         cobb::reflex::member<cobb::cs("return")>,
         cobb::reflex::member<cobb::cs("shapes")>,
         cobb::reflex::member<cobb::cs("cartographer")>,
         cobb::reflex::member<cobb::cs("eight_ball")>,
         cobb::reflex::member<cobb::cs("noble")>,
         cobb::reflex::member<cobb::cs("covenant")>,
         cobb::reflex::member<cobb::cs("attack")>,
         cobb::reflex::member<cobb::cs("defend")>,
         cobb::reflex::member<cobb::cs("ordnance")>,
         cobb::reflex::member<cobb::cs("circle")>,
         cobb::reflex::member<cobb::cs("recon")>,
         cobb::reflex::member<cobb::cs("recover")>,
         cobb::reflex::member<cobb::cs("ammo")>,
         cobb::reflex::member<cobb::cs("skull")>,
         cobb::reflex::member<cobb::cs("forge")>//,
      >
   >;
}
