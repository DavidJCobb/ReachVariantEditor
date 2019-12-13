#pragma once
#include <array>
#include "player_traits.h"

namespace reach {
   enum class loadout_palette {
      spartan_tier_1,
      spartan_tier_2,
      spartan_tier_3,
      elite_tier_1,
      elite_tier_2,
      elite_tier_3,
   };
};

class ReachLoadout {
   public:
      cobb::bitbool visible;
      cobb::bitnumber<7, int8_t, false, 0, std::false_type, -1> nameIndex = -1;
      cobb::bitnumber<8, reach::weapon> weaponPrimary   = reach::weapon::unchanged;
      cobb::bitnumber<8, reach::weapon> weaponSecondary = reach::weapon::unchanged;
      cobb::bitnumber<8, reach::ability> ability = reach::ability::unchanged;
      cobb::bitnumber<4, uint8_t> grenadeCount;
      //
      void read(cobb::bitstream&) noexcept;
};
class ReachLoadoutPalette {
   public:
      std::array<ReachLoadout, 5> loadouts;
      //
      void read(cobb::bitstream& stream) noexcept {
         for (size_t i = 0; i < this->loadouts.size(); i++)
            this->loadouts[i].read(stream);
      }
};