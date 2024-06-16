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

Rows marked with † indicate IDs that were found by reverse-engineering how MCC and Halo: Reach talk to each other: the two places have separate enums for Custom Game options, and there exists a function to translate between them.

Rows marked with ‡ indicate IDs found by reverse-engineering how Reach builds menus at run-time, but not yet tested.

When an option's name is formatted `like_this`, it is a string of the form `$halo_game_settings_like_this`.

**EDIT:** I went to all the trouble to RE this just to find that it's in the HREK: it's the "parameter" enumeration on any `text_value_pair_definition` for custom games.

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
|80|Primary Weapon|Map and Game|Base Player Traits?
|81|Secondary Weapon|Map and Game|Base Player Traits?
|
|103|Weapon Set|Map and Game
|104|Vehicle Set|Map and Game
|
|154|||Would probably map to Editor Traits, Offense
|155|||Would probably map to Editor Traits, Movement
|
|210|Red Powerup Duration|Map and Game
|211|Blue Powerup Duration|Map and Game
|212|Custom Powerup Duration|Map and Game
|
|216|Damage Resistance|Forge, Editor Traits, Defense|‡
|217
|218
|219|Shield Multiplier|Forge, Editor Traits, Defense|‡
|220|Shield Recharge Rate|Forge, Editor Traits, Defense|‡
|
|243|Motion Tracker Mode|Forge, Editor Traits, Appearance|‡
|244|Motion Tracker Range|Forge, Editor Traits, Appearance|‡
|245
|246|Player Waypoint|Forge, Editor Traits, Appearance|‡
|247|Active Camo|Forge, Editor Traits, Appearance|‡
|
|391|Damage Resistance|All Player Traits, Defense
|392|||"Health Multiplier" player trait?
|393|Health Recharge Rate|All Player Traits, Defense
|394|Shield Multiplier|All Player Traits, Defense
|395|Shield Recharge Rate|All Player Traits, Defense
|396
|397|Headshot Immunity|All Player Traits, Defense
|398|Assassination Immunity|All Player Traits, Defense
|399
|400
|401|Damage Multiplier|All Player Traits, Offense
|402|Melee Multiplier|All Player Traits, Offense
|403|Primary Weapon|All Player Traits, Offense
|404|Secondary Weapon|All Player Traits, Offense
|405|Armor Ability|All Player Traits, Offense
|406|Grenade Count|All Player Traits, Offense
|407|||"Grenade Regeneration" player trait?
|408|Infinite Ammo|All Player Traits, Offense
|409
|410|Weapon Pickup|All Player Traits, Offense
|411|Ability Usage|All Player Traits, Offense
|412
|413|Movement Speed|All Player Traits, Movement
|414|Gravity|All Player Traits, Movement
|415|Vehicle Use|All Player Traits, Movement
|416|Jump Height|All Player Traits, Movement
|417|||"Double Jump" player trait?
|418|Motion Tracker Mode|All Player Traits, Sensors
|419|Motion Tracker Range|All Player Traits, Sensors
|420|||"Directional Damage Indicator" player trait?
|421|Visible Waypoint|All Player Traits, Appearance
|422|Active Camo|All Player Traits, Appearance
|423|||"Aura" player trait?
|424|Forced Color|All Player Traits, Appearance
|425|Visible Name|All Player Traits, Appearance
|426|Vision|Firefight, All Wave Traits|‡
|427|Hearing|Firefight, All Wave Traits|‡
|428|Luck|Firefight, All Wave Traits|‡
|429|Shootiness|Firefight, All Wave Traits|‡
|430|Grenades|Firefight, All Wave Traits|‡
|431|`traits_ai_equipment_drop`|Firefight, All Wave Traits|‡
|432|Assassination Immunity|Firefight, All Wave Traits|‡
|433|Headshot Immunity|Firefight, All Wave Traits|‡
|434|Damage Resistance|Firefight, All Wave Traits|‡
|435|Damage Modifier|Firefight, All Wave Traits|‡
|436|Map Hazards|Firefight|†
|437|Weapon Drops|Firefight|†
|438|Ammo Crates|Firefight|†
|439|`generator_defend_all`|Firefight|†‡
|440|`generator_random_spawn`|Firefight|†‡
|441
|442|`firefight_ends_after`|Firefight|†‡
|
|445|Elite Kill Bonus|Firefight|†
|
|449|Max Lives|Firefight|†
|450|Generator Count|Firefight|†
|
|536|`firefight_biped_lives`|Firefight|†
|
|579|Skull: Black Eye|Firefight, Round 1|‡
|580|Skull: Tough Luck|Firefight, Round 1|‡
|581|Skull: Catch|Firefight, Round 1|‡
|582|Skull: Fog|Firefight, Round 1|‡
|583|Skull: Famine|Firefight, Round 1|‡
|584
|585|Skull: Tilt|Firefight, Round 1|‡
|586|Skull: Mythic|Firefight, Round 1|‡
|587
|588
|589|Skull: Cowbell|Firefight, Round 1|‡
|590|Skull: Grunt Birthday Party|Firefight, Round 1|‡
|591|Skull: IWHBYD|Firefight, Round 1|‡
|592|Skull: Red|Firefight, Round 1|‡
|593|Skull: Yellow|Firefight, Round 1|‡
|594|Skull: Blue|Firefight, Round 1|‡
|595
|596|Dropships|Firefight, Round 1 Initial Wave|†‡
|597|Selection|Firefight, Round 1 Initial Wave|†‡
|598|Squad Type 1|Firefight, Round 1 Initial Wave|†‡
|599|Squad Type 2|Firefight, Round 1 Initial Wave|†‡
|600|Squad Type 3|Firefight, Round 1 Initial Wave|†‡
|
|610|Dropships|Firefight, Round 1 Main Wave|†‡
|611|Selection|Firefight, Round 1 Main Wave|†‡
|612|Squad Type 1|Firefight, Round 1 Main Wave|†‡
|613|Squad Type 2|Firefight, Round 1 Main Wave|†‡
|614|Squad Type 3|Firefight, Round 1 Main Wave|‡
|615|Squad Type 4|Firefight, Round 1 Main Wave|‡
|616|Squad Type 5|Firefight, Round 1 Main Wave|‡
|
|624|Dropships|Firefight, Round 1 Boss Wave|‡
|625|Selection|Firefight, Round 1 Boss Wave|‡
|626|Squad Type 1|Firefight, Round 1 Boss Wave|‡
|627|Squad Type 2|Firefight, Round 1 Boss Wave|‡
|628|Squad Type 3|Firefight, Round 1 Boss Wave|‡
|
|639|Skull: Black Eye|Firefight, Round 2|‡
|640|Skull: Tough Luck|Firefight, Round 2|‡
|641|Skull: Catch|Firefight, Round 2|‡
|642|Skull: Fog|Firefight, Round 2|‡
|643|Skull: Famine|Firefight, Round 2|‡
|644
|645|Skull: Tilt|Firefight, Round 2|‡
|646|Skull: Mythic|Firefight, Round 2|‡
|647
|648
|649|Skull: Cowbell|Firefight, Round 2|‡
|650|Skull: Grunt Birthday Party|Firefight, Round 2|‡
|651|Skull: IWHBYD|Firefight, Round 2|‡
|652|Skull: Red|Firefight, Round 2|‡
|653|Skull: Yellow|Firefight, Round 2|‡
|654|Skull: Blue|Firefight, Round 2|‡
|655
|656|Dropships|Firefight, Round 2 Initial Wave|†‡
|657|Selection|Firefight, Round 2 Initial Wave|†‡
|658|Squad Type 1|Firefight, Round 2 Initial Wave|†‡
|659|Squad Type 2|Firefight, Round 2 Initial Wave|†‡
|660|Squad Type 3|Firefight, Round 2 Initial Wave|†‡
|
|670|Dropships|Firefight, Round 2 Main Wave|†‡
|671|Selection|Firefight, Round 2 Main Wave|†‡
|672|Squad Type 1|Firefight, Round 2 Main Wave|†‡
|673|Squad Type 2|Firefight, Round 2 Main Wave|†‡
|674|Squad Type 3|Firefight, Round 2 Main Wave|‡
|675|Squad Type 4|Firefight, Round 2 Main Wave|‡
|676|Squad Type 5|Firefight, Round 2 Main Wave|‡
|
|684|Dropships|Firefight, Round 2 Boss Wave|‡
|685|Selection|Firefight, Round 2 Boss Wave|‡
|686|Squad Type 1|Firefight, Round 2 Boss Wave|‡
|687|Squad Type 2|Firefight, Round 2 Boss Wave|‡
|688|Squad Type 3|Firefight, Round 2 Boss Wave|‡
|
|699|Skull: Black Eye|Firefight, Round 3|‡
|700|Skull: Tough Luck|Firefight, Round 3|‡
|701|Skull: Catch|Firefight, Round 3|‡
|702|Skull: Fog|Firefight, Round 3|‡
|703|Skull: Famine|Firefight, Round 3|‡
|704
|705|Skull: Tilt|Firefight, Round 3|‡
|706|Skull: Mythic|Firefight, Round 3|‡
|707
|708
|709|Skull: Cowbell|Firefight, Round 3|‡
|710|Skull: Grunt Birthday Party|Firefight, Round 3|‡
|711|Skull: IWHBYD|Firefight, Round 3|‡
|712|Skull: Red|Firefight, Round 3|‡
|713|Skull: Yellow|Firefight, Round 3|‡
|714|Skull: Blue|Firefight, Round 3|‡
|715
|716|Dropships|Firefight, Round 3 Initial Wave|†‡
|717|Selection|Firefight, Round 3 Initial Wave|†‡
|718|Squad Type 1|Firefight, Round 3 Initial Wave|†‡
|719|Squad Type 2|Firefight, Round 3 Initial Wave|†‡
|720|Squad Type 3|Firefight, Round 3 Initial Wave|†‡
|
|730|Dropships|Firefight, Round 3 Main Wave|†‡
|745|Selection|Firefight, Round 3 Main Wave|†‡
|732|Squad Type 1|Firefight, Round 3 Main Wave|†‡
|733|Squad Type 2|Firefight, Round 3 Main Wave|†‡
|734|Squad Type 3|Firefight, Round 3 Main Wave|‡
|735|Squad Type 4|Firefight, Round 3 Main Wave|‡
|737|Squad Type 5|Firefight, Round 3 Main Wave|‡
|
|744|Dropships|Firefight, Round 3 Boss Wave|‡
|745|Selection|Firefight, Round 3 Boss Wave|‡
|746|Squad Type 1|Firefight, Round 3 Boss Wave|‡
|747|Squad Type 2|Firefight, Round 3 Boss Wave|‡
|748|Squad Type 3|Firefight, Round 3 Boss Wave|‡
|
|758|Bonus Wave Duration|Firefight|‡
|
|760|Skull: Black Eye|Firefight, Round 3|‡
|761|Skull: Tough Luck|Firefight, Round 3|‡
|762|Skull: Catch|Firefight, Round 3|‡
|763|Skull: Fog|Firefight, Round 3|‡
|764|Skull: Famine|Firefight, Round 3|‡
|765
|766|Skull: Tilt|Firefight, Round 3|‡
|767|Skull: Mythic|Firefight, Round 3|‡
|768
|769
|770|Skull: Cowbell|Firefight, Round 3|‡
|771|Skull: Grunt Birthday Party|Firefight, Round 3|‡
|772|Skull: IWHBYD|Firefight, Round 3|‡
|773|Skull: Red|Firefight, Bonus Wave|‡
|774|Skull: Yellow|Firefight, Bonus Wave|‡
|775|Skull: Blue|Firefight, Bonus Wave|‡
|776
|777|Dropships|Firefight, Bonus Wave|‡
|778|Selection|Firefight, Bonus Wave|‡
|779|Squad Type 1|Firefight, Bonus Wave|‡
|780|Squad Type 2|Firefight, Bonus Wave|‡
|781|Squad Type 3|Firefight, Bonus Wave|‡
|
|826|Damage Resistance|Firefight, Red Skull Elite Traits, Defense|†
|827
|828
|829|Health/Shields?|Firefight, Red Skull Elite Traits, Defense|†
|
|870|Damage Modifier|Firefight, Red Skull Wave Traits|†
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

