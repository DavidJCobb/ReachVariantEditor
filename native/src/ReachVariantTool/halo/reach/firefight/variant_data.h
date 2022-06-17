#pragma once
#include <array>
#include "../game_variant.h"
#include "../trait_set.h"
#include "./custom_skull.h"
#include "./round.h"
#include "./wave.h"
#include "./wave_traits.h"

namespace halo::reach {
   class firefight_variant_data : public game_variant_data {
      public:
         using lives_count_t = bitnumber<7, int8_t, bitnumber_params<int8_t>{ .offset = 1 }>;
         using score_t = bitnumber<15, uint16_t>;
         
         struct scenario_flag {
            scenario_flag() = delete;
            enum {
               hazards_enabled             = 0x01,
               all_generators_must_survive = 0x02,
               random_generator_spawns     = 0x04,
               weapon_drops_enabled        = 0x08,
               ammo_crates_enabled         = 0x10,
            };
         };

         bitnumber<5,  uint8_t> scenario_flags;
         bitnumber<3,  uint8_t> unk0A;
         bitnumber<8,  uint8_t> wave_limit;
         bitnumber<4,  uint8_t> unk0B;
         score_t unk0C; // a quantity of points; always 10000?
         score_t elite_kill_bonus;
         struct {
            lives_count_t spartan;
            lives_count_t elite;
         } starting_lives;
         score_t unk0D;
         lives_count_t max_spartan_extra_lives;
         bitnumber<2, uint8_t> generator_count;
         struct {
            trait_set spartan;
            trait_set elite;
            firefight::wave_trait_set wave;
         } base_traits;
         std::array<firefight::custom_skull, 3> custom_skulls; // red, yellow, blue
         custom_game_options::respawn elite_respawn_options;
         std::array<firefight::round, 3> rounds;
         struct {
            bitnumber<12, uint16_t> duration; // in seconds
            firefight::round::skull_bitnumber skulls;
            firefight::wave data;
         } bonus_wave;

         virtual void read(bitreader&) override;
         virtual void write(bitwriter&) const override;
   };
}
