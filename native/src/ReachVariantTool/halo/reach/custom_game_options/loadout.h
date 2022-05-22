#pragma once
#include "halo/reach/bitstreams.fwd.h"
#include "halo/reach/traits/ability.h"
#include "halo/reach/traits/weapon.h"

namespace halo::reach::custom_game_options {
   enum class loadout_name : signed int {
      none = -1,
      //
      noble_1 = 0,
      noble_2,
      noble_3,
      noble_4,
      noble_5,
      noble_6,
      carter,
      kat,
      jorge,
      emile,
      jun,
      thom,
      rosenda,
      danny,
      john,
      kelly,
      linda,
      sam,
      kurt,
      jerry,
      jimmy,
      bobby,
      mario,
      omar,
      juggernaut,
      berserker,
      maverick,
      zombie,
      demon,
      angel,
      redshirt,
      ling_ling,
      pookie,
      fng,
      noob,
      camper,
      specter,
      bunker,
      claymore,
      firebase,
      intel,
      ninja,
      power,
      cobra,
      eagle,
      hog,
      bear,
      warrior,
      deceiver,
      ranger,
      zealot,
      royal_zealot,
      assassin,
      dark_assassin,
      champion_assassin, // "Champion"
      gladiator,
      sentry,
      warden,
      saboteur,
      spec_ops,
      scout,
      guard,
      air_assault,
      marksman,
      recon_marksman,  // "Recon"
      expert_marksman, // "Sharpshooter"
      operator_, // "operator" is a keyword in C++
      grenadier,
      medic,
      corpsman,
      stalker,
      demolitions,
      infiltrator,
      security,
      sprint,
      evade,
      armor_lock,
      active_camo,
      hologram,
      drop_shield,
      jet_pack
   };

   struct loadout {
      bitbool visible;
      bitnumber<7, loadout_name, bitnumber_params<loadout_name>{ .initial = loadout_name::none, .presence = false }> name;
      bitnumber<8, traits::weapon>  weapon_primary   = traits::weapon::unchanged;
      bitnumber<8, traits::weapon>  weapon_secondary = traits::weapon::unchanged;
      bitnumber<8, traits::ability> ability = traits::ability::unchanged;
      bitnumber<4, uint8_t> grenade_count;

      void read(bitreader&);
   };
}