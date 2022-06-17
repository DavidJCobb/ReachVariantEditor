#include "all.h"
#include "halo/reach/bitstreams.h"
#include "halo/bad_data_on_write_exception.h"

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
   void general::write(bitwriter& stream) const {
      stream.write(
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
   void respawn::write(bitwriter& stream) const {
      stream.write(
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
   void social::write(bitwriter& stream) const {
      stream.write(
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
   void map::write(bitwriter& stream) const {
      stream.write(
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
   void teams::write(bitwriter& stream) const {
      if (scoring > 3)
         throw bad_data_on_write_exception("invalid team scoring type");
      stream.write(
         scoring,
         species,
         designator_switch_type,
         definitions
      );
   }

   void loadouts::read(bitreader& stream) {
      stream.read(
         flags,
         palettes
      );
   }
   void loadouts::write(bitwriter& stream) const {
      stream.write(
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
   void all::write(bitwriter& stream) const {
      stream.write(
         general,
         respawn,
         social,
         map,
         teams,
         loadouts
      );
   }
}