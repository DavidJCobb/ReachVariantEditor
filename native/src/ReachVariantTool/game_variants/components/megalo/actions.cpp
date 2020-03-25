#include "actions.h"
#include "opcode_arg_types/all.h"
#include "../../errors.h"

#define MEGALO_DISALLOW_NONE_ACTION 0
#if _DEBUG
   #undef  MEGALO_DISALLOW_NONE_ACTION
   //#define MEGALO_DISALLOW_NONE_ACTION 1
#endif

namespace Megalo {
   extern std::array<ActionFunction, 99> actionFunctionList = {{
      //
      // The double-curly-braces for this array are NOT a mistake; you should be able to 
      // use single braces but not every compiler handles that correctly.
      //
      ActionFunction( // 0
         "None",
         "This condition does nothing.",
         "None.",
         {},
         OpcodeFuncToScriptMapping()
      ),
      ActionFunction( // 1
         "Modify Score",
         "Modify the score of a player or team.",
         "Modify the score of %1: %2 %3.",
         {
            OpcodeArgBase("target",   OpcodeArgValuePlayerOrGroup::typeinfo),
            OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum::typeinfo),
            OpcodeArgBase("operand",  OpcodeArgValueScalar::typeinfo)
         },
         OpcodeFuncToScriptMapping::make_setter("score", 0, 1)
      ),
      ActionFunction( // 2
         "Create Object",
         "Create an object.",
         "Create %1 at offset %6 from %3 with Forge label %4, settings (%5), and name %7. Store a reference to it in %2.",
         {
            OpcodeArgBase("type",        OpcodeArgValueObjectType::typeinfo),
            OpcodeArgBase("result",      OpcodeArgValueObject::typeinfo, true),
            OpcodeArgBase("spawn point", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("label",       OpcodeArgValueForgeLabel::typeinfo),
            OpcodeArgBase("flags",       OpcodeArgValueCreateObjectFlags::typeinfo),
            OpcodeArgBase("offset",      OpcodeArgValueVector3::typeinfo),
            OpcodeArgBase("name",        OpcodeArgValueVariantStringID::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("place_at_me", "", {0, 3, 4, 5, 6}, 2)
      ),
      ActionFunction( // 3
         "Delete Object",
         "Delete an object.",
         "Delete %1.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("delete", "", {}, 0)
      ),
      ActionFunction( // 4
         "Set Waypoint Visibility",
         "Control the visibility of a waypoint on an object.",
         "Modify waypoint visibility for %1: make visible to %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("who",    OpcodeArgValuePlayerSet::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_waypoint_visibility", "", {1}, 0)
      ),
      ActionFunction( // 5
         "Set Waypoint Icon",
         "Control the icon of a waypoint on an object.",
         "Set %1's waypoint icon to %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("icon",   OpcodeArgValueWaypointIcon::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_waypoint_icon", "", {1}, 0)
      ),
      ActionFunction( // 6
         "Set Waypoint Priority",
         "Control the priority of a waypoint on an object.",
         "Set %1's waypoint priority to %2.",
         {
            OpcodeArgBase("object",   OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("priority", OpcodeArgValueWaypointPriorityEnum::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_waypoint_priority", "", {1}, 0)
      ),
      ActionFunction( // 7
         "Set Object Displayed Timer",
         "Set which timer an object displays, of the timer variables scoped to that object.",
         "Have %1 display %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("timer",  OpcodeArgValueObjectTimerVariable::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_displayed_timer", "", {1}, 0)
      ),
      ActionFunction( // 8
         "Set Object Distance Range",
         "",
         "Set %1's distance range to minimum %2, maximum %3.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("min",    OpcodeArgValueScalar::typeinfo), // should be in the range of [-1, 100]
            OpcodeArgBase("max",    OpcodeArgValueScalar::typeinfo), // should be in the range of [-1, 100]
         },
         OpcodeFuncToScriptMapping::make_function("set_distance_range", "", {1, 2}, 0)
      ),
      ActionFunction( // 9
         "Modify Variable",
         "Modify one variable by another.",
         "Modify variable %1: %3 %2.",
         {
            OpcodeArgBase("a", OpcodeArgValueAnyVariable::typeinfo, true),
            OpcodeArgBase("b", OpcodeArgValueAnyVariable::typeinfo),
            OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_intrinsic_assignment(2)
      ),
      ActionFunction( // 10
         "Set Object Shape",
         "Add a Shape to an object, or replace its existing Shape.",
         "Set %1's shape to %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("shape",  OpcodeArgValueShape::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_shape", "", {1}, 0)
      ),
      ActionFunction( // 11
         "Apply Player Traits",
         "Apply one of the scripted sets of Player Traits to a player. There is no trigger action to remove a set of player traits; rather, you must reapply traits on every frame, and you \"remove\" traits by not reapplying them.",
         "Apply %2 to %1.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("traits", OpcodeArgValuePlayerTraits::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("apply_traits", "", {1}, 0)
      ),
      ActionFunction( // 12
         "Set Object Interact Permissions",
         "Limit who can interact with an object.",
         "Only allow %2 to interact with %1.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("who",    OpcodeArgValuePlayerSet::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_interact_permissions", "", {1}, 0)
      ),
      ActionFunction( // 13
         "Set Spawn Location Permissions",
         "Limit who can spawn at an object.",
         "Only allow %2 to spawn at %1.",
         {
            OpcodeArgBase("spawn location", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("who", OpcodeArgValuePlayerSet::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_spawn_location_permissions", "", {1}, 0)
      ),
      ActionFunction( // 14
         "Set Spawn Location Fireteam",
         "Limit which fireteam can spawn at an object.",
         "Only allow fireteam #%2 to spawn at %1.",
         {
            OpcodeArgBase("spawn location", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("fireteam",       OpcodeArgValueConstSInt8::typeinfo), // TODO: -1 == "no fireteam"
         },
         OpcodeFuncToScriptMapping::make_function("set_spawn_location_fireteam", "", {1}, 0)
      ),
      ActionFunction( // 15
         "Set Object Progress Bar",
         "Set whether an object displays a progress bar.",
         "Have %1 display %3 as a progress bar for %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("who",    OpcodeArgValuePlayerSet::typeinfo),
            OpcodeArgBase("timer",  OpcodeArgValueObjectTimerVariable::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_progress_bar", "", {1, 2}, 0) // TODO: should we make this an intrinsic instead? i.e. object_var.timer_var.set_progress_bar_visibility(who) ?
      ),
      ActionFunction( // 16
         "CHUD Message",
         "",
         "For %1, play sound %2 and display message: %3.",
         {
            OpcodeArgBase("who",   OpcodeArgValuePlayerOrGroup::typeinfo),
            OpcodeArgBase("sound", OpcodeArgValueSound::typeinfo),
            OpcodeArgBase("text",  OpcodeArgValueStringTokens2::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("show_message_to", "", {0, 1, 2}, OpcodeFuncToScriptMapping::game_namespace)
      ),
      ActionFunction( // 17
         "Set Timer Rate",
         "",
         "Have %1 tick at rate %2.",
         {
            OpcodeArgBase("timer", OpcodeArgValueTimer::typeinfo),
            OpcodeArgBase("rate",  OpcodeArgValueTimerRateEnum::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_rate", "", {1}, 0)
      ),
      ActionFunction( // 18
         "Deprecated-18",
         "This trigger action does nothing.",
         "Do nothing. (Unused message: %1)",
         {
            OpcodeArgBase("text", OpcodeArgValueStringTokens2::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("deprecated_18", "", {0})
      ),
      ActionFunction( // 19
         "Get Item Owner",
         "Set a player variable to the owner of an object.",
         "Set %2 to the owner of %1.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("get_owner", "", {}, 0)
      ),
      ActionFunction( // 20
         "Run Nested Trigger",
         "Branches execution to another trigger.",
         "Execute trigger %1.",
         {
            OpcodeArgBase("trigger", OpcodeArgValueTrigger::typeinfo),
         },
         OpcodeFuncToScriptMapping()
      ),
      ActionFunction( // 21
         "End Round With Winner",
         "Ends the round with a winner.",
         "End the current round, and declare the player or team with the highest score the winner of the round.",
         {},
         OpcodeFuncToScriptMapping::make_function("end_round", "", {}, OpcodeFuncToScriptMapping::game_namespace)
      ),
      ActionFunction( // 22
         "Set Object Shape Visibility",
         "Limit who can see an object's Shape.",
         "Only allow %2 to see %1's shape.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("who",    OpcodeArgValuePlayerSet::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_shape_visibility", "", {1}, 0)
      ),
      ActionFunction( // 23
         "Kill Object Instantly",
         "Instantly destroy the specified object.",
         "Destroy %1 %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("silent", OpcodeArgValueConstBool::typeinfo, "silently", "loudly"),
         },
         OpcodeFuncToScriptMapping::make_function("kill", "", {1}, 0)
      ),
      ActionFunction( // 24
         "Set Object Invincibility",
         "Set whether an object is invincible. Invincible objects cannot be destroyed by anything -- even level boundaries.",
         "Modify %1 invincibility: set to %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("invincible (treated as a bool)", OpcodeArgValueScalar::typeinfo), // treated as a bool
         },
         OpcodeFuncToScriptMapping::make_function("set_invincibility", "", {1}, 0)
      ),
      ActionFunction( // 25
         "Random Number",
         "Set a number variable to a random value.",
         "Set %2 to a random number generated using seed %1.",
         {
            OpcodeArgBase("seed",   OpcodeArgValueScalar::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("rand", "", {0})
      ),
      ActionFunction( // 26
         "Deprecated-26",
         "Does nothing.",
         "Do nothing.",
         {},
         OpcodeFuncToScriptMapping::make_function("deprecated_26", "", {})
      ),
      ActionFunction( // 27
         "Unknown-27",
         "Retrieve some unknown value from an object, always between 1 and 7 inclusive. This can fail, so you should consider resetting the number variable before calling this.",
         "Set %2 to the Unknown-27 value on %1.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("unknown_27", "", {}, 0)
      ),
      ActionFunction( // 28
         "Get Speed",
         "Set a number variable to an object's speed. The speed is premultiplied by 10 and, if less than 0.1, returned as zero.",
         "Set integer %2 to the current speed of %1 multiplied by 10.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("get_speed", "", {}, 0)
      ),
      ActionFunction( // 29
         "Get Killer",
         "Set a player variable to whoever most recently killed the target player.", // does nothing if player isn't dead? bungie sets the out-variable to no-player before running this
         "Set %2 to the killer of %1.",
         {
            OpcodeArgBase("victim", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("killer", OpcodeArgValuePlayer::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("try_get_killer", "get_killer", {}, 0, OpcodeFuncToScriptMapping::flags::secondary_property_zeroes_result)
      ),
      ActionFunction( // 30
         "Get Death Damage Type",
         "Set a number variable to indicate what damage type killed a player.",
         "Set integer %2 to the damage type(s) that killed %1.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("try_get_death_damage_type", "get_death_damage_type", {}, 0, OpcodeFuncToScriptMapping::flags::secondary_property_zeroes_result)
      ),
      ActionFunction( // 31
         "Get Death Damage Modifier",
         "Set a number variable to indicate what damage modifier killed a player.", // does nothing if player isn't dead? bungie sets the out-variable to 0 before running this
         "Set integer %2 to the damage modifier(s) that killed %1.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("try_get_death_damage_mod", "get_death_damage_mod", {}, 0, OpcodeFuncToScriptMapping::flags::secondary_property_zeroes_result)
         //
         // Known return values:
         // 1: pummel
         // 2: assassination (does this include backsmacks or just stabs?)
         // 3: splatter
         // 4: stuck with grenade
         // 5: headshot
         //
      ),
      ActionFunction( // 32
         "Unknown-32",
         "",
         "Carry out some unknown action (32) using boolean %1.",
         {
            OpcodeArgBase("bool", OpcodeArgValueConstBool::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("unknown_32", "", {0})
      ),
      ActionFunction( // 33
         "Attach Objects",
         "Attach one object to another.",
         "Attach %1 to %2 at offset %3. Unknown bool: %4.",
         {
            OpcodeArgBase("subject", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("target",  OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("offset",  OpcodeArgValueVector3::typeinfo),
            OpcodeArgBase("bool",    OpcodeArgValueConstBool::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("attach_to", "", {1, 2, 3}, 0)
      ),
      ActionFunction( // 34
         "Detach Objects",
         "Detach an object from any other object it is attached to.",
         "Detach %1 from whatever other object it is attached to.",
         {
            OpcodeArgBase("subject", OpcodeArgValueObject::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("detach", "", {}, 0)
      ),
      ActionFunction( // 35
         "Get Player Scoreboard Position",
         "Get a player's position on the scoreboard, across all players and teams.",
         "Set %2 to %1's position on the scoreboard.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("get_scoreboard_pos", "", {}, 0)
      ),
      ActionFunction( // 36
         "Get Team Index",
         "Get the index of a team variable's value.",
         "Set %2 to %1's index.",
         {
            OpcodeArgBase("team",   OpcodeArgValueTeam::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("get_index", "", {}, 0)
      ),
      ActionFunction( // 37
         "Get Player Killstreak",
         "Returns the number of consecutive kills that a player has accomplished without dying.",
         "Set %2 to the number of consecutive kills %1 has achieved without dying.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("get_spree_count", "", {}, 0)
      ),
      ActionFunction( // 38
         "Unknown-38",
         "",
         "Carry out some unknown action (38) involving %1 and %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("number", OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("unknown_38", "", {1}, 0)
      ),
      ActionFunction( // 39
         "Unknown-39",
         "",
         "Toggle unused flags %2 for %1.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("flags",  OpcodeArgValuePlayerUnusedModeFlags::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("unknown_39", "", {1}, 0)
      ),
      ActionFunction( // 40
         "Get Player Vehicle",
         "Sets an object variable to the vehicle that a player is currently using. Does not include boarding.", // does nothing if no player or no vehicle? bungie sets the out-variable to no-object before running this
         "Set %2 to the vehicle that %1 is currently using.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueObject::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("try_get_vehicle", "get_vehicle", {}, 0, OpcodeFuncToScriptMapping::flags::secondary_property_zeroes_result)
      ),
      ActionFunction( // 41
         "Force Player Into Vehicle",
         "Forces the player into a vehicle. The player is instantly teleported in.",
         "Force %1 into any unoccupied seat in %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("vehicle", OpcodeArgValueObject::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("force_into_vehicle", "", {1}, 0)
      ),
      ActionFunction( // 42
         "Set Player Biped", // Used for Halo Chess's bump-possession?
         "",
         "Set %1's biped to %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_biped", "", {1}, 0)
      ),
      ActionFunction( // 43
         "Reset Timer",
         "Resets a timer.", // to what?
         "Reset %1.",
         {
            OpcodeArgBase("timer", OpcodeArgValueTimer::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("reset", "", {}, 0)
      ),
      ActionFunction( // 44
         "Set Weapon Pickup Priority",
         "Modify whether a weapon is picked up automatically or as a hold action.",
         "Set %1's weapon pickup priority to %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("priority", OpcodeArgValuePickupPriorityEnum::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_weapon_pickup_priority", "", {1}, 0)
      ),
      ActionFunction( // 45
         "Push Object Up",
         "Launches an object upward by a random amount. This is used for things like skulls popping out of heads in Headhunter.",
         "Apply a semi-random amount of upward force to %1.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("push_upward", "apply_upward_impulse", {}, 0)
      ),
      ActionFunction( // 46
         "Set Widget Text",
         "Modifies the text for a HUD widget. You can stitch values together dynamically.",
         "Set text for %1 to: %2.",
         {
            OpcodeArgBase("widget", OpcodeArgValueWidget::typeinfo),
            OpcodeArgBase("text",   OpcodeArgValueStringTokens2::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_text", "", {1}, 0)
      ),
      ActionFunction( // 47
         "Set Widget Text (Unknown)",
         "Modifies an unknown text field for a HUD widget. You can stitch values together dynamically.",
         "Set text for %1 to: %2.",
         {
            OpcodeArgBase("widget", OpcodeArgValueWidget::typeinfo),
            OpcodeArgBase("text",   OpcodeArgValueStringTokens2::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_text_2", "", {1}, 0)
      ),
      ActionFunction( // 48
         "Set Meter Parameters",
         "Modify meter options for a HUD widget.",
         "Set meter parameters for %1 to %2.",
         {
            OpcodeArgBase("widget",     OpcodeArgValueWidget::typeinfo),
            OpcodeArgBase("parameters", OpcodeArgValueMeterParameters::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_meter_params", "", {1}, 0)
      ),
      ActionFunction( // 49
         "Set Widget Icon",
         "Modify a HUD widget's icon.",
         "Set %1's icon to %2.",
         {
            OpcodeArgBase("widget", OpcodeArgValueWidget::typeinfo),
            OpcodeArgBase("icon",   OpcodeArgValueIconIndex6Bits::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_icon", "", {1}, 0)
      ),
      ActionFunction( // 50
         "Set Widget Visibility",
         "Modify a HUD widget's icon.",
         "%3 widget %1 for player %2.",
         {
            OpcodeArgBase("widget",  OpcodeArgValueWidget::typeinfo),
            OpcodeArgBase("player",  OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("visible", OpcodeArgValueConstBool::typeinfo, "Show", "Hide"),
         },
         OpcodeFuncToScriptMapping::make_function("set_visibility", "", {1, 2}, 0)
      ),
      ActionFunction( // 51
         "Play Sound",
         "",
         "For %3, play sound %1. Unknown parameter: %2.",
         {
            OpcodeArgBase("sound",  OpcodeArgValueSound::typeinfo),
            OpcodeArgBase("params", OpcodeArgValueCHUDDestinationEnum::typeinfo),
            OpcodeArgBase("who",    OpcodeArgValuePlayerOrGroup::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("play_sound_for", "", {2, 0, 1}, OpcodeFuncToScriptMapping::game_namespace)
      ),
      ActionFunction( // 52
         "Modify Object Scale",
         "Resizes an object.",
         "Set %1's scale to %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("scale",  OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_scale", "", {1}, 0)
      ),
      ActionFunction( // 53
         "Set Object Display Text",
         "",
         "Set %1's display text to message: %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("text",   OpcodeArgValueStringTokens2::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_display_text", "", {1}, 0)
      ),
      ActionFunction( // 54
         "Get Object Shields",
         "Returns an object's shields as a percentage (i.e. full = 100).",
         "Set %2 to the percentage of shields %1 has remaining.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_getter("shields", 0)
      ),
      ActionFunction( // 55
         "Get Object Health",
         "Returns an object's health as a percentage (i.e. full = 100).",
         "Set %2 to the percentage of health %1 has remaining.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_getter("health", 0)
      ),
      ActionFunction( // 56
         "Set Objective Title for Player",
         "Alters the text of the title card shown at the start of a round.",
         "Set the objective title for %1 to %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("text",   OpcodeArgValueStringTokens2::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_round_card_title", "", {1}, 0)
      ),
      ActionFunction( // 57
         "Set Objective Description for Player",
         "Alters the description of the title card shown at the start of a round.",
         "Set the objective description for %1 to %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("text",   OpcodeArgValueStringTokens2::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_round_card_text", "", {1}, 0)
      ),
      ActionFunction( // 58
         "Set Objective Icon for Player",
         "Alters the icon of the title card shown at the start of a round.",
         "Set the objective icon for %1 to %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("icon",   OpcodeArgValueIconIndex7Bits::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_round_card_icon", "", {1}, 0)
      ),
      ActionFunction( // 59
         "Enable/Disable Fireteam Spawning",
         "Fireteam Spawning is more commonly known as \"Bro Spawning,\" and is used in Invasion and in Bro Slayer.",
         "%2 fireteam spawning for %1.",
         {
            OpcodeArgBase("team", OpcodeArgValueTeam::typeinfo),
            OpcodeArgBase("bool", OpcodeArgValueConstBool::typeinfo, "Enable", "Disable"),
         },
         OpcodeFuncToScriptMapping::make_function("set_fireteam_spawning_enabled", "", {1}, 0)
      ),
      ActionFunction( // 60
         "Set Respawn Object for Team",
         "",
         "Set %1's respawn object: %2.",
         {
            OpcodeArgBase("team",    OpcodeArgValueTeam::typeinfo),
            OpcodeArgBase("respawn", OpcodeArgValueObject::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_respawn_object", "", {1}, 0)
      ),
      ActionFunction( // 61
         "Set Respawn Object for Player",
         "",
         "Set %1's respawn object: %2.",
         {
            OpcodeArgBase("player",  OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("respawn", OpcodeArgValueObject::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_respawn_object", "", {1}, 0)
      ),
      ActionFunction( // 62
         "Get Player Fireteam",
         "Returns a player's fireteam number.",
         "Set %2 to the index of %1's fireteam.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("get_fireteam", "", {}, 0)
      ),
      ActionFunction( // 63
         "Set Player Fireteam",
         "Sets a player's fireteam number.",
         "Assign %1 to fireteam %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("index",  OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_fireteam", "", {1}, 0)
      ),
      ActionFunction( // 64
         "Modify Object Shields",
         "Modifies an object's shields. Remember that 100 = full.",
         "Modify shields for %1: %2 %3.",
         {
            OpcodeArgBase("object",   OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum::typeinfo),
            OpcodeArgBase("operand",  OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_setter("shields", 0, 1)
      ),
      ActionFunction( // 65
         "Modify Object Health",
         "Modifies an object's health. Remember that 100 = full.",
         "Modify health for %1: %2 %3.",
         {
            OpcodeArgBase("object",   OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum::typeinfo),
            OpcodeArgBase("operand",  OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_setter("health", 0, 1)
      ),
      ActionFunction( // 66
         "Get Distance",
         "Returns the distance between two objects.",
         "Set %3 to the distance between %1 and %2.",
         {
            OpcodeArgBase("a", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("b", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("get_distance_to", "", {1}, 0)
      ),
      ActionFunction( // 67
         "Modify Object Max Shields",
         "Modifies an object's max shields. Remember that 100 = full.",
         "Modify max shields for %1: %2 %3.",
         {
            OpcodeArgBase("object",   OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum::typeinfo),
            OpcodeArgBase("operand",  OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_setter("max_shields", 0, 1)
      ),
      ActionFunction( // 68
         "Modify Object Max Health",
         "Modifies an object's max health. Remember that 100 = full.",
         "Modify max health for %1: %2 %3.",
         {
            OpcodeArgBase("object",   OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum::typeinfo),
            OpcodeArgBase("operand",  OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_setter("max_health", 0, 1)
      ),
      ActionFunction( // 69
         "Set Player Requisition Palette",
         "Switches which Requisition Palette a player has access to. This functionality is not used in Halo: Reach.",
         "Switch %1 to %2.",
         {
            OpcodeArgBase("player",  OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("palette", OpcodeArgValueRequisitionPalette::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_requisition_palette", "", {1}, 0)
      ),
      ActionFunction( // 70
         "Set Device Power",
         "",
         "Set the power for %1 to %2.",
         {
            OpcodeArgBase("device", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("power",  OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_device_power", "", {1}, 0)
      ),
      ActionFunction( // 71
         "Get Device Power",
         "",
         "Set %2 to the power for %1.",
         {
            OpcodeArgBase("device", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("power",  OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("get_device_power", "", {}, 0)
      ),
      ActionFunction( // 72
         "Set Device Position",
         "",
         "Set the position for %1 to %2.",
         {
            OpcodeArgBase("device",   OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("position", OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_device_position", "", {1}, 0)
      ),
      ActionFunction( // 73
         "Get Device Position",
         "",
         "Set %2 to the position for %1.",
         {
            OpcodeArgBase("device",   OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("position", OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("get_device_position", "", {}, 0)
      ),
      ActionFunction( // 74
         "Modify Player Grenades",
         "Modify the amount of grenades the player is carrying.",
         "Modify %2 grenade count for %1: %3 %4.",
         {
            OpcodeArgBase("player",   OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("type",     OpcodeArgValueGrenadeTypeEnum::typeinfo),
            OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum::typeinfo),
            OpcodeArgBase("operand",  OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_setter("", 0, 2, 1)
      ),
      ActionFunction( // 75
         "Submit Incident",
         "",
         "Inform the game that incident %1 has occurred, caused by %2 and affecting %3.",
         {
            OpcodeArgBase("incident",  OpcodeArgValueIncident::typeinfo),
            OpcodeArgBase("cause?",    OpcodeArgValuePlayerOrGroup::typeinfo),
            OpcodeArgBase("affected?", OpcodeArgValuePlayerOrGroup::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("send_incident", "", {0, 1, 2})
      ),
      ActionFunction( // 76
         "Submit Incident with value",
         "",
         "Inform the game that incident %1 has occurred, caused by %2 and affecting %3, with value %4.",
         {
            OpcodeArgBase("incident",  OpcodeArgValueIncident::typeinfo),
            OpcodeArgBase("cause?",    OpcodeArgValuePlayerOrGroup::typeinfo),
            OpcodeArgBase("affected?", OpcodeArgValuePlayerOrGroup::typeinfo),
            OpcodeArgBase("value",     OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("send_incident", "", {0, 1, 2, 3})
      ),
      ActionFunction( // 77
         "Set Player Loadout Palette",
         "Set which loadout palette a player or team has access to.",
         "Switch %1 to %2.",
         {
            OpcodeArgBase("team or player",  OpcodeArgValuePlayerOrGroup::typeinfo),
            OpcodeArgBase("loadout palette", OpcodeArgValueLoadoutPalette::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_loadout_palette", "", {1}, 0)
      ),
      ActionFunction( // 78
         "Set Device Position Track",
         "",
         "Set %1's device track and position to %2 and %3, respectively.",
         {
            OpcodeArgBase("device",    OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("animation", OpcodeArgValueVariantStringID::typeinfo),
            OpcodeArgBase("position",  OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_device_animation_position", "", {1, 2}, 0)
      ),
      ActionFunction( // 79
         "Animate Device Position",
         "",
         "Animate %1's position using these values: %2, %3, %4, %5.",
         {
            OpcodeArgBase("device", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("unknown number A", OpcodeArgValueScalar::typeinfo),
            OpcodeArgBase("unknown number B", OpcodeArgValueScalar::typeinfo),
            OpcodeArgBase("unknown number C", OpcodeArgValueScalar::typeinfo),
            OpcodeArgBase("unknown number D", OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("animate_device_position", "", {1, 2, 3, 4}, 0)
      ),
      ActionFunction( // 80
         "Set Device Actual Position",
         "",
         "Set the actual position for %1 to %2.",
         {
            OpcodeArgBase("device",   OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("position", OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_device_actual_position", "", {1}, 0)
      ),
      ActionFunction( // 81
         "Unknown-81",
         "",
         "Carry out an unknown action (81) with number %1 and text: %2.",
         {
            OpcodeArgBase("number", OpcodeArgValueScalar::typeinfo),
            OpcodeArgBase("text",   OpcodeArgValueStringTokens2::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("unknown_81", "", {0, 1})
      ),
      ActionFunction( // 82
         "Enable/Disable Spawn Zone",
         "",
         "Modify enable state for spawn zone %1: set to %2.",
         {
            OpcodeArgBase("spawn zone", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("enable (treated as bool)", OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("enable_spawn_zone", "", {1}, 0)
      ),
      ActionFunction( // 83
         "Get Player Weapon",
         "Returns a player's primary or secondary weapon, as desired.",
         "Set %3 to %1's %2 weapon.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("which",  OpcodeArgValueConstBool::typeinfo, "primary", "secondary"),
            OpcodeArgBase("result", OpcodeArgValueObject::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("try_get_weapon", "get_weapon", {1}, 0, OpcodeFuncToScriptMapping::flags::secondary_property_zeroes_result)
      ),
      ActionFunction( // 84
         "Get Player Armor Ability",
         "Returns a player's armor ability.", // does nothing if no player / no AA? bungie sets the out-variable to no-object before running this
         "Set %2 to %1's Armor Ability.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueObject::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("try_get_armor_ability", "get_armor_ability", {1}, 0, OpcodeFuncToScriptMapping::flags::secondary_property_zeroes_result)
      ),
      ActionFunction( // 85
         "Enable/Disable Object Garbage Collection",
         "Set whether an object can be garbage-collected.",
         "%2 garbage collection of %1.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("state",  OpcodeArgValueConstBool::typeinfo, "Enable", "Disable"),
         },
         OpcodeFuncToScriptMapping::make_function("set_garbage_collection_enabled", "", {1}, 0)
      ),
      ActionFunction( // 86
         "Unknown-86",
         "",
         "Carry out some unknown (86) action with %1 and %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("unknown_86", "", {0, 1})
      ),
      ActionFunction( // 87
         "Create Object Equidistant",
         "",
         "Create an instance of %3 at the exact midpoint between %1 and %2, and store it in %5. Unknown number: %4.",
         {
            OpcodeArgBase("a", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("b", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("type",   OpcodeArgValueObjectType::typeinfo),
            OpcodeArgBase("number", OpcodeArgValueScalar::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueObject::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("place_between_me_and", "", {2, 1, 3, 4}, 0)
      ),
      ActionFunction( // 88
         "Deprecated-88",
         "",
         "Do nothing. (Unused argument: %1)",
         {
            OpcodeArgBase("number", OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("deprecated_88", "", {0})
      ),
      ActionFunction( // 86
         "Unknown-89",
         "",
         "Carry out some unknown (89) action with %1 and %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("unknown_89", "", {0, 1})
      ),
      ActionFunction( // 90
         "Unknown-90",
         "Used in Halo: Reach's standard Race variant?",
         "Carry out some unknown (90) action with %1 and %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("bool",   OpcodeArgValueConstBool::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("unknown_90", "", {1}, 0)
      ),
      ActionFunction( // 91
         "Copy Object Rotation",
         "",
         "Have %1 copy %2's rotation on the %3 axis.",
         {
            OpcodeArgBase("a", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("b", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("axis", OpcodeArgValueConstBool::typeinfo, "axis 1", "axis 0"),
         },
         OpcodeFuncToScriptMapping::make_function("copy_rotation_from", "", {2, 1}, 0)
      ),
      ActionFunction( // 92
         "Move Object Relative To", // needs verification
         "",
         "Move %1 to point %3 relative to %2.",
         {
            OpcodeArgBase("a", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("b", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("point", OpcodeArgValueVector3::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("move_to", "", {1, 2}, 0)
      ),
      ActionFunction( // 93
         "Add Weapon To",
         "Give a weapon to any biped, be it a player or an inanimate script-spawned Spartan, Elite, or Monitor.",
         "Add weapon of type %2 to %1 using mode %3.",
         {
            OpcodeArgBase("biped",  OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("weapon", OpcodeArgValueObjectType::typeinfo),
            OpcodeArgBase("mode",   OpcodeArgValueAddWeaponEnum::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("add_weapon", "", {1, 2}, 0)
      ),
      ActionFunction( // 94
         "Remove Weapon From",
         "Take a weapon from any biped, be it a player or an inanimate script-spawned Spartan, Elite, or Monitor.",
         "Remove %1's %2 weapon, %3.",
         {
            OpcodeArgBase("biped",  OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("which",  OpcodeArgValueDropWeaponEnum::typeinfo),
            OpcodeArgBase("delete", OpcodeArgValueConstBool::typeinfo, "and delete it", "but do not delete it"),
         },
         OpcodeFuncToScriptMapping::make_function("remove_weapon", "", {1, 2}, 0)
      ),
      ActionFunction( // 95
         "Set Scenario Interpolator State",
         "",
         "Set scenario interpolator state: %1, %2.",
         {
            OpcodeArgBase("a", OpcodeArgValueScalar::typeinfo),
            OpcodeArgBase("b", OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_scenario_interpolator_state", "", {0, 1})
      ),
      ActionFunction( // 96
         "Get Random Object With Label",
         "Used by Infection to move the active Haven. Will not select the same Haven twice in a row; if the map has only one Haven, then the containing trigger runs on every frame and spams \"Hill Moved\" announcements.",
         "Set %2 to a random object with label %3, excluding %1 as a possible result.",
         {
            OpcodeArgBase("exclude", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("result",  OpcodeArgValueObject::typeinfo, true),
            OpcodeArgBase("label",   OpcodeArgValueForgeLabel::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("get_random_object", "", {2, 0})
      ),
      ActionFunction( // 97
         "Unknown-97",
         "",
         "Carry out some unknown (97) action with %1 and %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("number", OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("unknown_97", "", {1}, 0)
      ),
      ActionFunction( // 98
         "Set Shape Owner",
         "Used by KOTH FFA to set a Hill's shape color to the player that controls the Hill. Takes a single argument consisting of a globally-scoped object and a player member on that object.",
         "Make an object's boundary visible to one of its player variables?: %1.",
         {
            OpcodeArgBase("object + player", OpcodeArgValueObjectPlayerVariable::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_doubly_contextual_call("apply_color_from_player_member", "", {0}, variable_scope::object, 0) // or perhaps "make_object_owner_property" if it lasts beyond one tick?
      ),
   }};
   extern const ActionFunction& actionFunction_runNestedTrigger = actionFunctionList[20];

   bool Action::read(cobb::ibitreader& stream) noexcept {
      #ifdef _DEBUG
         this->bit_offset = stream.get_bitpos();
      #endif
      {
         auto&  list  = actionFunctionList;
         size_t index = stream.read_bits<size_t>(cobb::bitcount(list.size() - 1));
         if (index >= list.size()) {
            auto& error = GameEngineVariantLoadError::get();
            error.state         = GameEngineVariantLoadError::load_state::failure;
            error.failure_point = GameEngineVariantLoadError::load_failure_point::megalo_actions;
            error.reason        = GameEngineVariantLoadError::load_failure_reason::invalid_script_opcode_function_index;
            error.extra[0]      = index;
            return false;
         }
         this->function = &list[index];
         if (index == 0) { // The "None" condition loads no further data.
            #if MEGALO_DISALLOW_NONE_ACTION == 1
               auto& error = GameEngineVariantLoadError::get();
               error.state         = GameEngineVariantLoadError::load_state::failure;
               error.failure_point = GameEngineVariantLoadError::load_failure_point::megalo_actions;
               error.reason        = GameEngineVariantLoadError::load_failure_reason::script_opcode_cannot_be_none;
               return false;
            #endif
            return true;
         }
      }
      //
      auto&  base     = this->function->arguments;
      size_t argCount = base.size();
      this->arguments.resize(argCount);
      for (size_t i = 0; i < argCount; i++) {
         auto factory = base[i].typeinfo.factory;
         this->arguments[i] = factory(stream);
         if (this->arguments[i]) {
            this->arguments[i]->configure_with_base(base[i]);
            if (!this->arguments[i]->read(stream)) {
               auto& error = GameEngineVariantLoadError::get();
               error.state            = GameEngineVariantLoadError::load_state::failure;
               error.failure_point    = GameEngineVariantLoadError::load_failure_point::megalo_actions;
               error.reason           = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
               error.failure_subindex = i; // (failure_index) must be set by the caller
               return false;
            }
         } else {
            auto& error = GameEngineVariantLoadError::get();
            error.state            = GameEngineVariantLoadError::load_state::failure;
            error.failure_point    = GameEngineVariantLoadError::load_failure_point::megalo_actions;
            error.reason           = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
            if (!error.has_detail()) // some arg factory functions may provide specific failure details
               error.detail = GameEngineVariantLoadError::load_failure_detail::failed_to_construct_script_opcode_arg;
            error.failure_subindex = i; // (failure_index) must be set by the caller
            return false;
         }
      }
      #if _DEBUG
         this->to_string(this->debug_str);
      #endif
      return true;
   }
   void Action::write(cobb::bitwriter& stream) const noexcept {
      {
         size_t index = 0;
         auto&  list  = actionFunctionList;
         for (; index < list.size(); index++)
            if (&list[index] == this->function)
               break;
         if (index == list.size()) {
            assert(false && "Action with unknown function -- can't serialize!");
         }
         stream.write(index, cobb::bitcount(list.size() - 1));
         if (index == 0) // The "None" condition loads no further data.
            return;
      }
      //
      auto& base = this->function->arguments;
      size_t argCount = base.size();
      for (size_t i = 0; i < argCount; i++) {
         auto arg = this->arguments[i];
         arg->write(stream);
      }
   }
   void Action::to_string(std::string& out) const noexcept {
      if (!this->function) {
         out = "<NO FUNC>";
         return;
      }
      auto f = this->function->format;
      assert(f && "Cannot stringify an action whose function has no format string!");
      out.clear();
      size_t l = strlen(f);
      for (size_t i = 0; i < l; i++) {
         unsigned char c = f[i];
         if (c != '%') {
            out += c;
            continue;
         }
         c = f[++i];
         if (c == '%') {
            out += '%';
            continue;
         }
         if (c >= '1' && c <= '9') {
            c -= '1';
            if (c >= this->arguments.size()) {
               out += '%';
               out += (c + '1');
               continue;
            }
            std::string temporary;
            this->arguments[c]->to_string(temporary);
            out += temporary;
         }
      }
   }
   void Action::decompile(Decompiler& out) noexcept {
      this->function->decompile(out, this->arguments);
   }
}