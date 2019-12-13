#pragma once
#include <cstdint>
#include "../helpers/bitstream.h"
#include "../helpers/bitnumber.h"

class ReachGameVariantTU1Options {
   public:
      cobb::bitnumber<32, uint32_t> flags;
      float precisionBloom            = 0.7677165354330708F;
      float activeCamoEnergy          = 0.04330708661417323F;
      float activeCamoEnergyBonus     = 0;
      float armorLockDamageDrain      = 1.5551181102362204F;
      float armorLockDamageDrainLimit = 0;
      float magnumDamage              = 2.8149606299212597F;
      float magnumFireRate            = 0.6102362204724409F;
      //
      void read(cobb::bitstream& stream) noexcept {
         this->flags.read(stream);
         this->precisionBloom            = stream.read_compressed_float(8, 0.0F, 10.0F, false, true);
         this->armorLockDamageDrain      = stream.read_compressed_float(8, 0.0F,  2.0F, false, true);
         this->armorLockDamageDrainLimit = stream.read_compressed_float(8, 0.0F,  2.0F, false, true);
         this->activeCamoEnergyBonus     = stream.read_compressed_float(8, 0.0F,  2.0F, false, true);
         this->activeCamoEnergy          = stream.read_compressed_float(8, 0.0F,  2.0F, false, true);
         this->magnumDamage              = stream.read_compressed_float(8, 0.0F, 10.0F, false, true);
         this->magnumFireRate            = stream.read_compressed_float(8, 0.0F, 10.0F, false, true);
      }
};