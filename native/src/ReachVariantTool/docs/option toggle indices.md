# Known option IDs

## The list

* Most notable flags are concentrated below 100, with a few outliers
* Player traits (all traits?) are in the range [391, 424]
* Spartan loadouts are in the range [1092, 1181]
  * 6 flags per loadout; total 30 per tier
* Flag 1182, between the species loadouts, is unknown
  * Maybe it's the checkbox to mass enable/disable Elite loadouts?
    * In which case flag 1091 would do the same for Spartan loadouts
* Elite loadouts are in the range [1183, 1272]
  * 6 flags per loadout; total 30 per tier

### General

| ID | Name | Section | Notes |
| -: | :- | :- |
| 0 | Score to Win | General
| 1 | Teams Enabled | General
| 2 |
| 3 | Round Time Limit | General
| 4 | Sudden Death Time | General
| 5 | Perfection Enabled | General
| 6 | Number of Rounds | General
| 7 | Rounds to Win | General
| 8 | Synchronize with Team | Respawn
| 9 ||| Might be any of: Respawn with Teammate; Respawn at Location; Respawn on Kills
| 10 ||| Might be any of: Respawn with Teammate; Respawn at Location; Respawn on Kills
| 11 ||| Might be any of: Respawn with Teammate; Respawn at Location; Respawn on Kills
|12|Lives per Round|Respawn
|13|Team Lives per Round|Respawn
|14|Respawn Time|Respawn
|15|Suicide Penalty|Respawn
|16|Betrayal Penalty|Respawn
|17|Respawn Time Growth|Respawn
|18|Loadout Camera Time|Respawn
|19|Respawn Traits Duration|Respawn
|
|56|Team Changing|Social
|57|Friendly Fire|Social
|58|Betrayal Booting|Social
|59|Proximity Voice|Social
|60|Don't Team-Restrict Voice Chat|Social
|61|Allow Dead Players to Talk|Social
|62|Grenades on Map|Map and Game
|63|Abilities on Map|Map and Game
|64|Turrets on Map|Map and Game
|65|Shortcuts on Map|Map and Game
|66|Powerups on Map|Map and Game
|67|Indestructible Vehicles|Map and Game
|
|80|Primary Weapon|Map and Game
|81|Secondary Weapon|Map and Game
|
|103|Weapon Set|Map and Game
|104|Vehicle Set|Map and Game
|
|210|Red Powerup Duration|Map and Game
|211|Blue Powerup Duration|Map and Game
|212|Custom Powerup Duration|Map and Game
|
|391|Damage Resistance|Player Traits, Defense
|392|||"Health Multiplier" player trait?
|393|Health Recharge Rate|Player Traits, Defense
|394|Shield Multiplier|Player Traits, Defense
|395|Shield Recharge Rate|Player Traits, Defense
|396
|397|Headshot Immunity|Player Traits, Defense
|398|Assassination Immunity|Player Traits, Defense
|399
|400
|401|Damage Multiplier|Player Traits, Offense
|402|Melee Multiplier|Player Traits, Offense
|403|Primary Weapon|Player Traits, Offense
|404|Secondary Weapon|Player Traits, Offense
|405|Armor Ability|Player Traits, Offense
|406|Grenade Count|Player Traits, Offense
|407|||"Grenade Regeneration" player trait?
|408|Infinite Ammo|Player Traits, Offense
|409
|410|Weapon Pickup|Player Traits, Offense
|411|Ability Usage|Player Traits, Offense
|412
|413|Movement Speed|Player Traits, Movement
|414|Gravity|Player Traits, Movement
|415|Vehicle Use|Player Traits, Movement
|416|Jump Height|Player Traits, Movement
|417|||"Double Jump" player trait?
|418|Motion Tracker Mode|Player Traits, Sensors
|419|Motion Tracker Range|Player Traits, Sensors
|420|||"Directional Damage Indicator" player trait?
|421|Visible Waypoint|Player Traits, Appearance
|422|Active Camo|Player Traits, Appearance
|423|||"Aura" player trait?
|424|Forced Color|Player Traits, Appearance
|425|Visible Name|Player Traits, Appearance
|
|1092 + 30*x* + 6*y* + 0|Spartan Tier *x* Loadout *y* Hidden|Loadouts
|1092 + 30*x* + 6*y* + 1|Spartan Tier *x* Loadout *y* Name|Loadouts
|1092 + 30*x* + 6*y* + 2|Spartan Tier *x* Loadout *y* Primary Weapon|Loadouts
|1092 + 30*x* + 6*y* + 3|Spartan Tier *x* Loadout *y* Secondary Weapon|Loadouts
|1092 + 30*x* + 6*y* + 4|Spartan Tier *x* Loadout *y* Armor Ability|Loadouts
|1092 + 30*x* + 6*y* + 5|Spartan Tier *x* Loadout *y* Grenade Count|Loadouts
|1182|
|1183 + 30*x* + 6*y* + 0|Elite Tier *x* Loadout *y* Hidden|Loadouts
|1183 + 30*x* + 6*y* + 1|Elite Tier *x* Loadout *y* Name|Loadouts
|1183 + 30*x* + 6*y* + 2|Elite Tier *x* Loadout *y* Primary Weapon|Loadouts
|1183 + 30*x* + 6*y* + 3|Elite Tier *x* Loadout *y* Secondary Weapon|Loadouts
|1183 + 30*x* + 6*y* + 4|Elite Tier *x* Loadout *y* Armor Ability|Loadouts
|1183 + 30*x* + 6*y* + 5|Elite Tier *x* Loadout *y* Grenade Count|Loadouts|Tier 3 Loadout 5 grenade count is Flag 1272

End of list.