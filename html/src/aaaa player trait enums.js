let traits = {
   damage: {
      damage_resist: {
         unchanged:   0,
         value_0010:  1,
         value_0050:  2,
         value_0090:  3,
         value_0100:  4,
         value_0110:  5,
         value_0150:  6,
         value_0200:  7,
         value_0300:  8,
         value_0500:  9,
         value_1000: 10,
         value_2000: 11,
         invulnerable: 12,
      },
      shield_mult: {
         unchanged:  0,
         value_0000: 1,
         value_0100: 2,
         value_0150: 3,
         value_0200: 4,
         value_0300: 5,
         value_0400: 6,
      },
      shield_rate: {
         unchanged:   0,
         // 1
         // 2
         // 3
         value_0000:  4,
         value_0010:  5,
         value_0025:  6,
         value_0050:  7,
         value_0075:  8,
         value_0090:  9,
         value_0100: 10,
         value_0110: 11,
         value_0125: 12,
         value_0150: 13,
         value_0200: 14,
      },
      shield_delay: {
         unchanged: 0,
      },
      health_rate: {
         unchanged:  0,
         // 1
         // 2
         // 3
         value_0000: 4,
         value_0050: 5,
         value_0090: 6,
         value_0100: 7,
         value_0110: 8,
         value_0200: 9,
      },
      assassin_immune: {
         unchanged: 0,
         disabled:  1,
         enabled:   2,
      },
      headshot_immune: {
         unchanged: 0,
         disabled:  1,
         enabled:   2,
      },
      vampirism: {
         unknown_0: 0, // default
      },
      unk09: {
         unknown_0: 0, // default
      },
   },
   weapons: {
      damage_mult: {
         unchanged:   0,
         value_0000:  1,
         value_0025:  2,
         value_0050:  3,
         value_0075:  4,
         value_0090:  5,
         value_0100:  6,
         value_0110:  7,
         value_0125:  8,
         value_0150:  9,
         value_0200: 10,
         value_0300: 11,
      },
      melee_mult: {
         unchanged:   0,
         value_0000:  1,
         value_0025:  2,
         value_0050:  3,
         value_0075:  4,
         value_0090:  5,
         value_0100:  6,
         value_0110:  7,
         value_0125:  8,
         value_0150:  9,
         value_0200: 10,
         value_0300: 11,
      },
      weapon: {
         random:           -4,
         unchanged:        -3, // default
         map_default:      -2,
         none:             -1, // UI doesn't allow this for primary weapon
         dmr:               0,
         assault_rifle:     1,
         plasma_pistol:     2,
         spiker:            3,
         energy_sword:      4,
         magnum:            5,
         needler:           6,
         plasma_rifle:      7,
         rocket_launcher:   8,
         shotgun:           9,
         sniper_rifle:     10,
         spartan_laser:    11,
         gravity_hammer:   12,
         plasma_repeater:  13,
         needle_rifle:     14,
         focus_rifle:      15,
         plasma_launcher:  16,
         concussion_rifle: 17,
         grenade_launcher: 18,
         golf_club:        19,
         fuel_rod_gun:     20,
      },
      ability: {
         unchanged: -3,
         // map_default == -2 ? no such setting exists in the UI
         none:       -1,
         sprint:      0,
         jetpack:     1,
         armor_lock:  2,
         // 3
         active_camo: 4,
         // 5
         // 6
         hologram:    7,
         evade:       8,
         drop_shield: 9,
      },
      grenade_count: {
         unchanged:   0,
         map_default: 1,
         none:        2,
         frag_1:      3,
         frag_2:      4,
         frag_3:      5,
         frag_4:      6,
         plasma_1:    7,
         plasma_2:    8,
         plasma_3:    9,
         plasma_4:   10,
         each_1:     11,
         each_2:     12,
         each_3:     13,
         each_4:     14,
      },
      infinite_ammo: {
         unchanged:  0,
         disabled:   1,
         enabled:    2,
         bottomless: 3,
      },
      weapon_pickup: {
         unchanged: 0,
         disabled:  1,
         enabled:   2,
      },
      ability_usage: { // actually ability pickup. lumped in with another setting (which?)
         //
         // ability drop, infinite ability, and ability usage are consecutive. 
         // are we sure they're even different fields? "infinite ability" is 
         // always zero, while this field and drop seem to vary with each other.
         //
         unknown_0: 0, // default
         unknown_1: 1,
         unlimited: 2, // unlimited ability usage
      },
      ability_drop: { // actually ability usage.
         unchanged: 0,
         unknown_1: 1, // actually disabled?
         default:   2,
         enabled:   3,
      },
      grenade_regen: {
         unknown_0: 0, // default
         // 1?
         // 2?
         // 3?
      },
   },
   movement: {
      speed: { // 5 bits
         unchanged:   0,
         value_0000:  1,
         value_0025:  2,
         value_0050:  3,
         value_0075:  4,
         value_0090:  5,
         value_0100:  6,
         value_0110:  7,
         value_0120:  8,
         //  9
         // 10
         value_0150: 11,
         // 12
         // 13
         // 14
         // 15
         value_0200: 16,
         value_0300: 17,
      },
      gravity: {
         unchanged:  0,
         value_0050: 1,
         value_0075: 2,
         value_0100: 3,
         value_0150: 4,
         value_0200: 5,
      },
      vehicle_usage: {
         unchanged:      0,
         none:           1,
         passenger_only: 2,
         driver_only:    3,
         gunner_only:    4,
         no_passenger:   5,
         no_driver:      6,
         no_gunner:      7,
         full_use:       8,
      },
      unknown: {
         unknown_0: 0, // default
      },
      jump_height: {
         unchanged:  -1,
         value_000:   0,
         value_025:  25,
         value_050:  50,
         value_075:  75,
         value_090:  90,
         value_100: 100,
         value_110: 110,
         value_125: 125,
         value_150: 150,
         value_200: 200,
         value_300: 300,
      },
   },
   appearance: {
      active_camo: {
         unchanged: 0,
         off:       1,
         // 2
         poor:      3,
         good:      4,
      },
      waypoint: {
         unchanged: 0,
         none:      1,
         allies:    2,
         everyone:  3,
      },
      visible_name: {
         unchanged: 0,
         none:      1,
         allies:    2,
         everyone:  3,
      },
      forced_color: {
         unchanged:  0,
         none:       1,
         red:        2,
         blue:       3,
         green:      4,
         orange:     5,
         purple:     6,
         gold:       7,
         brown:      8,
         pink:       9,
         white:     10,
         black:     11,
         zombie:    12,
      },
   },
   sensors: {
      radar_state: {
         unchanged: 0,
         off:       1,
         allies:    2,
         normal:    3,
         enhanced:  4,
      },
      radar_range: {
         unchanged:  0,
         meters_010: 1,
         // 2
         meters_025: 3,
         // 4
         meters_075: 5,
         meters_150: 6,
      },
      directional_damage_indicator: {
         unknown_0: 0, // default
      },
   },
};