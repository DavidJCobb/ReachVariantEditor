#pragma once
#include <cstdint>
#include "helpers/reflex/enumeration.h"
#include "../../variable_scope.h"
#include "../../variable_type.h"

namespace halo::reach::megalo::operands::variables::top_level_values {
   using number = cobb::reflex::enumeration<
      unsigned int, // so std::bit_width doesn't complain about a signed type
      //
      cobb::reflex::member_range<
         cobb::cs("global.number[n]"),
         variable_scopes::global.max_numbers
      >//,
   >;

   using object = cobb::reflex::enumeration<
      unsigned int, // so std::bit_width doesn't complain about a signed type
      //
      cobb::reflex::member<cobb::cs("no_object")>,
      cobb::reflex::member_range<
         cobb::cs("global.object[n]"),
         variable_scopes::global.max_objects
      >,
      cobb::reflex::member<cobb::cs("current_object")>,
      cobb::reflex::member<cobb::cs("hud_target_object")>,
      cobb::reflex::member<cobb::cs("killed_object")>,
      cobb::reflex::member<cobb::cs("killer_object")>//,
   >;
   
   using player = cobb::reflex::enumeration<
      unsigned int, // so std::bit_width doesn't complain about a signed type
      //
      cobb::reflex::member<cobb::cs("no_player")>,
      cobb::reflex::member_range< // static players
         cobb::cs("player[n]"),
         16
      >,
      cobb::reflex::member_range<
         cobb::cs("global.player[n]"),
         variable_scopes::global.max_players
      >,
      cobb::reflex::member<cobb::cs("current_player")>,
      cobb::reflex::member<cobb::cs("hud_player")>,
      cobb::reflex::member<cobb::cs("hud_target_player")>,
      cobb::reflex::member<cobb::cs("killer_player")>//,
   >;
   
   using team = cobb::reflex::enumeration<
      unsigned int, // so std::bit_width doesn't complain about a signed type
      //
      cobb::reflex::member<cobb::cs("no_team")>,
      cobb::reflex::member_range< // static teams
         cobb::cs("team[n]"),
         8
      >,
      cobb::reflex::member<cobb::cs("neutral_team")>,
      cobb::reflex::member_range<
         cobb::cs("global.team[n]"),
         variable_scopes::global.max_teams
      >,
      cobb::reflex::member<cobb::cs("current_team")>,
      cobb::reflex::member<cobb::cs("hud_player_team")>,
      cobb::reflex::member<cobb::cs("hud_target_player_team")>,
      cobb::reflex::member<cobb::cs("hud_target_object_team")>,
      cobb::reflex::member<cobb::cs("unk_15_team")>//,
   >;
   
   using timer = cobb::reflex::enumeration<
      unsigned int, // so std::bit_width doesn't complain about a signed type
      //
      cobb::reflex::member_range<
         cobb::cs("global.timer[n]"),
         variable_scopes::global.max_timers
      >//,
   >;

   constexpr size_t max_of_type(variable_type t) {
      switch (t) {
         case variable_type::number: return number::underlying_value_range;
         case variable_type::object: return object::underlying_value_range;
         case variable_type::player: return player::underlying_value_range;
         case variable_type::team:   return team::underlying_value_range;
         case variable_type::timer:  return timer::underlying_value_range;
      }
      if (std::is_constant_evaluated()) {
         throw;
      }
      return 0;
   }
}
