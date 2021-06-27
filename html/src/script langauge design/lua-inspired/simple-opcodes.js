class OpcodeToScriptFunctionMapping {
   constructor(a, b, c, d, e, f, g) {
      this.primary_name      = a || "";
      this.context_arg_index = b || -1; // -1 == no context; -2 == game namespace
      this.arg_indices       = c || [];
      this.secondary_name    = d || "";
      this.secondary_zeroes_out_arg = e || false;
      //
      this.handle_assignment_to_property = g || "";
   }
}
class OpcodeToScriptPropertyMapping {
   constructor(a, b, c) {
      this.name              = a || "";
      this.context_arg_index = b || -1;
      this.operand_arg_index = c || -1;
      //
      // The getter should use -1 for the operand index. For the setter, 
      // we can infer the operator index from whichever one is NOT the 
      // operand or context index.
      //
      // In some cases, only the setter may exist.
   }
}
class OpcodeToScriptGrenadePropertyMapping {
   constructor(a, b, c) {
      this.player_arg_index       = a || -1;
      this.grenade_type_arg_index = b || -1;
      this.operand_arg_index      = c || -1;
      //
      // Only a setter exists.
   }
}

class OpcodeArgBase {
   constructor(a, b, c, d) {
      this.name    = a;
      this.factory = b;
      this.is_out_variable = (!!c === c) ? c : false;
      //
      this.text_true  = (c+"" === c) ? c : "true";
      this.text_false = d || "false";
   }
}
class ActionFunction {
   constructor(n, d, f, a, sm) {
      this.name    = n;
      this.desc    = d || "";
      this.format  = f || ""; // for printing the opcode as plain English
      this.args    = a || [];
      this.mapping = sm; // can be null for built-in stuff
   }
}
class ConditionFunction {
   constructor(n, d, f, a, sm, vn, vi) {
      this.name        = n;
      this.desc        = d || "";
      this.format      = f  || "";       // for printing the opcode as plain English
      this.verb_normal = vn || "is";     // for printing the opcode as plain English
      this.verb_invert = vi || "is not"; // for printing the opcode as plain English
      this.args        = a || [];
      this.mapping     = sm; // can be null for built-in stuff
   }
}

// BEGIN JS MAPPINGS
let OpcodeArgValueScalar = { factory: Symbol("OpcodeArgValueScalar") };
let OpcodeArgValueObject = { factory: Symbol("OpcodeArgValueObject") };
let OpcodeArgValuePlayer = { factory: Symbol("OpcodeArgValuePlayer") };
let OpcodeArgValueTeam   = { factory: Symbol("OpcodeArgValueTeam") };
let OpcodeArgValueTimer  = { factory: Symbol("OpcodeArgValueTimer") };
let OpcodeArgValueAddWeaponEnum         = { factory: Symbol("OpcodeArgValueAddWeaponEnum") };
let OpcodeArgValueCHUDDestinationEnum   = { factory: Symbol("OpcodeArgValueCHUDDestinationEnum") };
let OpcodeArgValueCompareOperatorEnum   = { factory: Symbol("OpcodeArgValueCompareOperatorEnum") };
let OpcodeArgValueConstBool             = { factory: Symbol("OpcodeArgValueConstBool") };
let OpcodeArgValueConstSInt8            = { factory: Symbol("OpcodeArgValueConstSInt8") };
let OpcodeArgValueCreateObjectFlags     = { factory: Symbol("OpcodeArgValueCreateObjectFlags") };
let OpcodeArgValueDropWeaponEnum        = { factory: Symbol("OpcodeArgValueDropWeaponEnum") };
let OpcodeArgValueForgeLabel            = { factory: Symbol("OpcodeArgValueForgeLabel") };
let OpcodeArgValueGrenadeTypeEnum       = { factory: Symbol("OpcodeArgValueGrenadeTypeEnum") };
let OpcodeArgValueIconIndex6Bits        = { factory: Symbol("OpcodeArgValueIconIndex6Bits") };
let OpcodeArgValueIconIndex7Bits        = { factory: Symbol("OpcodeArgValueIconIndex7Bits") };
let OpcodeArgValueIncidentID            = { factory: Symbol("OpcodeArgValueIncidentID") };
let OpcodeArgValueKillerTypeFlags       = { factory: Symbol("OpcodeArgValueKillerTypeFlags") };
let OpcodeArgValueLoadoutPalette        = { factory: Symbol("OpcodeArgValueLoadoutPalette") };
let OpcodeArgValueMathOperatorEnum      = { factory: Symbol("OpcodeArgValueMathOperatorEnum") };
let OpcodeArgValueMeterParameters       = { factory: Symbol("OpcodeArgValueMeterParameters") };
let OpcodeArgValueMPObjectTypeIndex     = { factory: Symbol("OpcodeArgValueMPObjectTypeIndex") };
let OpcodeArgValueNameIndex             = { factory: Symbol("OpcodeArgValueNameIndex") };
let OpcodeArgValueObjectPlayerVariable  = { factory: Symbol("OpcodeArgValueObjectPlayerVariable") };
let OpcodeArgValueObjectTimerVariable   = { factory: Symbol("OpcodeArgValueObjectTimerVariable") };
let OpcodeArgValuePickupPriorityEnum    = { factory: Symbol("OpcodeArgValuePickupPriorityEnum") };
let OpcodeArgValuePlayerSet             = { factory: Symbol("OpcodeArgValuePlayerSet") };
let OpcodeArgValuePlayerTraits          = { factory: Symbol("OpcodeArgValuePlayerTraits") };
let OpcodeArgValuePlayerUnusedModeFlags = { factory: Symbol("OpcodeArgValuePlayerUnusedModeFlags") };
let OpcodeArgValueRequisitionPalette    = { factory: Symbol("OpcodeArgValueRequisitionPalette") };
let OpcodeArgValueShape                 = { factory: Symbol("OpcodeArgValueShape") };
let OpcodeArgValueSound                 = { factory: Symbol("OpcodeArgValueSound") };
let OpcodeArgValueStringTokens2         = { factory: Symbol("OpcodeArgValueStringTokens2") };
let OpcodeArgValueTeamDispositionEnum   = { factory: Symbol("OpcodeArgValueTeamDispositionEnum") };
let OpcodeArgValueTimerRateEnum         = { factory: Symbol("OpcodeArgValueTimerRateEnum") };
let OpcodeArgValueTrigger               = { factory: Symbol("OpcodeArgValueTrigger") };
let OpcodeArgValueVector3               = { factory: Symbol("OpcodeArgValueVector3") };
let OpcodeArgValueWaypointIcon          = { factory: Symbol("OpcodeArgValueWaypointIcon") };
let OpcodeArgValueWaypointPriorityEnum  = { factory: Symbol("OpcodeArgValueWaypointPriorityEnum") };
let OpcodeArgValueWidget                = { factory: Symbol("OpcodeArgValueWidget") };
let OpcodeArgAnyVariableFactory          = Symbol("OpcodeArgAnyVariableFactory");
let OpcodeArgTeamOrPlayerVariableFactory = Symbol("OpcodeArgTeamOrPlayerVariableFactory");
// END   JS MAPPINGS

