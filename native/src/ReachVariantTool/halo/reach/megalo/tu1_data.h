#pragma once
#include "halo/reach/bitstreams.fwd.h"
#include "halo/bitfloat.h"

namespace halo::reach {
   struct tu1_data {
      public:
         struct flag {
            flag() = delete;
            enum {
               enable_bleed_through          = 0x01,
               armor_lock_cant_shed_stickies = 0x02,
               armor_lock_can_be_stuck       = 0x04,
               enable_active_camo_modifiers  = 0x08,
               limit_sword_block_to_sword    = 0x10,
               enable_automatic_magnum       = 0x20,
               unused_6 = 0x40,
               unused_7 = 0x80,
            };
         };

      public:
         static constexpr float vanilla_precision_bloom = 1.00;
         static constexpr float patched_precision_bloom = 0.85;
         //
         static constexpr float vanilla_active_camo_energy_curve_min = 0.07F;
         static constexpr float patched_active_camo_energy_curve_min = vanilla_active_camo_energy_curve_min; // TU doesn't change this
         //
         static constexpr float vanilla_active_camo_energy_curve_max = 0.02;
         static constexpr float patched_active_camo_energy_curve_max = 0.035;
         //
         static constexpr float vanilla_armor_lock_damage_drain       = 0.0F;
         static constexpr float patched_armor_lock_damage_drain       = 0.3F;
         static constexpr float vanilla_armor_lock_damage_drain_limit = 0.0F;
         static constexpr float patched_armor_lock_damage_drain_limit = 0.4F;
         //
         static constexpr float vanilla_magnum_damage     = 1.0F; // patched changes this to like 0.999 which is an unintentional side-effect of how floats are encoded
         static constexpr float vanilla_magnum_fire_delay = 1.0F; // TU doesn't change this

         using float_02x = bitfloat<bitfloat_params{
            .bitcount = 8,
            .guarantee_exact_bounds = true,
            .is_signed = false,
            .min = 0,
            .max = 2,
         }>;
         using float_10x = bitfloat<bitfloat_params{
            .bitcount = 8,
            .guarantee_exact_bounds = true,
            .is_signed = false,
            .min = 0,
            .max = 10,
         }>;

      public:
         bitnumber<32, uint32_t> flags;
         float_02x precision_bloom = vanilla_precision_bloom;
         struct {
            float_02x min = vanilla_active_camo_energy_curve_min;
            float_02x max = vanilla_active_camo_energy_curve_max;
         } active_camo_energy_curve;
         struct {
            float_02x base  = vanilla_armor_lock_damage_drain;
            float_02x limit = vanilla_armor_lock_damage_drain_limit;
         } armor_lock_damage_drain;
         struct {
            float_10x damage     = vanilla_magnum_damage;
            float_10x fire_delay = vanilla_magnum_fire_delay;
         } magnum;
         
         void read(bitreader&);
         void write(bitwriter&) const;

         void make_vanilla();
         void make_patched();
         void make_anniversary();
         void make_zero_bloom() {
            this->make_patched();
            this->precision_bloom = 0.0F;
         }

         bool is_vanilla() const; // true if all values are within format margin of error of vanilla settings


   };
}