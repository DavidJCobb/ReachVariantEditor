#pragma once
#include <cstdint>
#include "../helpers/bitstream.h"
#include "../helpers/bitnumber.h"

namespace reach {
   enum class ability : int8_t {
      // -4: random?
      unchanged        = -3,
      // -2: map default?
      none             = -1,
      sprint           =  0,
      jetpack          =  1,
      armor_lock       =  2,
      // 3
      active_camo      =  4,
      // 5
      // 6
      hologram         =  7,
      evade            =  8,
      drop_shield      =  9
   };
   enum class active_camo : uint8_t {
      unchanged = 0,
      off       = 1,
      // 2
      poor      = 3,
      good      = 4,
   };
   enum class bool_trait : uint8_t {
      unchanged = 0,
      disabled  = 1,
      enabled   = 2,
   };
   enum class damage_resist : uint8_t {
      unchanged    =  0,
      value_0010   =  1,
      value_0050   =  2,
      value_0090   =  3,
      value_0100   =  4,
      value_0110   =  5,
      value_0150   =  6,
      value_0200   =  7,
      value_0300   =  8,
      value_0500   =  9,
      value_1000   = 10,
      value_2000   = 11,
      invulnerable = 12,
      // bitfield can hold 13, 14, and 15, though they're likely not valid
   };
   enum class forced_color : uint8_t {
      unchanged =  0,
      none      =  1,
      red       =  2,
      blue      =  3,
      green     =  4,
      orange    =  5,
      purple    =  6,
      gold      =  7,
      brown     =  8,
      pink      =  9,
      white     = 10,
      black     = 11,
      zombie    = 12, // olive drab
      // bitfield can hold 13, 14, and 15, though they're likely not valid
   };
   enum class grenade_count : uint8_t {
      unchanged   =  0,
      map_default =  1,
      none        =  2,
      frag_1      =  3,
      frag_2      =  4,
      frag_3      =  5,
      frag_4      =  6,
      plasma_1    =  7,
      plasma_2    =  8,
      plasma_3    =  9,
      plasma_4    = 10,
      each_1      = 11,
      each_2      = 12,
      each_3      = 13,
      each_4      = 14,
      // bitfield can hold 15, though it's likely not valid
   };
   enum class infinite_ammo : uint8_t {
      unchanged  = 0,
      disabled   = 1,
      enabled    = 2,
      bottomless = 3,
   };
   enum class radar_range : uint8_t {
      unchanged  = 0,
      meters_010 = 1,
      // 2
      meters_025 = 3,
      // 4
      meters_075 = 5,
      meters_150 = 6,
      // bitfield can hold 7, though it's likely not valid
   };
   enum class radar_state : uint8_t {
      unchanged = 0,
      off       = 1,
      allies    = 2,
      normal    = 3,
      enhanced  = 4,
      // bitfield can hold 5, 6, and 7, though they're likely not valid
   };
   enum class vehicle_usage : uint8_t {
      unchanged      = 0,
      none           = 1,
      passenger_only = 2,
      driver_only    = 3,
      gunner_only    = 4,
      no_passenger   = 5,
      no_driver      = 6,
      no_gunner      = 7,
      full_use       = 8,
   };
   enum class visible_identity : uint8_t { // visible waypoint, visible name
      unchanged = 0,
      none      = 1,
      allies    = 2,
      everyone  = 3,
   };
   enum class weapon : int8_t {
      random           = -4,
      unchanged        = -3, // default
      map_default      = -2,
      none             = -1, // UI doesn't allow this for primary weapon
      dmr              =  0,
      assault_rifle    =  1,
      plasma_pistol    =  2,
      spiker           =  3,
      energy_sword     =  4,
      magnum           =  5,
      needler          =  6,
      plasma_rifle     =  7,
      rocket_launcher  =  8,
      shotgun          =  9,
      sniper_rifle     = 10,
      spartan_laser    = 11,
      gravity_hammer   = 12,
      plasma_repeater  = 13,
      needle_rifle     = 14,
      focus_rifle      = 15,
      plasma_launcher  = 16,
      concussion_rifle = 17,
      grenade_launcher = 18,
      golf_club        = 19,
      fuel_rod_gun     = 20,
   };
}

class ReachPlayerTraits {
   public:
      struct {
         cobb::bitnumber<4, reach::damage_resist> damageResist = reach::damage_resist::unchanged;
         cobb::bitnumber<3, uint8_t> healthMult;
         cobb::bitnumber<4, uint8_t> healthRate;
         cobb::bitnumber<3, uint8_t> shieldMult;
         cobb::bitnumber<4, uint8_t> shieldRate;
         cobb::bitnumber<4, uint8_t> shieldDelay;
         cobb::bitnumber<2, reach::bool_trait> headshotImmune = reach::bool_trait::unchanged;
         cobb::bitnumber<3, uint8_t> vampirism;
         cobb::bitnumber<2, reach::bool_trait> assassinImmune = reach::bool_trait::unchanged;
         cobb::bitnumber<2, uint8_t> unk09;
      } defense;
      struct {
         cobb::bitnumber<4, uint8_t> damageMult;
         cobb::bitnumber<4, uint8_t> meleeMult;
         cobb::bitnumber<8, reach::weapon> weaponPrimary   = reach::weapon::unchanged;
         cobb::bitnumber<8, reach::weapon> weaponSecondary = reach::weapon::unchanged;
         cobb::bitnumber<4, reach::grenade_count> grenadeCount = reach::grenade_count::unchanged;
         cobb::bitnumber<2, reach::infinite_ammo> infiniteAmmo = reach::infinite_ammo::unchanged;
         cobb::bitnumber<2, uint8_t> grenadeRegen;
         cobb::bitnumber<2, reach::bool_trait> weaponPickup = reach::bool_trait::unchanged;
         cobb::bitnumber<2, uint8_t> abilityDrop; // this and the next three may actually be the same value?
         cobb::bitnumber<2, uint8_t> infiniteAbility;
         cobb::bitnumber<2, uint8_t> abilityUsage;
         cobb::bitnumber<8, reach::ability> ability = reach::ability::unchanged;
      } offense;
      struct {
         cobb::bitnumber<5, uint8_t> speed;
         cobb::bitnumber<4, uint8_t> gravity;
         cobb::bitnumber<4, reach::vehicle_usage> vehicleUsage = reach::vehicle_usage::unchanged;
         cobb::bitnumber<2, uint8_t> unknown;
         cobb::bitnumber<9, int16_t, false, 0, std::true_type, -1> jumpHeight = -1;
      } movement;
      struct {
         cobb::bitnumber<3, reach::active_camo> activeCamo = reach::active_camo::unchanged;
         cobb::bitnumber<2, reach::visible_identity> waypoint    = reach::visible_identity::unchanged;
         cobb::bitnumber<2, reach::visible_identity> visibleName = reach::visible_identity::unchanged;
         cobb::bitnumber<3, uint8_t> aura;
         cobb::bitnumber<4, reach::forced_color> forcedColor = reach::forced_color::unchanged;
      } appearance;
      struct {
         cobb::bitnumber<3, reach::radar_state> radarState = reach::radar_state::unchanged;
         cobb::bitnumber<3, reach::radar_range> radarRange = reach::radar_range::unchanged;
         cobb::bitnumber<2, uint8_t> directionalDamageIndicator;
      } sensors;
      //
      void read(cobb::bitstream&) noexcept;
};

class ReachMegaloPlayerTraits : public ReachPlayerTraits {
};