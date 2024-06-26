#include "object_types.h"

namespace Megalo {
   namespace enums {
      DetailedEnum object_type = DetailedEnum({
         DetailedEnumValue("spartan",
            DetailedEnumValueInfo::make_friendly_name("Spartan Biped"),
            DetailedEnumValueInfo::make_map_tag('bipd', "objects/characters/spartans/spartans")
         ),
         DetailedEnumValue("elite",
            DetailedEnumValueInfo::make_friendly_name("Elite Biped"),
            DetailedEnumValueInfo::make_map_tag('bipd', "objects/characters/elite/elite")
         ),
         DetailedEnumValue("monitor",
            DetailedEnumValueInfo::make_friendly_name("Monitor Biped"),
            DetailedEnumValueInfo::make_map_tag('bipd', "objects/characters/monitor/monitor_editor")
         ),
         DetailedEnumValue("flag",
            DetailedEnumValueInfo::make_friendly_name("Flag"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/multiplayer/flag/flag"),
            DetailedEnumValueInfo::make_description("A flag. It can be picked up by a player, carried, and used as a melee weapon.")
         ),
         DetailedEnumValue("bomb",
            DetailedEnumValueInfo::make_friendly_name("Bomb"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/multiplayer/assault_bomb/assault_bomb"),
            DetailedEnumValueInfo::make_description("A UNSC bomb. It can be picked up by a player, carried, and used as a melee weapon.")
         ),
         DetailedEnumValue("skull",
            DetailedEnumValueInfo::make_friendly_name("Skull"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/multiplayer/skull/skull"),
            DetailedEnumValueInfo::make_description("A skull. It can be picked up by a player, carried, and used as a melee weapon.")
         ),
         DetailedEnumValue("hill_marker",
            DetailedEnumValueInfo::make_friendly_name("Hill Marker"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/multi/models/mp_hill_beacon/mp_hill_beacon"),
            DetailedEnumValueInfo::make_description("An invisible hill marker, of the same variety as those placeable in Forge.")
         ),
         DetailedEnumValue("flag_stand",
            DetailedEnumValueInfo::make_friendly_name("Flag Stand"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/multi/models/mp_flag_base/mp_flag_base"),
            DetailedEnumValueInfo::make_description("A flag stand.")
         ),
         DetailedEnumValue("capture_plate",
            DetailedEnumValueInfo::make_friendly_name("Capture Plate"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/multi/models/mp_circle/mp_circle"),
            DetailedEnumValueInfo::make_description("A thick, olive drab plate of metal. It is typically used as a spawn or arming point for bombs.")
         ),
         DetailedEnumValue("frag_grenade",
            DetailedEnumValueInfo::make_friendly_name("Frag Grenade"),
            DetailedEnumValueInfo::make_map_tag('eqip', "objects/weapons/grenade/frag_grenade/frag_grenade")
         ),
         DetailedEnumValue("plasma_grenade",
            DetailedEnumValueInfo::make_friendly_name("Plasma Grenade"),
            DetailedEnumValueInfo::make_map_tag('eqip', "objects/weapons/grenade/plasma_grenade/plasma_grenade")
         ),
         DetailedEnumValue("spike_grenade",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Spike Grenade")
         ),
         DetailedEnumValue("firebomb_grenade",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Firebomb Grenade")
         ),
         DetailedEnumValue("dmr",
            DetailedEnumValueInfo::make_friendly_name("DMR"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/rifle/dmr/dmr")
         ),
         DetailedEnumValue("assault_rifle",
            DetailedEnumValueInfo::make_friendly_name("Assault Rifle"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/rifle/assault_rifle/assault_rifle")
         ),
         DetailedEnumValue("plasma_pistol",
            DetailedEnumValueInfo::make_friendly_name("Plasma Pistol"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/pistol/plasma_pistol/plasma_pistol")
         ),
         DetailedEnumValue("spiker",
            DetailedEnumValueInfo::make_friendly_name("Spiker"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/rifle/spike_rifle/spike_rifle")
         ),
         DetailedEnumValue("smg",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("SMG")
         ),
         DetailedEnumValue("needle_rifle",
            DetailedEnumValueInfo::make_friendly_name("Needle Rifle"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/rifle/needle_rifle/needle_rifle")
         ),
         DetailedEnumValue("plasma_repeater",
            DetailedEnumValueInfo::make_friendly_name("Plasma Repeater"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/rifle/plasma_repeater/plasma_repeater")
         ),
         DetailedEnumValue("energy_sword",
            DetailedEnumValueInfo::make_friendly_name("Energy Sword"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/melee/energy_sword/energy_sword")
         ),
         DetailedEnumValue("magnum",
            DetailedEnumValueInfo::make_friendly_name("Magnum"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/pistol/magnum/magnum")
         ),
         DetailedEnumValue("needler",
            DetailedEnumValueInfo::make_friendly_name("Needler"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/pistol/needler/needler")
         ),
         DetailedEnumValue("plasma_rifle",
            DetailedEnumValueInfo::make_friendly_name("Plasma Rifle"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/rifle/plasma_rifle/plasma_rifle")
         ),
         DetailedEnumValue("rocket_launcher",
            DetailedEnumValueInfo::make_friendly_name("Rocket Launcher"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/support_high/rocket_launcher/rocket_launcher")
         ),
         DetailedEnumValue("shotgun",
            DetailedEnumValueInfo::make_friendly_name("Shotgun"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/rifle/shotgun/shotgun")
         ),
         DetailedEnumValue("sniper_rifle",
            DetailedEnumValueInfo::make_friendly_name("Sniper Rifle"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/rifle/sniper_rifle/sniper_rifle")
         ),
         DetailedEnumValue("brute_shot",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Brute Shot")
         ),
         DetailedEnumValue("beam_rifle", // actually spawns a Focus Rifle
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Beam Rifle"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/rifle/focus_rifle/focus_rifle"),
            DetailedEnumValueInfo::make_description("This value seems to have been intended for Beam Rifles, but now it refers to Focus Rifles.")
         ),
         DetailedEnumValue("spartan_laser",
            DetailedEnumValueInfo::make_friendly_name("Spartan Laser"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/support_high/spartan_laser/spartan_laser")
         ),
         DetailedEnumValue("gravity_hammer",
            DetailedEnumValueInfo::make_friendly_name("Gravity Hammer"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/melee/gravity_hammer/gravity_hammer")
         ),
         DetailedEnumValue("mauler",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Mauler")
         ),
         DetailedEnumValue("flamethrower",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Flamethrower")
         ),
         DetailedEnumValue("missile_pod",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Missile Pod")
         ),
         DetailedEnumValue("warthog",
            DetailedEnumValueInfo::make_friendly_name("Warthog"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/human/warthog/warthog"),
            DetailedEnumValueInfo::make_description("A Warthog. Use a variant string ID to select a specific variety of Warthog to spawn.")
         ),
         DetailedEnumValue("ghost",
            DetailedEnumValueInfo::make_friendly_name("Ghost"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/covenant/ghost/ghost")
         ),
         DetailedEnumValue("scorpion",
            DetailedEnumValueInfo::make_friendly_name("Scorpion"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/human/scorpion/scorpion")
         ),
         DetailedEnumValue("wraith",
            DetailedEnumValueInfo::make_friendly_name("Wraith"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/covenant/wraith/wraith")
         ),
         DetailedEnumValue("banshee",
            DetailedEnumValueInfo::make_friendly_name("Banshee"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/covenant/banshee/banshee")
         ),
         DetailedEnumValue("mongoose",
            DetailedEnumValueInfo::make_friendly_name("Mongoose"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/human/mongoose/mongoose")
         ),
         DetailedEnumValue("chopper",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Chopper")
         ),
         DetailedEnumValue("prowler",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Prowler")
         ),
         DetailedEnumValue("hornet",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Hornet")
         ),
         DetailedEnumValue("stingray",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Stingray"),
            DetailedEnumValueInfo::make_map_tag('vehi', "")
         ),
         DetailedEnumValue("heavy_wraith",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Wraith (Heavy)"),
            DetailedEnumValueInfo::make_map_tag('vehi', "")
         ),
         DetailedEnumValue("falcon",
            DetailedEnumValueInfo::make_friendly_name("Falcon"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/human/falcon/falcon")
         ),
         DetailedEnumValue("sabre",
            DetailedEnumValueInfo::make_flags(DetailedEnumValueInfo::flags::is_thorage),
            DetailedEnumValueInfo::make_friendly_name("Sabre"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/human/sabre/sabre"),
            DetailedEnumValueInfo::make_description("An UNSC space vehicle capable of housing a single occupant. Sabres are armed with a chaingun and lock-on missiles, and can perform evasive maneuvers similarly to a Banshee. In addition, they can rotate about the roll axis, allowing them to turn sideways or upside-down mid-flight. This maneuverability comes at a cost, however: like Banshees, Sabres have a constant forward thrust, but a Sabre moves faster and takes damage upon any impact with the environment.")
         ),
         DetailedEnumValue("sprint",
            DetailedEnumValueInfo::make_friendly_name("Sprint"),
            DetailedEnumValueInfo::make_map_tag('eqip', "objects/equipment/sprint/sprint")
         ),
         DetailedEnumValue("jetpack",
            DetailedEnumValueInfo::make_friendly_name("Jetpack"),
            DetailedEnumValueInfo::make_map_tag('eqip', "objects/equipment/jet_pack/jet_pack")
         ),
         DetailedEnumValue("armor_lock",
            DetailedEnumValueInfo::make_friendly_name("Armor Lock"),
            DetailedEnumValueInfo::make_map_tag('eqip', "objects/equipment/armor_lockup/armor_lockup")
         ),
         DetailedEnumValue("power_fist",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Unused (Power Fist AA)"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("active_camo_aa",
            DetailedEnumValueInfo::make_friendly_name("Active Camo (Armor Ability)"),
            DetailedEnumValueInfo::make_map_tag('eqip', "objects/equipment/active_camouflage/active_camouflage")
         ),
         DetailedEnumValue("ammo_pack",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Unused (Ammo Pack AA)"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("sensor_pack",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Unused (Sensor Pack AA)")
         ),
         DetailedEnumValue("revenant",
            DetailedEnumValueInfo::make_friendly_name("Revenant"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/covenant/revenant/revenant")
         ),
         DetailedEnumValue("pickup_truck",
            DetailedEnumValueInfo::make_flags(DetailedEnumValueInfo::flags::is_thorage),
            DetailedEnumValueInfo::make_friendly_name("Pickup Truck"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/human/civilian/pickup/pickup"),
            DetailedEnumValueInfo::make_description("A truck with two front seats and a large truck bed. This is the first land vehicle seen in Reach's campaign, in the Winter Contingency mission.")
         ),
         DetailedEnumValue("focus_rifle",
            DetailedEnumValueInfo::make_friendly_name("Focus Rifle"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/rifle/focus_rifle/focus_rifle")
         ),
         DetailedEnumValue("territory_static", // Halo 3 leftover from before Forge labels were invented
            DetailedEnumValueInfo::make_friendly_name("Scrapped Territory Marker (Halo 3 leftover)"),
            DetailedEnumValueInfo::make_map_tag('bloc', ""),
            DetailedEnumValueInfo::make_description("A leftover from Halo 3's development, available only on Powerhouse. It has no inherent function, and reuses the Flag Stand model.")
         ),
         DetailedEnumValue("ctf_flag_return_area", // Halo 3 leftover from before Forge labels were invented
            DetailedEnumValueInfo::make_friendly_name("Scrapped Flag Return Marker (Halo 3 leftover)"),
            DetailedEnumValueInfo::make_map_tag('bloc', ""),
            DetailedEnumValueInfo::make_description("A leftover from Halo 3's development, available only on Powerhouse. It has no inherent function, and reuses the Flag Stand model.")
         ),
         DetailedEnumValue("ctf_flag_spawn_point", // Halo 3 leftover from before Forge labels were invented
            DetailedEnumValueInfo::make_friendly_name("Scrapped Flag Spawn Marker (Halo 3 leftover)"),
            DetailedEnumValueInfo::make_map_tag('bloc', ""),
            DetailedEnumValueInfo::make_description("A leftover from Halo 3's development, available only on Powerhouse. It has no inherent function, and reuses the Flag Stand model.")
         ),
         DetailedEnumValue("respawn_zone",
            DetailedEnumValueInfo::make_friendly_name("Respawn Zone"),
            DetailedEnumValueInfo::make_map_tag('scen', "objects/multi/spawning/respawn_zone"),
            DetailedEnumValueInfo::make_description("A respawn zone marker, of the same variety as those placeable in Forge.")
         ),
         DetailedEnumValue("invasion_elite_buy",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("invasion_elite_buy"),
            DetailedEnumValueInfo::make_map_tag('bloc', "")
         ),
         DetailedEnumValue("invasion_elite_drop",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("invasion_elite_drop"),
            DetailedEnumValueInfo::make_map_tag('bloc', "")
         ),
         DetailedEnumValue("invasion_slayer",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("invasion_slayer"),
            DetailedEnumValueInfo::make_map_tag('bloc', "")
         ),
         DetailedEnumValue("invasion_spartan_buy",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("invasion_spartan_buy"),
            DetailedEnumValueInfo::make_map_tag('bloc', "")
         ),
         DetailedEnumValue("invasion_spartan_drop",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("invasion_spartan_drop"),
            DetailedEnumValueInfo::make_map_tag('bloc', "")
         ),
         DetailedEnumValue("invasion_spawn_controller",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("invasion_spawn_controller"),
            DetailedEnumValueInfo::make_map_tag('bloc', "")
         ),
         DetailedEnumValue("oddball_ball_spawn_point", // Halo 3 leftover from before Forge labels were invented
            DetailedEnumValueInfo::make_friendly_name("Scrapped Oddball Spawn Point (Halo 3 leftover)"),
            DetailedEnumValueInfo::make_map_tag('bloc', ""),
            DetailedEnumValueInfo::make_description("A leftover from Halo 3's development, available only on Powerhouse. It has no inherent function, and reuses the Flag Stand model.")
         ),
         DetailedEnumValue("plasma_launcher",
            DetailedEnumValueInfo::make_friendly_name("Plasma Launcher"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/support_high/plasma_launcher/plasma_launcher")
         ),
         DetailedEnumValue("fusion_coil",
            DetailedEnumValueInfo::make_friendly_name("Fusion Coil"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/gear/human/military/fusion_coil/fusion_coil")
         ),
         DetailedEnumValue("unsc_shield_generator",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("unsc_shield_generator"),
            DetailedEnumValueInfo::make_map_tag('scen', "")
         ),
         DetailedEnumValue("cov_shield_generator",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("cov_shield_generator"),
            DetailedEnumValueInfo::make_map_tag('scen', "")
         ),
         DetailedEnumValue("initial_spawn_point",
            DetailedEnumValueInfo::make_friendly_name("Initial Spawn Point"),
            DetailedEnumValueInfo::make_map_tag('scen', "objects/multi/spawning/initial_spawn_point")
         ),
         DetailedEnumValue("invasion_vehicle_req",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("invasion_vehicle_req"),
            DetailedEnumValueInfo::make_map_tag('bloc', "")
         ),
         DetailedEnumValue("vehicle_req_floor",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("vehicle_req_floor"),
            DetailedEnumValueInfo::make_map_tag('mach', "")
         ),
         DetailedEnumValue("wall_switch",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("wall_switch"),
            DetailedEnumValueInfo::make_map_tag('ctrl', "")
         ),
         DetailedEnumValue("health_pack",
            DetailedEnumValueInfo::make_friendly_name("Health Pack"),
            DetailedEnumValueInfo::make_map_tag('ctrl', "objects/levels/shared/device_controls/health_station/health_station")
         ),
         DetailedEnumValue("req_unsc_laser",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("req_unsc_laser"),
            DetailedEnumValueInfo::make_map_tag('mach', "")
         ),
         DetailedEnumValue("req_unsc_dmr",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("req_unsc_dmr"),
            DetailedEnumValueInfo::make_map_tag('mach', "")
         ),
         DetailedEnumValue("req_unsc_rocket",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("req_unsc_rocket"),
            DetailedEnumValueInfo::make_map_tag('mach', "")
         ),
         DetailedEnumValue("req_unsc_shotgun",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("req_unsc_shotgun"),
            DetailedEnumValueInfo::make_map_tag('mach', "")
         ),
         DetailedEnumValue("req_unsc_sniper",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("req_unsc_sniper"),
            DetailedEnumValueInfo::make_map_tag('mach', "")
         ),
         DetailedEnumValue("req_covy_launcher",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("req_covy_launcher"),
            DetailedEnumValueInfo::make_map_tag('mach', "")
         ),
         DetailedEnumValue("req_covy_needler",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("req_covy_needler"),
            DetailedEnumValueInfo::make_map_tag('mach', "")
         ),
         DetailedEnumValue("req_covy_sniper",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("req_covy_sniper"),
            DetailedEnumValueInfo::make_map_tag('mach', "")
         ),
         DetailedEnumValue("req_covy_sword",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("req_covy_sword"),
            DetailedEnumValueInfo::make_map_tag('mach', "")
         ),
         DetailedEnumValue("shock_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("shock_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("specialist_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("specialist_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("assassin_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("assassin_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("infiltrator_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("infiltrator_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("warrior_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("warrior_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("combatant_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("combatant_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("engineer_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("engineer_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("infantry_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("infantry_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("operator_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("operator_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("recon_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("recon_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("scout_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("scout_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("seeker_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("seeker_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("airborne_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("airborne_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("ranger_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("ranger_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("req_buy_banshee",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("req_buy_banshee"),
            DetailedEnumValueInfo::make_map_tag('ctrl', "")
         ),
         DetailedEnumValue("req_buy_falcon",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("req_buy_falcon"),
            DetailedEnumValueInfo::make_map_tag('ctrl', "")
         ),
         DetailedEnumValue("req_buy_ghost",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("req_buy_ghost"),
            DetailedEnumValueInfo::make_map_tag('ctrl', "")
         ),
         DetailedEnumValue("req_buy_mongoose",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("req_buy_mongoose"),
            DetailedEnumValueInfo::make_map_tag('ctrl', "")
         ),
         DetailedEnumValue("req_buy_revenant",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("req_buy_revenant"),
            DetailedEnumValueInfo::make_map_tag('ctrl', "")
         ),
         DetailedEnumValue("req_buy_scorpion",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("req_buy_scorpion"),
            DetailedEnumValueInfo::make_map_tag('ctrl', "")
         ),
         DetailedEnumValue("req_buy_warthog",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("req_buy_warthog"),
            DetailedEnumValueInfo::make_map_tag('ctrl', "")
         ),
         DetailedEnumValue("req_buy_wraith",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("req_buy_wraith"),
            DetailedEnumValueInfo::make_map_tag('ctrl', "")
         ),
         DetailedEnumValue("fireteam_1_respawn_zone",
            DetailedEnumValueInfo::make_friendly_name("Respawn Zone, Fireteam 1"),
            DetailedEnumValueInfo::make_map_tag('scen', ""),
            DetailedEnumValueInfo::make_description("A special type of Respawn Zone defined on Boneyard. It can be seen in Forge on maps like Boneyard, but not edited.")
         ),
         DetailedEnumValue("fireteam_2_respawn_zone",
            DetailedEnumValueInfo::make_friendly_name("Respawn Zone, Fireteam 2"),
            DetailedEnumValueInfo::make_map_tag('scen', ""),
            DetailedEnumValueInfo::make_description("A special type of Respawn Zone defined on Boneyard. It can be seen in Forge on maps like Boneyard, but not edited.")
         ),
         DetailedEnumValue("fireteam_3_respawn_zone",
            DetailedEnumValueInfo::make_friendly_name("Respawn Zone, Fireteam 3"),
            DetailedEnumValueInfo::make_map_tag('scen', ""),
            DetailedEnumValueInfo::make_description("A special type of Respawn Zone defined on Boneyard. It can be seen in Forge on maps like Boneyard, but not edited.")
         ),
         DetailedEnumValue("fireteam_4_respawn_zone",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Respawn Zone, Fireteam 4"),
            DetailedEnumValueInfo::make_map_tag('scen', "")
         ),
         DetailedEnumValue("semi_truck",
            DetailedEnumValueInfo::make_flags(DetailedEnumValueInfo::flags::is_thorage),
            DetailedEnumValueInfo::make_friendly_name("Semi Truck"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/human/civilian/truck_cab_large/truck_cab_large")
         ),
         DetailedEnumValue("soccer_ball",
            DetailedEnumValueInfo::make_friendly_name("Soccer Ball"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/shared/soccer_ball/soccer_ball"),
            DetailedEnumValueInfo::make_description("A giant soccer ball, of the same variety as those placeable in Forge.")
         ),
         DetailedEnumValue("golf_ball",
            DetailedEnumValueInfo::make_friendly_name("Golf Ball"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/shared/golf_ball/golf_ball"),
            DetailedEnumValueInfo::make_description("A giant golf ball, of the same variety as those placeable in Forge.")
         ),
         DetailedEnumValue("golf_ball_blue",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Golf Ball, Blue"),
            DetailedEnumValueInfo::make_map_tag('bloc', "")
         ),
         DetailedEnumValue("golf_ball_red",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Golf Ball, Red"),
            DetailedEnumValueInfo::make_map_tag('bloc', "")
         ),
         DetailedEnumValue("golf_club",
            DetailedEnumValueInfo::make_friendly_name("Golf Club"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/levels/shared/golf_club/golf_club")
         ),
         DetailedEnumValue("golf_cup",
            DetailedEnumValueInfo::make_friendly_name("Golf Cup"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/shared/golf_cup/golf_cup")
         ),
         DetailedEnumValue("golf_tee",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Golf Tee"),
            DetailedEnumValueInfo::make_map_tag('bloc', "")
         ),
         DetailedEnumValue("dice",
            DetailedEnumValueInfo::make_friendly_name("Dice"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/multi/dice/dice"),
            DetailedEnumValueInfo::make_description("A giant six-sided die, of the same variety as those placeable in Forge. It is a muted green with white dots.")
         ),
         DetailedEnumValue("covenant_crate",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/props/covenant/crate_space/crate_space"),
            DetailedEnumValueInfo::make_description("A Covenant weapon crate. This tag is only defined on some Firefight maps; Multiplayer maps use a different tag.")
         ),
         DetailedEnumValue("eradicator_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("eradicator_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("saboteur_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("saboteur_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("grenadier_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("grenadier_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("marksman_loadout",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("marksman_loadout"),
            DetailedEnumValueInfo::make_map_tag('eqip', "")
         ),
         DetailedEnumValue("flare",
            DetailedEnumValueInfo::make_unused_sentinel(), // not defined on Forge World or Boneyard
            DetailedEnumValueInfo::make_friendly_name("Flare"),
            DetailedEnumValueInfo::make_map_tag('bloc', "")
         ),
         DetailedEnumValue("glow_stick",
            DetailedEnumValueInfo::make_unused_sentinel(), // not defined on Forge World or Boneyard
            DetailedEnumValueInfo::make_friendly_name("Glowstick"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/props/human/unsc/unsc_glow_stick/unsc_glow_stick"),
            DetailedEnumValueInfo::make_description("Defined on some Firefight maps.")
         ),
         DetailedEnumValue("concussion_rifle",
            DetailedEnumValueInfo::make_friendly_name("Concussion Rifle"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/rifle/concussion_rifle/concussion_rifle")
         ),
         DetailedEnumValue("grenade_launcher",
            DetailedEnumValueInfo::make_friendly_name("Grenade Launcher"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/rifle/grenade_launcher/grenade_launcher")
         ),
         DetailedEnumValue("phantom_approach",
            DetailedEnumValueInfo::make_friendly_name("phantom_approach"),
            DetailedEnumValueInfo::make_map_tag('mach', "")
         ),
         DetailedEnumValue("hologram",
            DetailedEnumValueInfo::make_friendly_name("Hologram"),
            DetailedEnumValueInfo::make_map_tag('eqip', "objects/equipment/hologram/hologram")
         ),
         DetailedEnumValue("evade",
            DetailedEnumValueInfo::make_friendly_name("Evade"),
            DetailedEnumValueInfo::make_map_tag('eqip', "objects/equipment/evade/evade")
         ),
         DetailedEnumValue("unsc_data_core",
            DetailedEnumValueInfo::make_friendly_name("UNSC Data Core"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/multiplayer/unsc_data_core/unsc_data_core"),
            DetailedEnumValueInfo::make_description("A UNSC data core, used as the \"capture the flag\" objective in Invasion: Boneyard. It can be picked up and carried by a player, reducing their speed and preventing them from attacking.")
         ),
         DetailedEnumValue("danger_zone",
            DetailedEnumValueInfo::make_friendly_name("Danger Zone"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/multi/spawning/danger_zone"),
            DetailedEnumValueInfo::make_description("Analysis of this object's map tag is inconclusive but suggests that it influences spawning when a shape is set on it. It doesn't really matter, though, because only Tempest has the Megalo object tag list specify the tag as of this writing.")
         ),
         DetailedEnumValue("teleporter_sender",
            DetailedEnumValueInfo::make_friendly_name("Halo 3 Teleporter, Sender"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/multi/teleporter_sender/teleporter_sender"),
            DetailedEnumValueInfo::make_description("A Halo 3 teleporter (sender node) available only on Boardwalk. It can be spawned via Megalo, and is interoperable with Halo: Reach teleporters on channel Alpha.")
         ),
         DetailedEnumValue("teleporter_receiver",
            DetailedEnumValueInfo::make_friendly_name("Halo 3 Teleporter, Receiver"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/multi/teleporter_receiver/teleporter_receiver"),
            DetailedEnumValueInfo::make_description("A Halo 3 teleporter (receiver node) available only on Boardwalk. It can be spawned via Megalo, and is interoperable with Halo: Reach teleporters on channel Alpha.")
         ),
         DetailedEnumValue("teleporter_2way",
            DetailedEnumValueInfo::make_unused_sentinel(), // Bungie typo'd the definition
            DetailedEnumValueInfo::make_friendly_name("Halo 3 Teleporter, Two-Way"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/multi/teleporter_2way/teleporter_2way"),
            DetailedEnumValueInfo::make_description("A Halo 3 teleporter (two-way node). The object definition exists in the game files, but it isn't available on any map, because Bungie typed in the wrong internal name and path when setting it up.")
         ),
         DetailedEnumValue("data_core_beam",
            DetailedEnumValueInfo::make_friendly_name("data_core_beam"),
            DetailedEnumValueInfo::make_map_tag('mach', "")
         ),
         DetailedEnumValue("phantom_overwatch",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("phantom_overwatch"),
            DetailedEnumValueInfo::make_map_tag('mach', "")
         ),
         DetailedEnumValue("longsword",
            DetailedEnumValueInfo::make_friendly_name("Longsword"),
            DetailedEnumValueInfo::make_map_tag('mach', ""),
            DetailedEnumValueInfo::make_description("A relatively low-detail static model of a Longsword bomber. The tag is defined on Boneyard.")
         ),
         DetailedEnumValue("particle_emitter_fire",
            DetailedEnumValueInfo::make_friendly_name("Particle Emitter (Fire)"),
            DetailedEnumValueInfo::make_map_tag('scen', "objects/multi/invisible_cube_of_derek/invisible_cube_of_derek"),
            DetailedEnumValueInfo::make_description("An invisible object which emits fire particles. This is purely a graphical effect and does no damage on its own.")
         ),
         DetailedEnumValue("phantom_scenery",
            DetailedEnumValueInfo::make_flags(DetailedEnumValueInfo::flags::is_thorage),
            DetailedEnumValueInfo::make_friendly_name("Phantom (Scenery)"),
            DetailedEnumValueInfo::make_map_tag('scen', "")
         ),
         DetailedEnumValue("pelican_scenery",
            DetailedEnumValueInfo::make_flags(DetailedEnumValueInfo::flags::is_thorage),
            DetailedEnumValueInfo::make_friendly_name("Pelican (Scenery)"),
            DetailedEnumValueInfo::make_map_tag('scen', "")
         ),
         DetailedEnumValue("phantom",
            DetailedEnumValueInfo::make_flags(DetailedEnumValueInfo::flags::is_thorage),
            DetailedEnumValueInfo::make_friendly_name("Phantom (Vehicle)"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/covenant/phantom/phantom_mp")
         ),
         DetailedEnumValue("pelican",
            DetailedEnumValueInfo::make_friendly_name("Pelican (Device)"),
            DetailedEnumValueInfo::make_map_tag('mach', "The animated device machine used on Spire.")
         ),
         DetailedEnumValue("armory_shelf",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Armory Shelf"),
            DetailedEnumValueInfo::make_map_tag('scen', ""),
            DetailedEnumValueInfo::make_description("A military-style shelving unit defined only on the Campaign level \"The Package.\"")
         ),
         DetailedEnumValue("covenant_resupply_capsule",
            DetailedEnumValueInfo::make_friendly_name("cov_resupply_capsule"),
            DetailedEnumValueInfo::make_map_tag('scen', "")
         ),
         DetailedEnumValue("covenant_drop_pod",
            DetailedEnumValueInfo::make_friendly_name("covy_drop_pod"),
            DetailedEnumValueInfo::make_map_tag('mach', "objects/levels/multi/70_boneyard/covy_drop_pod/covy_drop_pod"),
            DetailedEnumValueInfo::make_description("An unfinished asset available on Boneyard. It is a small, untextured (i.e. flat grey) cylinder with an upward taper, about the diameter of a player model. It has no collision and no apparent function.")
         ),
         DetailedEnumValue("invisible_marker",
            DetailedEnumValueInfo::make_friendly_name("Scrapped Invisible Marker"),
            DetailedEnumValueInfo::make_map_tag('bloc', ""),
            DetailedEnumValueInfo::make_description("Defined on Penance, Powerhouse, Reflection, Sword Base, and Zealot. It resembles a flag stand when viewed in the HREK, but is defined to be invisible outside of Forge.")
         ),
         DetailedEnumValue("respawn_zone_weak",
            DetailedEnumValueInfo::make_friendly_name("Respawn Zone (Weak)"),
            DetailedEnumValueInfo::make_map_tag('scen', "objects/multi/spawning/weak_respawn_zone"),
            DetailedEnumValueInfo::make_description("A respawn zone with a weak influence, of the same variety as those placeable in Forge.")
         ),
         DetailedEnumValue("respawn_zone_weak_anti",
            DetailedEnumValueInfo::make_friendly_name("Respawn Zone (Weak Anti)"),
            DetailedEnumValueInfo::make_map_tag('scen', "objects/multi/spawning/weak_anti_respawn_zone"),
            DetailedEnumValueInfo::make_description("A respawn zone with a weak negative influence, of the same variety as those placeable in Forge.")
         ),
         DetailedEnumValue("phantom_device",
            DetailedEnumValueInfo::make_friendly_name("Phantom (Device)"),
            DetailedEnumValueInfo::make_map_tag('mach', "objects/vehicles/covenant/phantom/phantom")
         ),
         DetailedEnumValue("resupply_capsule",
            DetailedEnumValueInfo::make_friendly_name("Resupply Capsule"),
            DetailedEnumValueInfo::make_map_tag('scen', "objects/gear/human/military/resupply_capsule/resupply_capsule"),
            DetailedEnumValueInfo::make_description("A drop pod for a single weapon, like those seen in Firefight; as of this writing, the object is only available on Anchor 9. Unfortunately, the pod is sealed, and there doesn't seem to be any way to open it. Its tag data suggests that it opens when it reaches 0% health, implying that Firefight opens it by using scripts to \"kill\" it; however, attempting to do the same with Megalo scripts produces no result.")
         ),
         DetailedEnumValue("resupply_capsule_open",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("resupply_capsule_open"),
            DetailedEnumValueInfo::make_map_tag('scen', ""),
            DetailedEnumValueInfo::make_description("This tag is not defined even in Firefight maps, which define the \"closed\" variant. Examination of the \"closed\" variant's tags suggest that an explicit \"open\" variant shouldn't even be necessary.")
         ),
         DetailedEnumValue("weapon_box",
            DetailedEnumValueInfo::make_friendly_name("weapon_box"),
            DetailedEnumValueInfo::make_map_tag('scen', "objects/gear/human/military/weapon_box/weapon_box"),
            DetailedEnumValueInfo::make_description("Defined on some Firefight maps, including Courtyard. It is a flat olive drab metal panel with UNSC branding that in Firefight can spawn with power weapons attached to it.")
         ),
         DetailedEnumValue("tech_console_stationary",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("tech_console_stationary"),
            DetailedEnumValueInfo::make_map_tag('mach', "")
         ),
         DetailedEnumValue("tech_console_wall",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("tech_console_wall"),
            DetailedEnumValueInfo::make_map_tag('mach', "")
         ),
         DetailedEnumValue("initial_loadout_camera",
            DetailedEnumValueInfo::make_friendly_name("Initial Loadout Camera"),
            DetailedEnumValueInfo::make_map_tag('scen', "objects/multi/generic/mp_cinematic_camera")
         ),
         DetailedEnumValue("invisible_covenant_resupply_capsule",
            DetailedEnumValueInfo::make_friendly_name("invis_cov_resupply_capsule"),
            DetailedEnumValueInfo::make_map_tag('mach', "objects/levels/shared/device_machines/invis_cob_resupply_capsule/invis_cov_resully_capsule"),
            DetailedEnumValueInfo::make_description("This tag is defined on Boneyard, and inspection with third-party tools indicates that it takes the form of an invisible cube. Its function has yet to be tested in-game.")
         ),
         DetailedEnumValue("covenant_power_core",
            DetailedEnumValueInfo::make_friendly_name("Covenant Power Core"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/multiplayer/cov_power_module/cov_power_module")
         ),
         DetailedEnumValue("fuel_rod_gun",
            DetailedEnumValueInfo::make_friendly_name("Fuel Rod Gun"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/support_high/flak_cannon/flak_cannon")
         ),
         DetailedEnumValue("dropzone_boundary",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("dropzone_boundary"),
            DetailedEnumValueInfo::make_map_tag('bloc', "")
         ),
         DetailedEnumValue("shield_door_small",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("shield_door_small"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/props/covenant/shield_door_small/shield_door_small"),
            DetailedEnumValueInfo::make_description("Defined on some Firefight maps, including Courtyard.")
         ),
         DetailedEnumValue("shield_door_medium",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("shield_door_medium"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/props/covenant/shield_door_medium/shield_door_medium"),
            DetailedEnumValueInfo::make_description("Defined on some Firefight maps, including Courtyard.")
         ),
         DetailedEnumValue("shield_door_large",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("shield_door_large"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/props/covenant/shield_door_large/shield_door_large"),
            DetailedEnumValueInfo::make_description("Defined on some Firefight maps, including Courtyard.")
         ),
         DetailedEnumValue("drop_shield",
            DetailedEnumValueInfo::make_friendly_name("Drop Shield (Armor Ability)"),
            DetailedEnumValueInfo::make_map_tag('eqip', "objects/equipment/drop_shield/drop_shield")
         ),
         DetailedEnumValue("detached_machine_gun_turret",
            DetailedEnumValueInfo::make_friendly_name("Machine Gun Turret (Detached)"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/vehicles/human/turrets/machinegun/weapon/machinegun_turret/machinegun_turret"),
            DetailedEnumValueInfo::make_description("An infantry-operable turret that has been detached from its stand. It has a finite ammo supply and cannot be refilled. It can be carried in addition to a primary and secondary weapon, but will slow its user's movement.")
         ),
         DetailedEnumValue("machine_gun_turret",
            DetailedEnumValueInfo::make_friendly_name("Machine Gun Turret"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/human/turrets/machinegun/machinegun"),
            DetailedEnumValueInfo::make_description("An infantry-operable turret mounted on a stand. While mounted, the turret has unlimited ammo but cannot be moved; if the stand is broken, the turret can be carried but will have a finite ammo supply.")
         ),
         DetailedEnumValue("detached_plasma_cannon",
            DetailedEnumValueInfo::make_friendly_name("Plasma Cannon (Detached)"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/vehicles/covenant/turrets/plasma_turret/weapon/plasma_turret/plasma_turret"),
            DetailedEnumValueInfo::make_description("An infantry-operable turret that has been detached from its stand. It has a finite ammo supply and cannot be refilled. It can be carried in addition to a primary and secondary weapon, but will slow its user's movement.")
         ),
         DetailedEnumValue("plasma_cannon",
            DetailedEnumValueInfo::make_friendly_name("Plasma Cannon"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/covenant/turrets/plasma_turret/plasma_turret_mounted"),
            DetailedEnumValueInfo::make_description("An infantry-operable turret mounted on a stand. While mounted, the turret has unlimited ammo but cannot be moved; if the stand is broken, the turret can be carried but will have a finite ammo supply.")
         ),
         DetailedEnumValue("shade",
            DetailedEnumValueInfo::make_friendly_name("Shade Turret"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/covenant/turrets/shade/shade")
         ),
         DetailedEnumValue("cargo_truck",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("Cargo Truck"),
            DetailedEnumValueInfo::make_map_tag('vehi', "")
         ),
         DetailedEnumValue("electric_cart",
            DetailedEnumValueInfo::make_flags(DetailedEnumValueInfo::flags::is_thorage),
            DetailedEnumValueInfo::make_friendly_name("Electric Cart"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/human/civilian/cart_electric/cart_electric"),
            DetailedEnumValueInfo::make_description("An electric cart with no armaments.")
         ),
         DetailedEnumValue("forklift",
            DetailedEnumValueInfo::make_flags(DetailedEnumValueInfo::flags::is_thorage),
            DetailedEnumValueInfo::make_friendly_name("Forklift"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/human/civilian/forklift/forklift")
         ),
         DetailedEnumValue("military_truck",
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_friendly_name("military_truck"),
            DetailedEnumValueInfo::make_map_tag('vehi', "")
         ),
         DetailedEnumValue("oni_van",
            DetailedEnumValueInfo::make_flags(DetailedEnumValueInfo::flags::is_thorage),
            DetailedEnumValueInfo::make_friendly_name("ONI Van"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/human/civilian/oni_van/oni_van"),
            DetailedEnumValueInfo::make_description("A huge black van with a driver and passenger seat. It is fairly slow.")
         ),
         DetailedEnumValue("warthog_turret",
            DetailedEnumValueInfo::make_friendly_name("Warthog Turret (Normal)"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/human/warthog/weapons/warthog_chaingun/warthog_chaingun"),
            DetailedEnumValueInfo::make_description("A Warthog rear chaingun, separate from the Warthog itself.")
         ),
         DetailedEnumValue("warthog_turret_gauss",
            DetailedEnumValueInfo::make_friendly_name("Warthog Turret (Gauss)"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/human/warthog/weapons/warthog_gauss/warthog_gauss"),
            DetailedEnumValueInfo::make_description("A Warthog rear gauss turret, separate from the Warthog itself.")
         ),
         DetailedEnumValue("warthog_turret_rocket",
            DetailedEnumValueInfo::make_friendly_name("Warthog Turret (Rocket)"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/human/warthog/weapons/warthog_rocket/warthog_rocket"),
            DetailedEnumValueInfo::make_description("A Warthog rear rocket turret, separate from the Warthog itself.")
         ),
         DetailedEnumValue("scorpion_turret_anti_infantry",
            DetailedEnumValueInfo::make_friendly_name("Scorpion Turret (Chaingun)"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/human/scorpion/turrets/scorpion_anti_infantry/scorpion_anti_infantry")
         ),
         DetailedEnumValue("falcon_turret_grenade_left",
            DetailedEnumValueInfo::make_friendly_name("Falcon Turret (Grenade Left)"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/human/falcon/turrets/falcon_side_grenade_left/falcon_side_grenade_left")
         ),
         DetailedEnumValue("falcon_turret_grenade_right",
            DetailedEnumValueInfo::make_friendly_name("Falcon Turret (Grenade Right)"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/human/falcon/turrets/falcon_side_grenade_right/falcon_side_grenade_right")
         ),
         DetailedEnumValue("wraith_turret_anti_infantry",
            DetailedEnumValueInfo::make_friendly_name("Wraith Turret (Anti-Infantry)"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/covenant/wraith/turrets/wraith_anti_infantry/wraith_anti_infantry")
         ),
         DetailedEnumValue("landmine",
            DetailedEnumValueInfo::make_friendly_name("Land Mine"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/multi/land_mine/land_mine")
         ),
         DetailedEnumValue("target_locator",
            DetailedEnumValueInfo::make_friendly_name("Target Locator"),
            DetailedEnumValueInfo::make_map_tag('weap', "objects/weapons/pistol/target_laser/target_laser"),
            DetailedEnumValueInfo::make_description("A Target Locator capable of calling down a devastating air strike. This weapon will only spawn on maps that define its tag.")
         ),
         DetailedEnumValue("kill_boundary_3d_model", // "ff_kill_zone" // internally, this tag name is used for the (Soft) Kill Boundary's 3D model, but not for the actual object... and that tag in specific isn't even specified anyway
            DetailedEnumValueInfo::make_unused_sentinel(),
            DetailedEnumValueInfo::make_map_tag('bloc', "")
         ),
         DetailedEnumValue("block_1x1_flat",
            DetailedEnumValueInfo::make_friendly_name("Block 1x1 Flat"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/forge/ff_plat_1x1_flat/ff_plat_1x1_flat"),
            DetailedEnumValueInfo::make_description("A Block 1x1 Flat, of the same variety as those placeable in Forge.")
         ),
         DetailedEnumValue("shade_gun_anti_air",
            DetailedEnumValueInfo::make_friendly_name("Shade Turret Gun (Anti-Air)"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/covenant/turrets/shade/weapons/shade_anti_air_cannon/shade_anti_air_cannon"),
            DetailedEnumValueInfo::make_description("The \"gun\" part of a Shade turret; the base is not included. This variant fires devastatingly powerful shots at a high rate. Shields protect its operator.")
         ),
         DetailedEnumValue("shade_gun_fuel_rod",
            DetailedEnumValueInfo::make_friendly_name("Shade Turret Gun (Fuel Rod)"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/covenant/turrets/shade/weapons/shade_flak_cannon/shade_flak_cannon"),
            DetailedEnumValueInfo::make_description("The \"gun\" part of a Shade turret; the base is not included. This variant fires fuel rods.")
         ),
         DetailedEnumValue("shade_gun_plasma",
            DetailedEnumValueInfo::make_friendly_name("Shade Turret Gun (Plasma)"),
            DetailedEnumValueInfo::make_map_tag('vehi', "objects/vehicles/covenant/turrets/shade/weapons/shade_plasma_cannon/shade_plasma_cannon"),
            DetailedEnumValueInfo::make_description("The \"gun\" part of a Shade turret; the base is not included.")
         ),
         DetailedEnumValue("kill_ball",
            DetailedEnumValueInfo::make_friendly_name("Kill Ball"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/shared/damage_sphere/damage_sphere")
         ),
         DetailedEnumValue("light_red",
            DetailedEnumValueInfo::make_friendly_name("Forge Light (Red)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/forge/ff_light_red/ff_light_red")
         ),
         DetailedEnumValue("light_blue",
            DetailedEnumValueInfo::make_friendly_name("Forge Light (Blue)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/forge/ff_light_blue/ff_light_blue")
         ),
         DetailedEnumValue("light_green",
            DetailedEnumValueInfo::make_friendly_name("Forge Light (Green)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/forge/ff_light_green/ff_light_green")
         ),
         DetailedEnumValue("light_orange",
            DetailedEnumValueInfo::make_friendly_name("Forge Light (Orange)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/forge/ff_light_orange/ff_light_orange")
         ),
         DetailedEnumValue("light_purple",
            DetailedEnumValueInfo::make_friendly_name("Forge Light (Purple)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/forge/ff_light_purple/ff_light_purple")
         ),
         DetailedEnumValue("light_yellow",
            DetailedEnumValueInfo::make_friendly_name("Forge Light (Yellow)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/forge/ff_light_yellow/ff_light_yellow")
         ),
         DetailedEnumValue("light_white",
            DetailedEnumValueInfo::make_friendly_name("Forge Light (White)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/forge/ff_light_white/ff_light_white")
         ),
         DetailedEnumValue("light_red_flashing",
            DetailedEnumValueInfo::make_friendly_name("Forge Light (Flashing Red)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/forge/ff_light_flash_red/ff_light_flash_red")
         ),
         DetailedEnumValue("light_yellow_flashing",
            DetailedEnumValueInfo::make_friendly_name("Forge Light (Flashing Yellow)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/forge/ff_light_flash_yellow/ff_light_flash_yellow")
         ),
         DetailedEnumValue("fx_colorblind",
            DetailedEnumValueInfo::make_friendly_name("FX (Colorblind)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/shared/screen_fx_orb/fx/colorblind")
         ),
         DetailedEnumValue("fx_gloomy",
            DetailedEnumValueInfo::make_friendly_name("FX (Gloomy)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/shared/screen_fx_orb/fx/gloomy")
         ),
         DetailedEnumValue("fx_juicy",
            DetailedEnumValueInfo::make_friendly_name("FX (Juicy)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/shared/screen_fx_orb/fx/juicy")
         ),
         DetailedEnumValue("fx_nova",
            DetailedEnumValueInfo::make_friendly_name("FX (Nova)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/shared/screen_fx_orb/fx/nova")
         ),
         DetailedEnumValue("fx_olde_timey",
            DetailedEnumValueInfo::make_friendly_name("FX (Olde Timey)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/shared/screen_fx_orb/fx/olde_timey")
         ),
         DetailedEnumValue("fx_pen_and_ink",
            DetailedEnumValueInfo::make_friendly_name("FX (Pen And Ink)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/shared/screen_fx_orb/fx/pen_and_ink")
         ),
         DetailedEnumValue("fx_purple",
            DetailedEnumValueInfo::make_friendly_name("FX (Purple)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/shared/screen_fx_orb/fx/dusk"),
            DetailedEnumValueInfo::make_description("A Forge filter that covers the screen in a deep purple overlay, similar to the lighting in the Nightfall mission.")
         ),
         DetailedEnumValue("fx_orange",
            DetailedEnumValueInfo::make_friendly_name("FX (Orange)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/shared/screen_fx_orb/fx/golden_hour")
         ),
         DetailedEnumValue("fx_green",
            DetailedEnumValueInfo::make_friendly_name("FX (Green)"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/shared/screen_fx_orb/fx/eerie"),
            DetailedEnumValueInfo::make_description("A Forge filter that covers the screen in a grimy, green overlay, with small visual distortions away from the center. This filter affects the UI, and it can stack; spawning too many of it will blind all players.")
         ),
         DetailedEnumValue("grid",
            DetailedEnumValueInfo::make_friendly_name("Grid"),
            DetailedEnumValueInfo::make_map_tag('bloc', "objects/levels/forge/ff_grid_object/ff_grid_object"),
            DetailedEnumValueInfo::make_description("A glowing blue platform with a grid texture, of the same variety as those placeable in Forge. The platform is square, and each side is 30 Forge units long; on Tempest, a single grid placed at the center of the map can reach from one base's frontmost man cannon to the other base's frontmost man cannon.")
         ),
         DetailedEnumValue("sound_emitter_alarm_1",
            DetailedEnumValueInfo::make_friendly_name("Invisible Cube of Alarming 1"),
            DetailedEnumValueInfo::make_map_tag('scen', "objects/multi/invisible_cube_of_derek/invisible_cube_of_alarming_1"),
            DetailedEnumValueInfo::make_description("An invisible object which emits an alarm sound. The emitter may be silent by default; call set_scenario_interpolator_state(1, 1) to make it audible, and set_scenario_interpolator_state(1, 0) to silence it again.")
         ),
         DetailedEnumValue("sound_emitter_alarm_2",
            DetailedEnumValueInfo::make_friendly_name("Invisible Cube of Alarming 2"),
            DetailedEnumValueInfo::make_map_tag('scen', "objects/multi/invisible_cube_of_derek/invisible_cube_of_alarming_2"),
            DetailedEnumValueInfo::make_description("An invisible object which emits an alarm sound. The emitter may be silent by default; call set_scenario_interpolator_state(2, 1) to make it audible, and set_scenario_interpolator_state(2, 0) to silence it again.")
         ),
         DetailedEnumValue("safe_boundary",
            DetailedEnumValueInfo::make_friendly_name("Safe Boundary"),
            DetailedEnumValueInfo::make_map_tag('scen', "objects/multi/boundaries/safe_volume"),
            DetailedEnumValueInfo::make_description("A Safe Boundary, of the same variety as those placeable in Forge. Players who remain outside of all extant Safe Boundaries will be killed.")
         ),
         DetailedEnumValue("soft_safe_boundary",
            DetailedEnumValueInfo::make_friendly_name("Soft Safe Boundary"),
            DetailedEnumValueInfo::make_map_tag('scen', "objects/multi/boundaries/soft_safe_volume"),
            DetailedEnumValueInfo::make_description("A Soft Safe Boundary, of the same variety as those placeable in Forge. Players who remain outside of all extant Soft Safe Boundaries for longer than ten seconds will be killed.")
         ),
         DetailedEnumValue("kill_boundary",
            DetailedEnumValueInfo::make_friendly_name("Kill Boundary"),
            DetailedEnumValueInfo::make_map_tag('scen', "objects/multi/boundaries/kill_volume"),
            DetailedEnumValueInfo::make_description("A Kill Boundary, of the same variety as those placeable in Forge. Players who enter its shape will be killed instantly.")
         ),
         DetailedEnumValue("soft_kill_boundary",
            DetailedEnumValueInfo::make_friendly_name("Soft Kill Boundary"),
            DetailedEnumValueInfo::make_map_tag('scen', "objects/multi/boundaries/soft_kill_volume"),
            DetailedEnumValueInfo::make_description("A Soft Kill Boundary, of the same variety as those placeable in Forge. Players who remain inside of its shape for longer than ten seconds will be killed.")
         ),
         DetailedEnumValue("unsc_data_core_holder",
            DetailedEnumValueInfo::make_friendly_name("UNSC Data Core Holder"),
            DetailedEnumValueInfo::make_map_tag('mach', "objects/multi/package_cabinet/package_cabinet"),
            DetailedEnumValueInfo::make_description("The Invasion: Boneyard prop which holds the UNSC Data Core used as an objective in Phase 3.")
         ),
         DetailedEnumValue("covenant_power_module_stand",
            DetailedEnumValueInfo::make_friendly_name("Covenant Power Module Stand"),
            DetailedEnumValueInfo::make_map_tag('mach', "objects/props/covenant/cov_powermodule_stand/cov_powermodule_stand"),
            DetailedEnumValueInfo::make_description("The Invasion: Spire prop linked to the Phase 1 territories.")
         ),
         DetailedEnumValue("covenant_bomb",
            DetailedEnumValueInfo::make_friendly_name("Covenant Bomb"),
            DetailedEnumValueInfo::make_map_tag('weap', "levels/multi/dlc/objects/dlc_covenant_bomb/dlc_covenant_bomb"),
            DetailedEnumValueInfo::make_description("The Covenant bomb used in Invasion: Breakpoint. Like the UNSC bomb, it can be picked up and carried.")
         ),
         DetailedEnumValue("heavy_barrier",
            DetailedEnumValueInfo::make_friendly_name("Heavy Barrier"),
            DetailedEnumValueInfo::make_map_tag('bloc', "levels/multi/dlc/objects/dlc_invasion_heavy_shield/dlc_invasion_heavy_shield"),
            DetailedEnumValueInfo::make_description("A massive steel barricade. This object was originally only available on Breakpoint, but the Thorage update has made it usable on Forge World.")
         ),
         DetailedEnumValue("breakpoint_bomb_door",
            DetailedEnumValueInfo::make_friendly_name("dlc_invasion_bomb_door"),
            DetailedEnumValueInfo::make_map_tag('scen', "levels/multi/dlc/objects/dlc_invasion_bomb_door/dlc_invasion_bomb_door"),
            DetailedEnumValueInfo::make_description("A massive metal wall. It is usable on Forge World, but as of this writing I'm not sure yet whether it will be included in Thorage.")
         ),
      });
   }
}