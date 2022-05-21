#include "all.h"

namespace halo::reach::custom_game_options {
   void general::read(bitreader& stream) {
      stream.read(
         flags,
         time_limit,
         round_limit,
         rounds_to_win,
         sudden_death_time,
         grace_period_time
      );
   }

   void respawn::read(bitreader& stream) {
      stream.read(
         flags,
         lives_per_round,
         team_lives_per_round,
         respawn_time,
         suicide_penalty,
         betrayal_penalty,
         respawn_growth,
         loadout_camera_time,
         traits_duration,
         traits
      );
   }

   void social::read(bitreader& stream) {
      stream.read(
         observers,
         team_changes,
         flags
      );
   }

   void map::read(bitreader& stream) {
      stream.read(
         flags,
         base_player_traits,
         weapon_set,
         vehicle_set,
         powerups.red.traits,
         powerups.blue.traits,
         powerups.yellow.traits,
         powerups.red.duration,
         powerups.blue.duration,
         powerups.yellow.duration
      );
   }

   void teams::read(bitreader& stream) {
      stream.read(
         scoring,
         species,
         designator_switch_type,
         definitions
      );
      if (scoring > 3)
         scoring = 0;
   }

   void loadouts::read(bitreader& stream) {
      stream.read(
         flags,
         palettes
      );
   }

   void all::read(bitreader& stream) {
      stream.read(
         general,
         respawn,
         social,
         map,
         teams,
         loadouts
      );
   }
}