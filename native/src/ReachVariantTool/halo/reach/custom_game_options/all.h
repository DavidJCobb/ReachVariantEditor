#pragma once
#include <array>
#include "halo/reach/bitstreams.fwd.h"
#include "./loadout.h"
#include "./loadout_palette.h"
#include "./match_species.h"
#include "./powerup.h"
#include "./team_definition.h"
#include "./weapon_set.h"
#include "./vehicle_set.h"
#include "halo/reach/trait_set.h"

namespace halo::reach::custom_game_options {
   struct general {
      struct flag {
         flag() = delete;
         enum : uint8_t {
            perfection_enabled       = 0x01, // can players earn a Perfection medal in this gametype?
            new_round_resets_players = 0x02,
            new_round_resets_map     = 0x04,
            teams_enabled            = 0x08,
         };
      };
      
      bitnumber<4, uint8_t> flags; // game reads these individually (four read-bit-bool calls)
      bytenumber<uint8_t>   time_limit; // round time limit in minutes
      bitnumber<5, uint8_t> round_limit;
      bitnumber<4, uint8_t> rounds_to_win;
      bitnumber<7, int8_t, bitnumber_params<int8_t>{ .offset = true }> sudden_death_time; // seconds
      bitnumber<5, uint8_t> grace_period_time; // seconds

      void read(bitreader&);
   };
   
   struct respawn {
      struct flag {
         flag() = delete;
         enum : uint8_t {
            sync_with_team        = 0x01,
            respawn_with_teammate = 0x02,
            respawn_at_location   = 0x04,
            respawn_on_kills      = 0x08,
         };
      };
      
      bitnumber<4, uint8_t> flags;
      bitnumber<6, uint8_t> lives_per_round;
      bitnumber<7, uint8_t> team_lives_per_round;
      bytenumber<uint8_t>   respawn_time     = 5;
      bytenumber<uint8_t>   suicide_penalty  = 5;
      bytenumber<uint8_t>   betrayal_penalty = 5;
      bitnumber<4, uint8_t> respawn_growth   = 0;
      bitnumber<4, uint8_t> loadout_camera_time = 10;
      bitnumber<6, uint8_t> traits_duration;
      trait_set traits;

      void read(bitreader&);
   };
   
   struct social {
      struct flag {
         flag() = delete;
         enum : uint8_t {
            dead_player_voice = 0x01,
            global_voice      = 0x02, // can players hear enemies on voice chat, regardless of distance?
            proximity_voice   = 0x04,
            betrayal_booting  = 0x08,
            friendly_fire     = 0x10,
         };
      };
      
      enum class team_change_options {
         disabled,
         enabled,
         balancing_only,
      };
      
      bitbool observers = false; // game reads this but discards the value
      bitnumber<2, team_change_options> team_changes;
      bitnumber<5, uint8_t> flags;

      void read(bitreader&);
   };
   
   struct map {
      struct flag {
         flag() = delete;
         enum : uint8_t {
            grenades  = 0x01,
            shortcuts = 0x02,
            abilities = 0x04,
            powerups  = 0x08,
            turrets   = 0x10,
            indestructible_vehicles = 0x20,
         };
      };
      
      bitnumber<6, uint8_t> flags;
      trait_set base_player_traits;
      bitnumber<8, weapon_set>  weapon_set;  // map default == -2
      bitnumber<8, vehicle_set> vehicle_set; // map default == -2
      struct {
         powerup red;
         powerup blue;
         powerup yellow;
      } powerups;

      void read(bitreader&);
   };
   
   struct teams {
      bitnumber<3, uint8_t>          scoring; // values above 3 are treated as 0
      bitnumber<3, match_species>    species;
      bitnumber<2, uint8_t>          designator_switch_type;
      std::array<team_definition, 8> definitions;

      void read(bitreader&);
   };
   
   struct loadouts {
      bitnumber<2, uint8_t> flags; // flags: spartan loadouts; elite loadouts
      loadout_palette_set palettes;

      void read(bitreader&);
   };

   struct all {
      general  general;
      respawn  respawn;
      social   social;
      map      map;
      teams    teams;
      loadouts loadouts;

      void read(bitreader&);
   };
}