For completeness, here are the remaining (non-Megalo-accessible) option IDs in Reach:

| ID | Name | Section | Notes |
| -: | :- | :- |
| 1273
| 1274 | `firefight_turn_count` | Firefight | ‡
| 1275 | `firefight_waves` | Firefight | ‡

## Submenu IDs

Listed here for completeness' sake, but these seem to use a separate ID space from options; and in any case, the checks for option toggles get skipped when processing (sub)menus.

These seem to be the same IDs seen in `multiplayer/game_variant_settings/multiplayer-editable_settings`, for the "player trait category" enumeration. That enum was designed for the original Reach menus, though, and the values below appear to be the only ones that are logged when viewing MCC's menus (which sorta-wrap and sorta-shim the Reach menu data). Moreover, a lot of those enum values are Halo 3 leftovers and would've been unused even in the Xbox-era Reach builds.

| ID | Name | Section | Notes |
| -: | :- | :- |
|0|[Menu] `base_map`||‡
|1|[Menu] `respawn_options`||‡
|
|3|[Menu] `map_overrides`||‡
|
|32|[Menu] Custom Powerup Traits||‡
|
|152|[Menu] Editor Traits|Forge|‡
|153|[Menu] `traits_health`|Forge, Editor Traits|‡
|
|156|[Menu] `traits_sensors`|Forge, Editor Traits|‡
|157|[Menu] `traits_appearance`|Forge, Editor Traits|‡
|
|235|[Menu] `traits_health`|All Player Traits
|236|[Menu] `traits_weapons`|All Player Traits
|237|[Menu] `traits_movement`|All Player Traits
|238|[Menu] `traits_sensors`|All Player Traits
|239|[Menu] `traits_appearance`|All Player Traits
|
|242|[Menu] `alternate_traits_weapons`|All Player Traits
|
|249|[Menu] Specific Game Options||‡ Submenu for Megalo-defined options
|250|[Menu] `firefight`|Firefight|‡
|
|253|[Menu] `firefight_round_properties`|Firefight|‡
|254|[Menu] Round 1|Firefight|‡
|255|[Menu] Round 1 Skulls|Firefight|‡
|256|[Menu] Round 1 Initial Wave|Firefight|‡
|257|[Menu] Round 1 Main Wave|Firefight|‡
|258|[Menu] Round 1 Boss Wave|Firefight|‡
|259|[Menu] Round 2|Firefight|‡
|260|[Menu] Round 2 Skulls|Firefight|‡
|261|[Menu] Round 2 Initial Wave|Firefight|‡
|262|[Menu] Round 2 Main Wave|Firefight|‡
|263|[Menu] Round 2 Boss Wave|Firefight|‡
|264|[Menu] Round 3|Firefight|‡
|265|[Menu] Round 3 Skulls|Firefight|‡
|266|[Menu] Round 3 Initial Wave|Firefight|‡
|267|[Menu] Round 3 Main Wave|Firefight|‡
|268|[Menu] Round 3 Boss Wave|Firefight|‡
|
|274|[Menu] Bonus Wave Skulls|Firefight|‡
|275|[Menu] Bonus Wave Options|Firefight|‡
|276|[Menu] Spartan Settings|Firefight|‡
|277|[Menu] Elite Settings|Firefight|‡
|
|292|[Menu] Elite Loadouts||‡
|293|[Menu] Spartan Loadouts||‡
|
|301|[Menu] `base_map`||‡
|302
|303|[Menu] `generator_properties`|Firefight|‡
|304|[Menu] `custom_skulls`|Firefight|‡
|305|[Menu] `custom_skull_red`|Firefight|‡
|306|[Menu] `custom_skull_yellow`|Firefight|‡
|307|[Menu] `custom_skull_blue`|Firefight|‡
|
|356|`loadout_spartans_reach`||‡
|357|[Menu] Spartan Tier 1|Loadouts|‡
|358|[Menu] Loadout 1|Loadouts, Spartan, Any Tier|‡
|359|[Menu] Loadout 2|Loadouts, Spartan, Any Tier|‡
|360|[Menu] Loadout 3|Loadouts, Spartan, Any Tier|‡
|361|[Menu] Loadout 4|Loadouts, Spartan, Any Tier|‡
|362|[Menu] Loadout 5|Loadouts, Spartan, Any Tier|‡
|363|[Menu] Spartan Tier 2|Loadouts|‡
|
|369|[Menu] Spartan Tier 3|Loadouts|‡
|
|375|[Menu] `loadout_elites`||‡
|376|[Menu] Elite Tier 1|Loadouts|‡
|
|377|[Menu] Loadout 1|Loadouts, Elite, Any Tier|‡
|378|[Menu] Loadout 2|Loadouts, Elite, Any Tier|‡
|379|[Menu] Loadout 3|Loadouts, Elite, Any Tier|‡
|380|[Menu] Loadout 4|Loadouts, Elite, Any Tier|‡
|381|[Menu] Loadout 5|Loadouts, Elite, Any Tier|‡
|382|[Menu] Elite Tier 2|Loadouts|‡
|
|388|[Menu] Elite Tier 3|Loadouts|‡