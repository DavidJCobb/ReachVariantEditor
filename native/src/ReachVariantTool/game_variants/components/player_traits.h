#pragma once
#include <cstdint>
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitreader.h"
#include "../../helpers/bitwriter.h"
#include "../../formats/localized_string_table.h"
#include <QObject>

namespace reach {
   //
   // TODO: A lot of these enums have values that aren't ever exposed in the UI; we should 
   // try to determine those values' function through testing. Additionally, some enums are 
   // able to hold bits past the last known value; we should see if the game engine uses 
   // those bits, or if they're valid in general.
   //
   enum class ability : int8_t {
      // -4: Icon is two concentric circles. Acts as Sprint.
      unchanged        = -3,
      // -2: Acts as None.
      none             = -1,
      sprint           =  0,
      jetpack          =  1,
      armor_lock       =  2,
      //  3: KSoft.Tool identifies this as "power fist." Armor Lock icon in loadout selection; acts as None.
      active_camo      =  4,
      //  5: KSoft.Tool identifies this as "ammo pack." Armor Lock icon in loadout selection; acts as None.
      //  6: KSoft.Tool identifies this as "sensor pack." Armor Lock icon in loadout selection; acts as None.
      hologram         =  7,
      evade            =  8,
      drop_shield      =  9
   };
   enum class ability_usage : uint8_t {
      unchanged = 0,
      disabled  = 1,
      default   = 2,
      enabled   = 3,
   };
   enum class active_camo : uint8_t {
      unchanged = 0,
      off       = 1,
      unknown_2 = 2, // fades only very slightly with movement
      poor      = 3,
      good      = 4,
      best      = 5, // seen in Blue Powerup Traits; doesn't fade with movement
      // bitfield can hold 6 and 7, though they're likely not valid
   };
   enum class aura : uint8_t {
      unchanged    = 0,
      unknown_1    = 1,
      unknown_2    = 2,
      darken_armor = 3, // armor color becomes darker and faded
      pastel_armor = 4, // orange armor becomes sulfur yellow
      unknown_5    = 5,
      unknown_6    = 6,
      // bitfield can hold 7, but using that causes the MCC to crash on startup
   };
   enum class bool_trait : uint8_t {
      unchanged = 0,
      disabled  = 1,
      enabled   = 2,
   };
   enum class damage_multiplier : uint8_t {
      unchanged =  0,
      value_000 =  1,
      value_025 =  2,
      value_050 =  3,
      value_075 =  4,
      value_090 =  5,
      value_100 =  6,
      value_110 =  7,
      value_125 =  8,
      value_150 =  9,
      value_200 = 10,
      value_300 = 11,
      // bitfield can hold 12, 13, 14, and 15, though they're likely not valid
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
   enum class health_rate : uint8_t {
      unchanged = 0,
      // 1 (possibly decay)
      // 2 (possibly decay)
      // 3 (possibly decay)
      value_000 = 4,
      value_050 = 5,
      value_090 = 6,
      value_100 = 7,
      value_110 = 8,
      value_200 = 9,
   };
   enum class infinite_ammo : uint8_t {
      unchanged  = 0,
      disabled   = 1,
      enabled    = 2,
      bottomless = 3,
   };
   enum class movement_speed : uint8_t {
      unchanged =  0,
      value_000 =  1,
      value_025 =  2,
      value_050 =  3,
      value_075 =  4,
      value_090 =  5,
      value_100 =  6,
      value_110 =  7,
      value_120 =  8,
      //  9 == 130?
      // 10 == 140?
      value_150 = 11,
      // 12 == 160?
      // 13 == 170?
      // 14 == 180?
      // 15 == 190?
      value_200 = 16,
      value_300 = 17,
      // bitfield can hold up to 31, though values past the known end likely aren't valid
   };
   enum class player_gravity : uint8_t {
      unchanged = 0,
      value_050 = 1,
      value_075 = 2,
      value_100 = 3,
      value_150 = 4,
      value_200 = 5,
      // bitfield can hold up to 15, though values past the known end likely aren't valid
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
   enum class shield_multiplier : uint8_t {
      unchanged = 0,
      value_000 = 1,
      value_100 = 2,
      value_150 = 3,
      value_200 = 4,
      value_300 = 5,
      value_400 = 6,
      // bitfield can hold 7, but that's probably not a valid value
   };
   enum class shield_rate : uint8_t {
      unchanged =  0,
      drain_in_12_s = 1, // decays from 100% to 0% in 12 seconds
      drain_in_30_s = 2, // decays from 100% to 0% in 30 seconds
      drain_in_60_s = 3, // decays from 100% to 0% in 60 seconds
      value_000 =  4,
      value_010 =  5,
      value_025 =  6,
      value_050 =  7,
      value_075 =  8,
      value_090 =  9,
      value_100 = 10,
      value_110 = 11,
      value_125 = 12,
      value_150 = 13,
      value_200 = 14,
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
      machine_gun_turret = 21, // 400 rounds instead of the normal 200; loadout icon is Mauler
      plasma_cannon    = 22, // 400 rounds instead of the normal 200; loadout icon is Flamethrower
      // 23 // Magnum, but with SMG icon in loadout selection
      // 24 // Magnum, but with no icon in loadout selection
      // ...
      // 32 // Magnum, but with no icon in loadout selection; probably just a default
   };
}

class ReachPlayerTraits {
   public:
      struct {
         cobb::bitnumber<4, reach::damage_resist> damageResist = reach::damage_resist::unchanged;
         cobb::bitnumber<3, uint8_t> healthMult;
         cobb::bitnumber<4, reach::health_rate> healthRate = reach::health_rate::unchanged;
         cobb::bitnumber<3, reach::shield_multiplier> shieldMult = reach::shield_multiplier::unchanged;
         cobb::bitnumber<4, reach::shield_rate> shieldRate = reach::shield_rate::unchanged;
         cobb::bitnumber<4, uint8_t> shieldDelay;
         cobb::bitnumber<2, reach::bool_trait> headshotImmune = reach::bool_trait::unchanged;
         cobb::bitnumber<3, uint8_t> vampirism;
         cobb::bitnumber<2, reach::bool_trait> assassinImmune = reach::bool_trait::unchanged;
         cobb::bitnumber<2, reach::bool_trait> cannotDieFromDamage;
      } defense;
      struct {
         cobb::bitnumber<4, reach::damage_multiplier> damageMult = reach::damage_multiplier::unchanged;
         cobb::bitnumber<4, reach::damage_multiplier> meleeMult  = reach::damage_multiplier::unchanged;
         cobb::bitnumber<8, reach::weapon> weaponPrimary   = reach::weapon::unchanged;
         cobb::bitnumber<8, reach::weapon> weaponSecondary = reach::weapon::unchanged;
         cobb::bitnumber<4, reach::grenade_count> grenadeCount = reach::grenade_count::unchanged;
         cobb::bitnumber<2, reach::infinite_ammo> infiniteAmmo = reach::infinite_ammo::unchanged;
         cobb::bitnumber<2, reach::bool_trait> grenadeRegen = reach::bool_trait::unchanged;
         cobb::bitnumber<2, reach::bool_trait> weaponPickup = reach::bool_trait::unchanged;
         cobb::bitnumber<2, reach::ability_usage> abilityUsage;
         cobb::bitnumber<2, reach::bool_trait> abilitiesDropOnDeath;
         cobb::bitnumber<2, reach::bool_trait> infiniteAbility;
         cobb::bitnumber<8, reach::ability> ability = reach::ability::unchanged;
      } offense;
      struct {
         cobb::bitnumber<5, reach::movement_speed> speed = reach::movement_speed::unchanged;
         cobb::bitnumber<4, reach::player_gravity> gravity = reach::player_gravity::unchanged;
         cobb::bitnumber<4, reach::vehicle_usage> vehicleUsage = reach::vehicle_usage::unchanged;
         cobb::bitnumber<2, uint8_t> unknown;
         cobb::bitnumber<9, int16_t, false, std::true_type, -1> jumpHeight = -1;
      } movement;
      struct {
         cobb::bitnumber<3, reach::active_camo> activeCamo = reach::active_camo::unchanged;
         cobb::bitnumber<2, reach::visible_identity> waypoint    = reach::visible_identity::unchanged;
         cobb::bitnumber<2, reach::visible_identity> visibleName = reach::visible_identity::unchanged;
         cobb::bitnumber<3, reach::aura> aura = reach::aura::unchanged;
         cobb::bitnumber<4, reach::forced_color> forcedColor = reach::forced_color::unchanged;
      } appearance;
      struct {
         cobb::bitnumber<3, reach::radar_state> radarState = reach::radar_state::unchanged;
         cobb::bitnumber<3, reach::radar_range> radarRange = reach::radar_range::unchanged;
         cobb::bitnumber<2, uint8_t> directionalDamageIndicator;
      } sensors;
      //
      void read(cobb::bitreader&) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;
};

class ReachMegaloPlayerTraits : public ReachPlayerTraits {
   public:
      ReachString* name = nullptr;
      ReachString* desc = nullptr;
      MegaloStringIndex nameIndex;
      MegaloStringIndex descIndex;
      //
      void read(cobb::bitreader& stream) noexcept {
         this->nameIndex.read(stream);
         this->descIndex.read(stream);
         ReachPlayerTraits::read(stream);
      }
      void write(cobb::bitwriter& stream) const noexcept {
         this->nameIndex.write(stream);
         this->descIndex.write(stream);
         ReachPlayerTraits::write(stream);
      }
      void postprocess_string_indices(ReachStringTable& table) noexcept {
         this->name = table.get_entry(this->nameIndex);
         this->desc = table.get_entry(this->descIndex);
      }
};