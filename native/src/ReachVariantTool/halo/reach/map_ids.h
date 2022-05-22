#pragma once
#include <array>
#include <cstdint>

namespace halo::reach {
   using map_id = uint16_t;

   namespace map_ids {
      constexpr map_id sword_base     = 1000;
      constexpr map_id sword_base_dlc = 1001; // unused
      constexpr map_id countdown      = 1020;
      constexpr map_id boardwalk      = 1035;
      constexpr map_id zealot         = 1040;
      constexpr map_id powerhouse     = 1055;
      constexpr map_id powerhouse_dlc = 1056; // unused
      constexpr map_id boneyard       = 1080;
      constexpr map_id reflection     = 1150;
      constexpr map_id spire          = 1200;
      constexpr map_id condemned      = 1500;
      constexpr map_id highlands      = 1510;
      constexpr map_id anchor_9       = 2001;
      constexpr map_id breakpoint     = 2002;
      constexpr map_id tempest        = 2004;
      constexpr map_id forge_world    = 3006;

      // A player with modded MAPINFOs can host Custom Games on Campaign and Firefight maps, 
      // and players without modded MAPs can join those.
      constexpr std::array<map_id, 11> campaign_missions = {
         5005, // NOBLE Actual
         5010, // Winter Contingency
         5020, // ONI: Sword Base
         5030, // Nightfall
         5035, // Tip of the Spear
         5045, // Long Night of Solace
         5050, // Exodus
         5052, // New Alexandria
         5060, // The Package
         5070, // The Pillar of Autumn
         5080, // Lone Wolf
      };

      constexpr map_id ff_overlook   = 7000;
      constexpr map_id ff_courtyard  = 7020;
      constexpr map_id ff_outpost    = 7030;
      constexpr map_id ff_waterfront = 7040;
      constexpr map_id ff_beachhead  = 7060;
      constexpr map_id ff_holdout    = 7080;
      constexpr map_id ff_corvette   = 7110;
      constexpr map_id ff_glacier    = 7130;
      constexpr map_id ff_unearthed  = 7500;

      constexpr map_id penance       = 10010;
      constexpr map_id battle_canyon = 10020;
      constexpr map_id ridgeline     = 10030;
      constexpr map_id breakneck     = 10050;
      constexpr map_id high_noon     = 10060;
      constexpr map_id solitary      = 10070;

      constexpr map_id ff_installation_04 = 10080;
   }
}