actionFunctionList = [
   new ActionFunction( // 0
      "None",
      "This condition does nothing.",
      "None.",
      [],
      null
   ),
   new ActionFunction( // 1
      "Modify Score",
      "Modify the score of a player or team.",
      "Modify the score of %1: %2 %3.",
      [
         new OpcodeArgBase("target",   OpcodeArgTeamOrPlayerVariableFactory),
         new OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum.factory),
         new OpcodeArgBase("operand",  OpcodeArgValueScalar.factory)
      ],
      new OpcodeToScriptFunctionMapping("", 0, [], "", false, "score")
   ),
   new ActionFunction( // 2
      "Create Object",
      "Create an object.",
      "Create %1 at offset %6 from %3 with Forge label %4, settings (%5), and name %7. Store a reference to it in %2.",
      [
         new OpcodeArgBase("type",        OpcodeArgValueMPObjectTypeIndex.factory),
         new OpcodeArgBase("result",      OpcodeArgValueObject.factory, true),
         new OpcodeArgBase("spawn point", OpcodeArgValueObject.factory),
         new OpcodeArgBase("label",       OpcodeArgValueForgeLabel.factory),
         new OpcodeArgBase("flags",       OpcodeArgValueCreateObjectFlags.factory),
         new OpcodeArgBase("offset",      OpcodeArgValueVector3.factory),
         new OpcodeArgBase("name",        OpcodeArgValueNameIndex.factory),
      ],
      new OpcodeToScriptFunctionMapping("create_object", 2, [0, 3, 4, 5, 6], "place_at_me", false)
   ),
   new ActionFunction( // 3
      "Delete Object",
      "Delete an object.",
      "Delete %1.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
      ],
      new OpcodeToScriptFunctionMapping("delete", 0)
   ),
   new ActionFunction( // 4
      "Set Waypoint Visibility",
      "Control the visibility of a waypoint on an object.",
      "Modify waypoint visibility for %1: make visible to %2.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("who",    OpcodeArgValuePlayerSet.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_waypoint_visibility", 0, [1])
   ),
   new ActionFunction( // 5
      "Set Waypoint Icon",
      "Control the icon of a waypoint on an object.",
      "Set %1's waypoint icon to %2.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("icon",   OpcodeArgValueWaypointIcon.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_waypoint_icon", 0, [1])
   ),
   new ActionFunction( // 6
      "Set Waypoint Priority",
      "Control the priority of a waypoint on an object.",
      "Set %1's waypoint priority to %2.",
      [
         new OpcodeArgBase("object",   OpcodeArgValueObject.factory),
         new OpcodeArgBase("priority", OpcodeArgValueWaypointPriorityEnum.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_waypoint_priority", 0, [1])
   ),
   new ActionFunction( // 7
      "Set Object Displayed Timer",
      "Set which timer an object displays.",
      "Have %1 display %2.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("timer",  OpcodeArgValueObjectTimerVariable.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_displayed_timer", 0, [1])
   ),
   new ActionFunction( // 8
      "Set Object Distance Range",
      "",
      "Set %1's distance range to minimum %2, maximum %3.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("min",    OpcodeArgValueScalar.factory), // should be in the range of [-1, 100]
         new OpcodeArgBase("max",    OpcodeArgValueScalar.factory), // should be in the range of [-1, 100]
      ],
      new OpcodeToScriptFunctionMapping("set_distance_range", 0, [1, 2])
   ),
   new ActionFunction( // 9
      "Modify Variable",
      "Modify one variable by another.",
      "Modify variable %1: %3 %2.",
      [
         new OpcodeArgBase("a", OpcodeArgAnyVariableFactory, true),
         new OpcodeArgBase("b", OpcodeArgAnyVariableFactory),
         new OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum.factory),
      ],
      new OpcodeToScriptFunctionMapping("", -1, []) // SPECIAL-CASE
   ),
   new ActionFunction( // 10
      "Set Object Shape",
      "Add a Shape to an object, or replace its existing Shape.",
      "Set %1's shape to %2.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("shape",  OpcodeArgValueShape.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_shape", 0, [1])
   ),
   new ActionFunction( // 11
      "Apply Player Traits",
      "Apply one of the scripted sets of Player Traits to a player. There is no trigger action to remove a set of player traits; rather, you must reapply traits on every frame, and you \"remove\" traits by not reapplying them.",
      "Apply %2 to %1.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("traits", OpcodeArgValuePlayerTraits.factory),
      ],
      new OpcodeToScriptFunctionMapping("apply_traits", 0, [1])
   ),
   new ActionFunction( // 12
      "Set Object Interact Permissions",
      "Limit who can interact with an object.",
      "Only allow %2 to interact with %1.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("who",    OpcodeArgValuePlayerSet.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_interact_permissions", 0, [1])
   ),
   new ActionFunction( // 13
      "Set Spawn Location Permissions",
      "Limit who can spawn at an object.",
      "Only allow %2 to spawn at %1.",
      [
         new OpcodeArgBase("spawn location", OpcodeArgValueObject.factory),
         new OpcodeArgBase("who", OpcodeArgValuePlayerSet.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_spawn_location_permissions", 0, [1])
   ),
   new ActionFunction( // 14
      "Set Spawn Location Fireteam",
      "Limit which fireteam can spawn at an object.",
      "Only allow fireteam #%2 to spawn at %1.",
      [
         new OpcodeArgBase("spawn location", OpcodeArgValueObject.factory),
         new OpcodeArgBase("fireteam",       OpcodeArgValueConstSInt8.factory), // TODO: -1 == "no fireteam"
      ],
      new OpcodeToScriptFunctionMapping("set_spawn_location_fireteam", 0, [1])
   ),
   new ActionFunction( // 15
      "Set Object Progress Bar",
      "Set whether an object displays a progress bar.",
      "Have %1 display %3 as a progress bar for %2.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("who",    OpcodeArgValuePlayerSet.factory),
         new OpcodeArgBase("timer",  OpcodeArgValueObjectTimerVariable.factory),
      ],
      new OpcodeToScriptFunctionMapping("show_progress_bar", 0, [1, 2])
   ),
   new ActionFunction( // 16
      "CHUD Message",
      "",
      "For %1, play sound %2 and display message: %3.",
      [
         new OpcodeArgBase("who",   OpcodeArgTeamOrPlayerVariableFactory),
         new OpcodeArgBase("sound", OpcodeArgValueSound.factory),
         new OpcodeArgBase("text",  OpcodeArgValueStringTokens2.factory),
      ],
      new OpcodeToScriptFunctionMapping("show_message", 0, [1, 2])
   ),
   new ActionFunction( // 17
      "Set Timer Rate",
      "",
      "Have %1 tick at rate %2.",
      [
         new OpcodeArgBase("timer", OpcodeArgValueTimer.factory),
         new OpcodeArgBase("rate",  OpcodeArgValueTimerRateEnum.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_rate", 0, [1])
   ),
   new ActionFunction( // 18
      "Deprecated-18",
      "This trigger action does nothing.",
      "Do nothing. (Unused message: %1)",
      [
         new OpcodeArgBase("text", OpcodeArgValueStringTokens2.factory),
      ],
      new OpcodeToScriptFunctionMapping("deprecated_18", -1, [0])
   ),
   new ActionFunction( // 19
      "Get Item Owner",
      "Set a player variable to the owner of an object.",
      "Set %2 to the owner of %1.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("get_owner", 0, [1])
   ),
   new ActionFunction( // 20
      "Run Nested Trigger",
      "Branches execution to another trigger.",
      "Execute trigger %1.",
      [
         new OpcodeArgBase("trigger", OpcodeArgValueTrigger.factory),
      ],
      null // SPECIAL_CASE
   ),
   new ActionFunction( // 21
      "End Round With Winner",
      "Ends the round with a winner.",
      "End the current round, and declare the player or team with the highest score the winner of the round.",
      [],
      new OpcodeToScriptFunctionMapping("end_round", -2)
   ),
   new ActionFunction( // 22
      "Set Object Shape Visibility",
      "Limit who can see an object's Shape.",
      "Only allow %2 to see %1's shape.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("who",    OpcodeArgValuePlayerSet.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_shape_visibility", 0, [1])
   ),
   new ActionFunction( // 23
      "Kill Object Instantly",
      "Instantly destroy the specified object.",
      "Destroy %1 %2.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("silent", OpcodeArgValueConstBool.factory, "silently", "loudly"),
      ],
      new OpcodeToScriptFunctionMapping("kill", 0, [1])
   ),
   new ActionFunction( // 24
      "Set Object Invincibility",
      "Set whether an object is invincible. Invincible objects cannot be destroyed by anything -- even level boundaries.",
      "Modify %1 invincibility: set to %2.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("invincible (treated as a bool)", OpcodeArgValueScalar.factory), // treated as a bool
      ],
      new OpcodeToScriptFunctionMapping("set_invincible", 0, [1])
   ),
   new ActionFunction( // 25
      "Random Number",
      "Set a number variable to a random value.",
      "Set %2 to a random number generated using seed %1.",
      [
         new OpcodeArgBase("seed",   OpcodeArgValueScalar.factory),
         new OpcodeArgBase("result", OpcodeArgValueScalar.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("get_random_number", -1, [0])
   ),
   new ActionFunction( // 26
      "Deprecated-26",
      "Does nothing.",
      "Do nothing.",
      [],
      new OpcodeToScriptFunctionMapping("deprecated_26")
   ),
   new ActionFunction( // 27
      "Unknown-27",
      "Retrieve some unknown value from an object, always between 1 and 7 inclusive. This can fail, so you should consider resetting the number variable before calling this.",
      "Set %2 to the Unknown-27 value on %1.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("result", OpcodeArgValueScalar.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("unknown_27", 0)
   ),
   new ActionFunction( // 28
      "Get Speed",
      "Set a number variable to an object's speed. The speed is premultiplied by 10 and, if less than 0.1, returned as zero.",
      "Set integer %2 to the current speed of %1 multiplied by 10.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("result", OpcodeArgValueScalar.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("get_speed", 0)
   ),
   new ActionFunction( // 29
      "Get Killer",
      "Set a player variable to whoever most recently killed the target player.", // does nothing if player isn't dead? bungie sets the out-variable to no-player before running this
      "Set %2 to the killer of %1.",
      [
         new OpcodeArgBase("victim", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("killer", OpcodeArgValuePlayer.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("try_get_killer", 0, [], "get_killer", true)
   ),
   new ActionFunction( // 30
      "Get Death Damage Type",
      "Set a number variable to indicate what damage type killed a player.",
      "Set integer %2 to the damage type(s) that killed %1.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("result", OpcodeArgValueScalar.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("try_get_death_damage_type", 0, [], "get_death_damage_type", true)
   ),
   new ActionFunction( // 31
      "Get Death Damage Modifier",
      "Set a number variable to indicate what damage modifier killed a player.", // does nothing if player isn't dead? bungie sets the out-variable to 0 before running this
      "Set integer %2 to the damage modifier(s) that killed %1.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("result", OpcodeArgValueScalar.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("try_get_death_damage_mod", 0, [], "get_death_damage_mod", true)
      //
      // Known return values:
      // 1: pummel
      // 2: assassination (does this include backsmacks or just stabs?)
      // 3: splatter
      // 4: stuck with grenade
      // 5: headshot
      //
   ),
   new ActionFunction( // 32
      "Unknown-32",
      "",
      "Carry out some unknown action (32) using boolean %1.",
      [
         new OpcodeArgBase("bool", OpcodeArgValueConstBool.factory),
      ],
      new OpcodeToScriptFunctionMapping("unknown_32", -1, [0])
   ),
   new ActionFunction( // 33
      "Attach Objects",
      "Attach one object to another.",
      "Attach %1 to %2 at offset %3. Unknown bool: %4.",
      [
         new OpcodeArgBase("subject", OpcodeArgValueObject.factory),
         new OpcodeArgBase("target",  OpcodeArgValueObject.factory),
         new OpcodeArgBase("offset",  OpcodeArgValueVector3.factory),
         new OpcodeArgBase("bool",    OpcodeArgValueConstBool.factory),
      ],
      new OpcodeToScriptFunctionMapping("attach_to", 0, [1, 2, 3])
   ),
   new ActionFunction( // 34
      "Detach Objects",
      "Detach an object from any other object it is attached to.",
      "Detach %1 from whatever other object it is attached to.",
      [
         new OpcodeArgBase("subject", OpcodeArgValueObject.factory),
      ],
      new OpcodeToScriptFunctionMapping("detach", 0)
   ),
   new ActionFunction( // 35
      "Get Player Scoreboard Position",
      "Get a player's position on the scoreboard, across all players and teams.",
      "Set %2 to %1's position on the scoreboard.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("result", OpcodeArgValueScalar.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("get_scoreboard_position", 0)
   ),
   new ActionFunction( // 36
      "Get Team Index",
      "Get the index of a team variable's value.",
      "Set %2 to %1's index.",
      [
         new OpcodeArgBase("team",   OpcodeArgValueTeam.factory),
         new OpcodeArgBase("result", OpcodeArgValueScalar.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("get_index", 0)
   ),
   new ActionFunction( // 37
      "Get Player Killstreak",
      "Returns the number of consecutive kills that a player has accomplished without dying.",
      "Set %2 to the number of consecutive kills %1 has achieved without dying.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("result", OpcodeArgValueScalar.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("get_spree_count", 0)
   ),
   new ActionFunction( // 38
      "Unknown-38",
      "",
      "Carry out some unknown action (38) involving %1 and %2.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("number", OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptFunctionMapping("unknown_38", 0, [1])
   ),
   new ActionFunction( // 39
      "Unknown-39",
      "",
      "Toggle unused flags %2 for %1.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("flags",  OpcodeArgValuePlayerUnusedModeFlags.factory),
      ],
      new OpcodeToScriptFunctionMapping("unknown_39", 0, [1])
   ),
   new ActionFunction( // 40
      "Get Player Vehicle",
      "Sets an object variable to the vehicle that a player is currently using. Does not include boarding.", // does nothing if no player or no vehicle? bungie sets the out-variable to no-object before running this
      "Set %2 to the vehicle that %1 is currently using.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("result", OpcodeArgValueObject.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("get_vehicle", 0, [])
   ),
   new ActionFunction( // 41
      "Force Player Into Vehicle",
      "Forces the player into a vehicle. The player is instantly teleported in.",
      "Force %1 into any unoccupied seat in %2.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("vehicle", OpcodeArgValueObject.factory),
      ],
      new OpcodeToScriptFunctionMapping("force_into_vehicle", 0, [1])
   ),
   new ActionFunction( // 42
      "Set Player Biped", // Used for Halo Chess's bump-possession?
      "",
      "Set %1's biped to %2.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_biped", 0, [1]) // TODO: use property-assign handler instead?
   ),
   new ActionFunction( // 43
      "Reset Timer",
      "Resets a timer.", // to what?
      "Reset %1.",
      [
         new OpcodeArgBase("timer", OpcodeArgValueTimer.factory),
      ],
      new OpcodeToScriptFunctionMapping("reset", 0)
   ),
   new ActionFunction( // 44
      "Set Weapon Pickup Priority",
      "Modify whether a weapon is picked up automatically or as a hold action.",
      "Set %1's weapon pickup priority to %2.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("priority", OpcodeArgValuePickupPriorityEnum.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_weapon_pickup_priority", 0, [1])
   ),
   new ActionFunction( // 45
      "Push Object Up",
      "Launches an object upward by a random amount. This is used for things like skulls popping out of heads in Headhunter.",
      "Apply a semi-random amount of upward force to %1.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
      ],
      new OpcodeToScriptFunctionMapping("push_upward", 0)
   ),
   new ActionFunction( // 46
      "Set Widget Text",
      "Modifies the text for a HUD widget. You can stitch values together dynamically.",
      "Set text for %1 to: %2.",
      [
         new OpcodeArgBase("widget", OpcodeArgValueWidget.factory),
         new OpcodeArgBase("text",   OpcodeArgValueStringTokens2.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_text", 0, [1])
   ),
   new ActionFunction( // 47
      "Set Widget Text (Unknown)",
      "Modifies an unknown text field for a HUD widget. You can stitch values together dynamically.",
      "Set text for %1 to: %2.",
      [
         new OpcodeArgBase("widget", OpcodeArgValueWidget.factory),
         new OpcodeArgBase("text",   OpcodeArgValueStringTokens2.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_text_2", 0, [1])
   ),
   new ActionFunction( // 48
      "Set Meter Parameters",
      "Modify meter options for a HUD widget.",
      "Set meter parameters for %1 to %2.",
      [
         new OpcodeArgBase("widget",     OpcodeArgValueWidget.factory),
         new OpcodeArgBase("parameters", OpcodeArgValueMeterParameters.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_meter_params", 0, [1])
   ),
   new ActionFunction( // 49
      "Set Widget Icon",
      "Modify a HUD widget's icon.",
      "Set %1's icon to %2.",
      [
         new OpcodeArgBase("widget", OpcodeArgValueWidget.factory),
         new OpcodeArgBase("icon",   OpcodeArgValueIconIndex6Bits.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_icon", 0, [1])
   ),
   new ActionFunction( // 50
      "Set Widget Visibility",
      "Modify a HUD widget's icon.",
      "%3 widget %1 for player %2.",
      [
         new OpcodeArgBase("widget",  OpcodeArgValueWidget.factory),
         new OpcodeArgBase("player",  OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("visible", OpcodeArgValueConstBool.factory, "Show", "Hide"),
      ],
      new OpcodeToScriptFunctionMapping("set_visibility", 0, [1, 2])
   ),
   new ActionFunction( // 51
      "Play Sound",
      "",
      "For %3, play sound %1. Unknown parameter: %2.",
      [
         new OpcodeArgBase("sound",  OpcodeArgValueSound.factory),
         new OpcodeArgBase("params", OpcodeArgValueCHUDDestinationEnum.factory),
         new OpcodeArgBase("who",    OpcodeArgTeamOrPlayerVariableFactory),
      ],
      new OpcodeToScriptFunctionMapping("play_sound_for", -2, [0, 1, 2])
   ),
   new ActionFunction( // 52
      "Modify Object Scale",
      "Resizes an object.",
      "Set %1's scale to %2.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("scale",  OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_scale", 0, [1])
   ),
   new ActionFunction( // 53
      "Set Object Display Text",
      "",
      "Set %1's display text to message: %2.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("text",   OpcodeArgValueStringTokens2.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_display_text", 0, [1])
   ),
   new ActionFunction( // 54
      "Get Object Shields",
      "Returns an object's shields as a percentage (i.e. full = 100).",
      "Set %2 to the percentage of shields %1 has remaining.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("result", OpcodeArgValueScalar.factory, true),
      ],
      new OpcodeToScriptPropertyMapping("shields", 0, -1)
   ),
   new ActionFunction( // 55
      "Get Object Health",
      "Returns an object's health as a percentage (i.e. full = 100).",
      "Set %2 to the percentage of health %1 has remaining.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("result", OpcodeArgValueScalar.factory, true),
      ],
      new OpcodeToScriptPropertyMapping("health", 0, -1)
   ),
   new ActionFunction( // 56
      "Set Objective Title for Player",
      "Alters the text of the title card shown at the start of a round.",
      "Set the objective title for %1 to %2.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("text",   OpcodeArgValueStringTokens2.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_round_card_title", 0, [1])
   ),
   new ActionFunction( // 57
      "Set Objective Description for Player",
      "Alters the description of the title card shown at the start of a round.",
      "Set the objective description for %1 to %2.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("text",   OpcodeArgValueStringTokens2.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_round_card_description", 0, [1])
   ),
   new ActionFunction( // 58
      "Set Objective Icon for Player",
      "Alters the icon of the title card shown at the start of a round.",
      "Set the objective icon for %1 to %2.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("icon",   OpcodeArgValueIconIndex7Bits.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_round_card_icon", 0, [1])
   ),
   new ActionFunction( // 59
      "Enable/Disable Fireteam Spawning",
      "Fireteam Spawning is more commonly known as \"Bro Spawning,\" and is used in Invasion and in Bro Slayer.",
      "%2 fireteam spawning for %1.",
      [
         new OpcodeArgBase("team", OpcodeArgValueTeam.factory),
         new OpcodeArgBase("bool", OpcodeArgValueConstBool.factory, "Enable", "Disable"),
      ],
      new OpcodeToScriptFunctionMapping("set_fireteam_spawning_enabled", 0, [1])
   ),
   new ActionFunction( // 60
      "Set Respawn Object for Team",
      "",
      "Set %1's respawn object: %2.",
      [
         new OpcodeArgBase("team",    OpcodeArgValueTeam.factory),
         new OpcodeArgBase("respawn", OpcodeArgValueObject.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_respawn_object", 0, [1])
   ),
   new ActionFunction( // 61
      "Set Respawn Object for Player",
      "",
      "Set %1's respawn object: %2.",
      [
         new OpcodeArgBase("player",  OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("respawn", OpcodeArgValueObject.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_respawn_object", 0, [1])
   ),
   new ActionFunction( // 62
      "Get Player Fireteam",
      "Returns a player's fireteam number.",
      "Set %2 to the index of %1's fireteam.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("result", OpcodeArgValueScalar.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("get_fireteam", 0)
   ),
   new ActionFunction( // 63
      "Set Player Fireteam",
      "Sets a player's fireteam number.",
      "Assign %1 to fireteam %2..",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("index",  OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_fireteam", 0, [1])
   ),
   new ActionFunction( // 64
      "Modify Object Shields",
      "Modifies an object's shields. Remember that 100 = full.",
      "Modify shields for %1: %2 %3.",
      [
         new OpcodeArgBase("object",   OpcodeArgValueObject.factory),
         new OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum.factory),
         new OpcodeArgBase("operand",  OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptPropertyMapping("shields", 0, 2)
   ),
   new ActionFunction( // 65
      "Modify Object Health",
      "Modifies an object's health. Remember that 100 = full.",
      "Modify health for %1: %2 %3.",
      [
         new OpcodeArgBase("object",   OpcodeArgValueObject.factory),
         new OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum.factory),
         new OpcodeArgBase("operand",  OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptPropertyMapping("health", 0, 2)
   ),
   new ActionFunction( // 66
      "Get Distance",
      "Returns the distance between two objects.",
      "Set %3 to the distance between %1 and %2.",
      [
         new OpcodeArgBase("a", OpcodeArgValueObject.factory),
         new OpcodeArgBase("b", OpcodeArgValueObject.factory),
         new OpcodeArgBase("result", OpcodeArgValueScalar.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("get_distance_to", 0, [1])
   ),
   new ActionFunction( // 67
      "Modify Object Max Shields",
      "Modifies an object's max shields. Remember that 100 = full.",
      "Modify max shields for %1: %2 %3.",
      [
         new OpcodeArgBase("object",   OpcodeArgValueObject.factory),
         new OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum.factory),
         new OpcodeArgBase("operand",  OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptPropertyMapping("shields_max", 0, 2)
   ),
   new ActionFunction( // 68
      "Modify Object Max Health",
      "Modifies an object's max health. Remember that 100 = full.",
      "Modify max health for %1: %2 %3.",
      [
         new OpcodeArgBase("object",   OpcodeArgValueObject.factory),
         new OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum.factory),
         new OpcodeArgBase("operand",  OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptPropertyMapping("health_max", 0, 2)
   ),
   new ActionFunction( // 69
      "Set Player Requisition Palette",
      "Switches which Requisition Palette a player has access to. This functionality is not used in Halo: Reach.",
      "Switch %1 to %2.",
      [
         new OpcodeArgBase("player",  OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("palette", OpcodeArgValueRequisitionPalette.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_requisition_palette", 0, [1])
   ),
   new ActionFunction( // 70
      "Set Device Power",
      "",
      "Set the power for %1 to %2.",
      [
         new OpcodeArgBase("device", OpcodeArgValueObject.factory),
         new OpcodeArgBase("power",  OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_device_power", 0, [1])
   ),
   new ActionFunction( // 71
      "Get Device Power",
      "",
      "Set %2 to the power for %1.",
      [
         new OpcodeArgBase("device", OpcodeArgValueObject.factory),
         new OpcodeArgBase("power",  OpcodeArgValueScalar.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("get_device_power", 0, [1])
   ),
   new ActionFunction( // 72
      "Set Device Position",
      "",
      "Set the position for %1 to %2.",
      [
         new OpcodeArgBase("device",   OpcodeArgValueObject.factory),
         new OpcodeArgBase("position", OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_device_position", 0, [1])
   ),
   new ActionFunction( // 73
      "Get Device Position",
      "",
      "Set %2 to the position for %1.",
      [
         new OpcodeArgBase("device",   OpcodeArgValueObject.factory),
         new OpcodeArgBase("position", OpcodeArgValueScalar.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("get_device_position", 0, [1])
   ),
   new ActionFunction( // 74
      "Modify Player Grenades",
      "Modify the amount of grenades the player is carrying.",
      "Modify %2 grenade count for %1: %3 %4.",
      [
         new OpcodeArgBase("player",   OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("type",     OpcodeArgValueGrenadeTypeEnum.factory),
         new OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum.factory),
         new OpcodeArgBase("operand",  OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptGrenadePropertyMapping(0, 1, 3)
   ),
   new ActionFunction( // 75
      "Submit Incident",
      "",
      "Inform the game that incident %1 has occurred, caused by %2 and affecting %3.",
      [
         new OpcodeArgBase("incident",  OpcodeArgValueIncidentID.factory),
         new OpcodeArgBase("cause?",    OpcodeArgTeamOrPlayerVariableFactory),
         new OpcodeArgBase("affected?", OpcodeArgTeamOrPlayerVariableFactory),
      ],
      new OpcodeToScriptFunctionMapping("send_incident", -1, [0, 1, 2])
   ),
   new ActionFunction( // 76
      "Submit Incident with value",
      "",
      "Inform the game that incident %1 has occurred, caused by %2 and affecting %3, with value %4.",
      [
         new OpcodeArgBase("incident",  OpcodeArgValueIncidentID.factory),
         new OpcodeArgBase("cause?",    OpcodeArgTeamOrPlayerVariableFactory),
         new OpcodeArgBase("affected?", OpcodeArgTeamOrPlayerVariableFactory),
         new OpcodeArgBase("value",     OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptFunctionMapping("send_incident", -1, [0, 1, 2, 3])
   ),
   new ActionFunction( // 77
      "Set Loadout Palette",
      "Set which loadout palette a player or team has access to.",
      "Switch %1 to %2.",
      [
         new OpcodeArgBase("team or player",  OpcodeArgTeamOrPlayerVariableFactory),
         new OpcodeArgBase("loadout palette", OpcodeArgValueLoadoutPalette.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_loadout_palette", 0, [1])
   ),
   new ActionFunction( // 78
      "Set Device Position Track",
      "",
      "Set %1's device track and position to %2 and %3, respectively.",
      [
         new OpcodeArgBase("device",    OpcodeArgValueObject.factory),
         new OpcodeArgBase("animation", OpcodeArgValueNameIndex.factory),
         new OpcodeArgBase("position",  OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_device_position_track", 0, [1, 2])
   ),
   new ActionFunction( // 79
      "Animate Device Position",
      "",
      "Animate %1's position using these values: %2, %3, %4, %5.",
      [
         new OpcodeArgBase("device", OpcodeArgValueObject.factory),
         new OpcodeArgBase("unknown number A", OpcodeArgValueScalar.factory),
         new OpcodeArgBase("unknown number B", OpcodeArgValueScalar.factory),
         new OpcodeArgBase("unknown number C", OpcodeArgValueScalar.factory),
         new OpcodeArgBase("unknown number D", OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptFunctionMapping("animate_device_position", 0, [1, 2, 3, 4])
   ),
   new ActionFunction( // 80
      "Set Device Actual Position",
      "",
      "Set the actual position for %1 to %2.",
      [
         new OpcodeArgBase("device",   OpcodeArgValueObject.factory),
         new OpcodeArgBase("position", OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_device_actual_position", 0, [1])
   ),
   new ActionFunction( // 81
      "Unknown-81",
      "",
      "Carry out an unknown action (81) with number %1 and text: %2.",
      [
         new OpcodeArgBase("number", OpcodeArgValueScalar.factory),
         new OpcodeArgBase("text",   OpcodeArgValueStringTokens2.factory),
      ],
      new OpcodeToScriptFunctionMapping("unknown_81", -1, [0, 1])
   ),
   new ActionFunction( // 82
      "Enable/Disable Spawn Zone",
      "",
      "Modify enable state for spawn zone %1: set to %2.",
      [
         new OpcodeArgBase("spawn zone", OpcodeArgValueObject.factory),
         new OpcodeArgBase("enable (treated as bool)", OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_spawn_zone_enabled", 0, [1])
   ),
   new ActionFunction( // 83
      "Get Player Weapon",
      "Returns a player's primary or secondary weapon, as desired.",
      "Set %3 to %1's %2 weapon.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("which",  OpcodeArgValueConstBool.factory, "primary", "secondary"),
         new OpcodeArgBase("result", OpcodeArgValueObject.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("get_weapon", 0, [1])
   ),
   new ActionFunction( // 84
      "Get Player Armor Ability",
      "Returns a player's armor ability.", // does nothing if no player / no AA? bungie sets the out-variable to no-object before running this
      "Set %2 to %1's Armor Ability.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("result", OpcodeArgValueObject.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("try_get_armor_ability", 0, [1], "get_armor_ability", true)
   ),
   new ActionFunction( // 85
      "Enable/Disable Object Garbage Collection",
      "Set whether an object can be garbage-collected.",
      "%2 garbage collection of %1.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("state",  OpcodeArgValueConstBool.factory, "Enable", "Disable"),
      ],
      new OpcodeToScriptFunctionMapping("set_garbage_collection_enabled", 0, [1])
   ),
   new ActionFunction( // 86
      "Unknown-86",
      "",
      "Carry out some unknown (86) action with %1 and %2.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
      ],
      new OpcodeToScriptFunctionMapping("unknown_86", -1, [0, 1])
   ),
   new ActionFunction( // 87
      "Create Object Equidistant",
      "",
      "Create an instance of %3 at the exact midpoint between %1 and %2, and store it in %5. Unknown number: %4.",
      [
         new OpcodeArgBase("a", OpcodeArgValueObject.factory),
         new OpcodeArgBase("b", OpcodeArgValueObject.factory),
         new OpcodeArgBase("type",   OpcodeArgValueMPObjectTypeIndex.factory),
         new OpcodeArgBase("number", OpcodeArgValueScalar.factory),
         new OpcodeArgBase("result", OpcodeArgValueObject.factory, true),
      ],
      new OpcodeToScriptFunctionMapping("create_object_equidistant_to", 0, [1, 2, 3], "place_equidistant_to_me")
   ),
   new ActionFunction( // 88
      "Deprecated-88",
      "",
      "Do nothing. (Unused argument: %1)",
      [
         new OpcodeArgBase("number", OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptFunctionMapping("deprecated_88", -1, [0])
   ),
   new ActionFunction( // 86
      "Unknown-89",
      "",
      "Carry out some unknown (89) action with %1 and %2.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
      ],
      new OpcodeToScriptFunctionMapping("unknown_89", -1, [0, 1])
   ),
   new ActionFunction( // 90
      "Unknown-90",
      "Used in Halo: Reach's standard Race variant?",
      "Carry out some unknown (90) action with %1 and %2.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("bool",   OpcodeArgValueConstBool.factory),
      ],
      new OpcodeToScriptFunctionMapping("unknown_90", 0, [1])
   ),
   new ActionFunction( // 91
      "Copy Object Rotation",
      "",
      "Have %1 copy %2's rotation on the %3 axis.",
      [
         new OpcodeArgBase("a", OpcodeArgValueObject.factory),
         new OpcodeArgBase("b", OpcodeArgValueObject.factory),
         new OpcodeArgBase("axis", OpcodeArgValueConstBool.factory, "axis 1", "axis 0"),
      ],
      new OpcodeToScriptFunctionMapping("copy_rotation", 0, [2, 1])
   ),
   new ActionFunction( // 92
      "Move Object Relative To", // needs verification
      "",
      "Move %1 to point %3 relative to %2.",
      [
         new OpcodeArgBase("a", OpcodeArgValueObject.factory),
         new OpcodeArgBase("b", OpcodeArgValueObject.factory),
         new OpcodeArgBase("point", OpcodeArgValueVector3.factory),
      ],
      new OpcodeToScriptFunctionMapping("move_to", 0, [1, 2])
   ),
   new ActionFunction( // 93
      "Add Weapon To",
      "Give a weapon to any biped, be it a player or an inanimate script-spawned Spartan, Elite, or Monitor.",
      "Add weapon of type %2 to %1 using mode %3.",
      [
         new OpcodeArgBase("biped",  OpcodeArgValueObject.factory),
         new OpcodeArgBase("weapon", OpcodeArgValueMPObjectTypeIndex.factory),
         new OpcodeArgBase("mode",   OpcodeArgValueAddWeaponEnum.factory),
      ],
      new OpcodeToScriptFunctionMapping("add_weapon", 0, [1, 2])
   ),
   new ActionFunction( // 94
      "Remove Weapon From",
      "Take a weapon from any biped, be it a player or an inanimate script-spawned Spartan, Elite, or Monitor.",
      "Remove %1's %2 weapon, %3.",
      [
         new OpcodeArgBase("biped",  OpcodeArgValueObject.factory),
         new OpcodeArgBase("which",  OpcodeArgValueDropWeaponEnum.factory),
         new OpcodeArgBase("delete", OpcodeArgValueConstBool.factory, "and delete it", "but do not delete it"),
      ],
      new OpcodeToScriptFunctionMapping("remove_weapon", 0, [1, 2])
   ),
   new ActionFunction( // 95
      "Set Scenario Interpolator State",
      "",
      "Set scenario interpolator state: %1, %2.",
      [
         new OpcodeArgBase("a", OpcodeArgValueScalar.factory),
         new OpcodeArgBase("b", OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_scenario_interpolator_state", -2, [0, 1])
   ),
   new ActionFunction( // 96
      "Unknown-96",
      "Used in CTF to return the flag (none, flag, return Forge label).",
      "Carry out some unknown (96) action with %1 and %3 and store the result in %2.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("result", OpcodeArgValueObject.factory, true),
         new OpcodeArgBase("label",  OpcodeArgValueForgeLabel.factory),
      ],
      new OpcodeToScriptFunctionMapping("unknown_96", -1, [0, 1, 2])
   ),
   new ActionFunction( // 97
      "Unknown-97",
      "",
      "Carry out some unknown (97) action with %1 and %2.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("number", OpcodeArgValueScalar.factory),
      ],
      new OpcodeToScriptFunctionMapping("unknown_97", 0, [1])
   ),
   new ActionFunction( // 98
      "Set Boundary Visibility Filter", // TODO: figure out what this even means; current speculation is based on Halo 4's known actions
      "",
      "Make an object's boundary visible to one of its player variables?: %1.",
      [
         new OpcodeArgBase("object + player", OpcodeArgValueObjectPlayerVariable.factory),
      ],
      new OpcodeToScriptFunctionMapping("set_shape_visibility_filter", 0, []) // TODO: THIS PROBABLY ISN'T RIGHT
   ),
];
conditionFunctionList = [
   new ConditionFunction( // 0
      "None",
      "This condition does nothing.",
      "None.",
      [],
      null
   ),
   new ConditionFunction( // 1
      "Compare",
      "Compares any two values.",
      "%1 %v %3 %2.",
      [
         new OpcodeArgBase("a", OpcodeArgAnyVariableFactory),
         new OpcodeArgBase("b", OpcodeArgAnyVariableFactory),
         new OpcodeArgBase("operator", OpcodeArgValueCompareOperatorEnum.factory)
      ],
      null // SPECIAL-CASE
   ),
   new ConditionFunction( // 2
      "Object In Boundary",
      "Checks whether one object is inside of another object's Shape.",
      "%1 %v inside of %2's shape.",
      [
         new OpcodeArgBase("a", OpcodeArgValueObject.factory),
         new OpcodeArgBase("b", OpcodeArgValueObject.factory),
      ],
      new OpcodeToScriptFunctionMapping("shape_contains", 1, [0])
   ),
   new ConditionFunction( // 3
      "Killer Type",
      "Checks what killed a player.",
      "%1 %v killed by any of: %2.",
      [
         new OpcodeArgBase("victim", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("killer types", OpcodeArgValueKillerTypeFlags.factory),
      ],
      new OpcodeToScriptFunctionMapping("killed_by", 0, [1]),
      "was", "was not"
   ),
   new ConditionFunction( // 4
      "Team Disposition",
      "",
      "%1 %v disposition %3 with %2.",
      [
         new OpcodeArgBase("a", OpcodeArgValueTeam.factory),
         new OpcodeArgBase("b", OpcodeArgValueTeam.factory),
         new OpcodeArgBase("disposition", OpcodeArgValueTeamDispositionEnum.factory),
      ],
      new OpcodeToScriptFunctionMapping("has_disposition_with", 0, [1, 2]),
      "has", "does not have"
   ),
   new ConditionFunction( // 5
      "Timer Is Zero",
      "Checks whether a timer is at zero.",
      "%1 %v at zero.",
      [
         new OpcodeArgBase("timer", OpcodeArgValueTimer.factory),
      ],
      new OpcodeToScriptFunctionMapping("is_zero", 0),
   ),
   new ConditionFunction( // 6
      "Object Type",
      "Checks an object's type.",
      "%1 %v of type %2.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("type",   OpcodeArgValueMPObjectTypeIndex.factory),
      ],
      new OpcodeToScriptFunctionMapping("is_of_type", 0, [1]),
   ),
   new ConditionFunction( // 7
      "Team Has Players",
      "Checks whether a team has one or more players on it.",
      "%1 %v one or more players on it.",
      [
         new OpcodeArgBase("team", OpcodeArgValueTeam.factory),
      ],
      new OpcodeToScriptFunctionMapping("has_any_players", 0),
      "has", "does not have"
   ),
   new ConditionFunction( // 8
      "Object Out Of Bounds",
      "Checks whether an object is out of bounds.",
      "%1 %v out of bounds.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
      ],
      new OpcodeToScriptFunctionMapping("is_out_of_bounds", 0),
   ),
   new ConditionFunction( // 9
      "Deprecated-09",
      "This condition always returns false.", // TODO: does inverting it change that?
      "Never. (Unused argument: %1)",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
      ],
      new OpcodeToScriptFunctionMapping("deprecated_09", 0)
   ),
   new ConditionFunction( // 10
      "Player Assisted Kill Of",
      "Checks whether one player assisted in the slaying of another player. Note that players don't count as \"assisting\" themselves.",
      "%1 %v in the killing of %2.",
      [
         new OpcodeArgBase("attacker", OpcodeArgValuePlayer.factory),
         new OpcodeArgBase("victim",   OpcodeArgValuePlayer.factory),
      ],
      new OpcodeToScriptFunctionMapping("assisted_kill_of", 0, [1]),
      "assisted", "did not assist"
   ),
   new ConditionFunction( // 11
      "Object Has Label",
      "Checks whether an object has a Forge label.",
      "%1 %v label %2.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
         new OpcodeArgBase("label",  OpcodeArgValueForgeLabel.factory),
      ],
      new OpcodeToScriptFunctionMapping("has_label", 0, [1]),
      "has", "does not have"
   ),
   new ConditionFunction( // 12
      "Player Is Not Respawning",
      "Checks whether a player is NOT waiting to respawn.", // TODO: includes loadout cam time?
      "%1 %v waiting to respawn.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
      ],
      new OpcodeToScriptFunctionMapping("is_not_respawning", 0, []),
      "is not", "is"
   ),
   new ConditionFunction( // 13
      "Equipment In Use",
      "",
      "%1 %v in use.",
      [
         new OpcodeArgBase("object", OpcodeArgValueObject.factory),
      ],
      new OpcodeToScriptFunctionMapping("is_in_use", 0),
   ),
   new ConditionFunction( // 14
      "Species Is Spartan",
      "Checks whether a player is a Spartan.",
      "%1 %v a Spartan.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
      ],
      new OpcodeToScriptFunctionMapping("is_spartan", 0),
   ),
   new ConditionFunction( // 15
      "Species Is Elite",
      "Checks whether a player is an Elite.",
      "%1 %v an Elite.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
      ],
      new OpcodeToScriptFunctionMapping("is_elite", 0, [], "is_sangheili"),
   ),
   new ConditionFunction( // 16
      "Species Is Monitor",
      "Checks whether a player is a Monitor.",
      "%1 %v a Monitor.",
      [
         new OpcodeArgBase("player", OpcodeArgValuePlayer.factory),
      ],
      new OpcodeToScriptFunctionMapping("is_monitor", 0),
   ),
   new ConditionFunction( // 17
      "In Matchmaking",
      "This condition is believed to test whether the current match is taking place in Matchmaking.",
      "This match %v taking place in Matchmaking.",
      [],
      new OpcodeToScriptFunctionMapping("is_matchmaking", -2),
   ),
];