#pragma once
#include <cstdint>
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitreader.h"
#include "../../helpers/bitwriter.h"

enum class ReachTU1Flags : uint8_t {
   enable_bleed_through = 0x01,
   armor_lock_cant_shed_stickies = 0x02,
   armor_lock_can_be_stuck       = 0x04,
   enable_active_camo_modifiers  = 0x08,
   limit_sword_block_to_sword    = 0x10,
   enable_automatic_magnum       = 0x20,
   unused_6                      = 0x40,
   unused_7                      = 0x80,
};
class ReachGameVariantTU1Options {
   public:
      static constexpr float vanilla_precision_bloom = 5.01968479F; // 100%
      static constexpr float patched_precision_bloom = 4.23228359F; //  85%
      //
      static constexpr float vanilla_active_camo_energy = 0.0669191317F; // TU doesn't change this
      //
      static constexpr float vanilla_active_camo_energy_bonus = 0.0196850393F;
      static constexpr float patched_active_camo_energy_bonus = 0.0354330689F;
      //
      static constexpr float vanilla_armor_lock_damage_drain       = 0.0F;
      static constexpr float patched_armor_lock_damage_drain       = 0.3F;
      static constexpr float vanilla_armor_lock_damage_drain_limit = 0.0F;
      static constexpr float patched_armor_lock_damage_drain_limit = 0.4F;
      //
      static constexpr float vanilla_magnum_damage     = 1.0F; // patched changes this to like 0.999 which is probably an unintentional side-effect of how floats are encoded
      static constexpr float vanilla_magnum_fire_delay = 1.0F; // TU doesn't change this

      cobb::bitnumber<32, uint32_t> flags;
      float precisionBloom            = vanilla_precision_bloom;
      float activeCamoEnergy          = vanilla_active_camo_energy;
      float activeCamoEnergyBonus     = vanilla_active_camo_energy_bonus; // drain rate?
      float armorLockDamageDrain      = vanilla_armor_lock_damage_drain;
      float armorLockDamageDrainLimit = vanilla_armor_lock_damage_drain_limit;
      float magnumDamage              = vanilla_magnum_damage;
      float magnumFireDelay           = vanilla_magnum_fire_delay;
      //
      void read(cobb::bitreader& stream) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;

      void make_vanilla() noexcept;
      void make_patched() noexcept;
      void make_anniversary() noexcept;
      void make_zero_bloom() noexcept {
         this->make_patched();
         this->precisionBloom = 0.0F;
      }
      //
      #if __cplusplus <= 201703L
      bool operator==(const ReachGameVariantTU1Options&) const noexcept;
      bool operator!=(const ReachGameVariantTU1Options& other) const noexcept { return !(*this == other); }
      #else
      bool operator==(const ReachGameVariantTU1Options&) const noexcept = default;
      bool operator!=(const ReachGameVariantTU1Options&) const noexcept = default;
      #endif
};