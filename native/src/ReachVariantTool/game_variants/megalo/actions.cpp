#include "actions.h"
#include "opcode_arg_types/all.h"
#include "parse_error_reporting.h"

#define MEGALO_DISALLOW_NONE_ACTION 0
#if _DEBUG
   #undef  MEGALO_DISALLOW_NONE_ACTION
   #define MEGALO_DISALLOW_NONE_ACTION 1
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
         {}
      ),
      ActionFunction( // 1
         "Modify Score",
         "Modify the score of a player or team.",
         "Modify the score of %1: %2 %3.",
         {
            OpcodeArgBase("target",   OpcodeArgTeamOrPlayerVariableFactory),
            OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum::factory),
            OpcodeArgBase("operand",  OpcodeArgValueScalar::factory)
         }
      ),
      ActionFunction( // 2
         "Create Object",
         "Create an object.",
         "Create %1 at offset %6 from %3 with Forge label %4, settings (%5), and name %7. Store a reference to it in %2.",
         {
            OpcodeArgBase("type",        OpcodeArgValueMPObjectTypeIndex::factory),
            OpcodeArgBase("result",      OpcodeArgValueObject::factory, true),
            OpcodeArgBase("spawn point", OpcodeArgValueObject::factory),
            OpcodeArgBase("label",       OpcodeArgValueLabelIndex::factory),
            OpcodeArgBase("flags",       OpcodeArgValueCreateObjectFlags::factory),
            OpcodeArgBase("offset",      OpcodeArgValueVector3::factory),
            OpcodeArgBase("name",        OpcodeArgValueNameIndex::factory),
         }
      ),
      ActionFunction( // 3
         "Delete Object",
         "Delete an object.",
         "Delete %1.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
         }
      ),
      ActionFunction( // 4
         "Set Waypoint Visibility",
         "Control the visibility of a waypoint on an object.",
         "Modify waypoint visibility for %1: make visible to %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("who",    OpcodeArgValuePlayerSet::factory),
         }
      ),
      ActionFunction( // 5
         "Set Waypoint Icon",
         "Control the icon of a waypoint on an object.",
         "Set %1's waypoint icon to %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("icon",   OpcodeArgValueWaypointIcon::factory),
         }
      ),
      ActionFunction( // 6
         "Set Waypoint Priority",
         "Control the priority of a waypoint on an object.",
         "Set %1's waypoint priority to %2.",
         {
            OpcodeArgBase("object",   OpcodeArgValueObject::factory),
            OpcodeArgBase("priority", OpcodeArgValueWaypointPriorityEnum::factory),
         }
      ),
      ActionFunction( // 7
         "Set Object Displayed Timer",
         "Set which timer an object displays.",
         "Have %1 display %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("timer",  OpcodeArgValueObjectTimerVariable::factory),
         }
      ),
      ActionFunction( // 8
         "Set Object Distance Range",
         "",
         "Set %1's distance range to minimum %2, maximum %3.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("min",    OpcodeArgValueScalar::factory), // should be in the range of [-1, 100]
            OpcodeArgBase("max",    OpcodeArgValueScalar::factory), // should be in the range of [-1, 100]
         }
      ),
      ActionFunction( // 9
         "Modify Variable",
         "Modify one variable by another.",
         "Modify variable %1: %3 %2.",
         {
            OpcodeArgBase("a", OpcodeArgAnyVariableFactory, true),
            OpcodeArgBase("b", OpcodeArgAnyVariableFactory),
            OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum::factory),
         }
      ),
      ActionFunction( // 10
         "Set Object Shape",
         "Add a Shape to an object, or replace its existing Shape.",
         "Set %1's shape to %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("shape",  OpcodeArgValueShape::factory),
         }
      ),
      ActionFunction( // 11
         "Apply Player Traits",
         "Apply one of the scripted sets of Player Traits to a player.",
         "Apply %2 to %1.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("traits", OpcodeArgValuePlayerTraits::factory),
         }
      ),
      ActionFunction( // 12
         "Set Object Interact Permissions",
         "Limit who can interact with an object.",
         "Only allow %2 to interact with %1.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("who",    OpcodeArgValuePlayerSet::factory),
         }
      ),
      ActionFunction( // 13
         "Set Spawn Location Permissions",
         "Limit who can spawn at an object.",
         "Only allow %2 to spawn at %1.",
         {
            OpcodeArgBase("spawn location", OpcodeArgValueObject::factory),
            OpcodeArgBase("who", OpcodeArgValuePlayerSet::factory),
         }
      ),
      ActionFunction( // 14
         "Set Spawn Location Fireteam",
         "Limit which fireteam can spawn at an object.",
         "Only allow fireteam #%2 to spawn at %1.",
         {
            OpcodeArgBase("spawn location", OpcodeArgValueObject::factory),
            OpcodeArgBase("fireteam", OpcodeArgValueConstSInt8::factory),
         }
      ),
      ActionFunction( // 15
         "Set Object Progress Bar",
         "Set whether an object displays a progress bar.",
         "Have %1 display %3 as a progress bar for %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("who",    OpcodeArgValuePlayerSet::factory),
            OpcodeArgBase("timer",  OpcodeArgValueObjectTimerVariable::factory),
         }
      ),
      ActionFunction( // 16
         "CHUD Message",
         "",
         "For %1, play sound %2 and display message: %3.",
         {
            OpcodeArgBase("who",   OpcodeArgTeamOrPlayerVariableFactory),
            OpcodeArgBase("sound", OpcodeArgValueSound::factory),
            OpcodeArgBase("text",  OpcodeArgValueStringTokens2::factory),
         }
      ),
      ActionFunction( // 17
         "Set Timer Rate",
         "",
         "Have %1 tick at rate %2.",
         {
            OpcodeArgBase("timer", OpcodeArgValueTimer::factory),
            OpcodeArgBase("rate",  OpcodeArgValueTimerRateEnum::factory),
         }
      ),
      ActionFunction( // 18
         "Deprecated-18",
         "This trigger action does nothing.",
         "Do nothing. (Unused message: %1)",
         {
            OpcodeArgBase("text", OpcodeArgValueStringTokens2::factory),
         }
      ),
      ActionFunction( // 19
         "Get Item Owner",
         "Set a player variable to the owner of an object.",
         "Set %2 to the owner of %1.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory, true),
         }
      ),
      ActionFunction( // 20
         "Run Nested Trigger",
         "Branches execution to another trigger.",
         "Execute trigger %1.",
         {
            OpcodeArgBase("trigger", OpcodeArgValueTrigger::factory),
         }
      ),
      ActionFunction( // 21
         "End Round With Winner",
         "Ends the round with a winner.",
         "End the current round, and declare the player or team with the highest score the winner of the round.",
         {}
      ),
      ActionFunction( // 22
         "Set Object Shape Visibility",
         "Limit who can see an object's Shape.",
         "Only allow %2 to see %1's shape.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("who",    OpcodeArgValuePlayerSet::factory),
         }
      ),
      ActionFunction( // 23
         "Kill Object Instantly",
         "Instantly destroy the specified object.",
         "Destroy %1 %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("silent", OpcodeArgValueConstBool::factory, "silently", "loudly"),
         }
      ),
      ActionFunction( // 24
         "Set Object Invincibility",
         "Set whether an object is invincible.",
         "Modify %1 invincibility: set to %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("invincible (treated as a bool)", OpcodeArgValueScalar::factory), // treated as a bool
         }
      ),
      ActionFunction( // 25
         "Random Number",
         "Set a number variable to a random value.",
         "Set %2 to a random number generated using seed %1.",
         {
            OpcodeArgBase("seed",   OpcodeArgValueScalar::factory),
            OpcodeArgBase("result", OpcodeArgValueScalar::factory, true),
         }
      ),
      ActionFunction( // 26
         "Deprecated-26",
         "Does nothing.",
         "Do nothing.",
         {}
      ),
      ActionFunction( // 27
         "Unknown-27",
         "Retrieve some unknown value from an object, always between 1 and 7 inclusive.",
         "Set %2 to the Unknown-27 value on %1.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("result", OpcodeArgValueScalar::factory, true),
         }
      ),
      ActionFunction( // 28
         "Get Speed",
         "Set a number variable to an object's speed. The speed is premultiplied by 10 and, if less than 0.1, returned as zero.",
         "Set integer %2 to the current speed of %1 multiplied by 10.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("result", OpcodeArgValueScalar::factory, true),
         }
      ),
      ActionFunction( // 29
         "Get Killer",
         "Set a player variable to whoever most recently killed the target player.", // does nothing if player isn't dead? bungie sets the out-variable to no-player before running this
         "Set %2 to the killer of %1.",
         {
            OpcodeArgBase("victim", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("killer", OpcodeArgValuePlayer::factory, true),
         }
      ),
      ActionFunction( // 30
         "Get Death Damage Type",
         "Set a number variable to indicate what damage type killed a player.",
         "Set integer %2 to the damage type(s) that killed %1.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("result", OpcodeArgValueScalar::factory, true),
         }
      ),
      ActionFunction( // 31
         "Get Death Damage Modifier",
         "Set a number variable to indicate what damage modifier killed a player.", // does nothing if player isn't dead? bungie sets the out-variable to 0 before running this
         "Set integer %2 to the damage modifier(s) that killed %1.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("result", OpcodeArgValueScalar::factory, true),
         }
      ),
      ActionFunction( // 32
         "Unknown-32",
         "",
         "Carry out some unknown action (32) using boolean %1.",
         {
            OpcodeArgBase("bool", OpcodeArgValueConstBool::factory),
         }
      ),
      ActionFunction( // 33
         "Attach Objects",
         "Attach one object to another.",
         "Attach %1 to %2 at offset %3. Unknown bool: %4.",
         {
            OpcodeArgBase("subject", OpcodeArgValueObject::factory),
            OpcodeArgBase("target",  OpcodeArgValueObject::factory),
            OpcodeArgBase("offset",  OpcodeArgValueVector3::factory),
            OpcodeArgBase("bool",    OpcodeArgValueConstBool::factory),
         }
      ),
      ActionFunction( // 34
         "Detach Objects",
         "Detach an object from any other object it is attached to.",
         "Detach %1 from whatever other object it is attached to.",
         {
            OpcodeArgBase("subject", OpcodeArgValueObject::factory),
         }
      ),
      ActionFunction( // 35
         "Get Player Scoreboard Position",
         "Get a player's position on the scoreboard, across all players and teams.",
         "Set %2 to %1's position on the scoreboard.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("result", OpcodeArgValueScalar::factory, true),
         }
      ),
      ActionFunction( // 36
         "Get Team Index",
         "Get the index of a team variable's value.",
         "Set %2 to %1's index.",
         {
            OpcodeArgBase("team",   OpcodeArgValueTeam::factory),
            OpcodeArgBase("result", OpcodeArgValueScalar::factory, true),
         }
      ),
      ActionFunction( // 37
         "Get Player Killstreak",
         "Returns the number of consecutive kills that a player has accomplished without dying.",
         "Set %2 to the number of consecutive kills %1 has achieved without dying.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("result", OpcodeArgValueScalar::factory, true),
         }
      ),
      ActionFunction( // 38
         "Unknown-38",
         "",
         "Carry out some unknown action (38) involving %1 and %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("number", OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 39
         "Unknown-39",
         "",
         "Toggle unused flags %2 for %1.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("flags",  OpcodeArgValuePlayerUnusedModeFlags::factory),
         }
      ),
      ActionFunction( // 40
         "Get Player Vehicle",
         "Sets an object variable to the vehicle that a player is currently using. Does not include boarding.", // does nothing if no player or no vehicle? bungie sets the out-variable to no-object before running this
         "Set %2 to the vehicle that %1 is currently using.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("result", OpcodeArgValueObject::factory, true),
         }
      ),
      ActionFunction( // 41
         "Force Player Into Vehicle",
         "Forces the player into a vehicle. The player is instantly teleported in.",
         "Force %1 into any unoccupied seat in %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("vehicle", OpcodeArgValueObject::factory),
         }
      ),
      ActionFunction( // 42
         "Set Player Unit", // Used for Halo Chess's bump-possession?
         "",
         "Set %1's unit to %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
         }
      ),
      ActionFunction( // 43
         "Reset Timer",
         "Resets a timer.", // to what?
         "Reset %1.",
         {
            OpcodeArgBase("timer", OpcodeArgValueTimer::factory),
         }
      ),
      ActionFunction( // 44
         "Set Weapon Pickup Priority",
         "Modify whether a weapon is picked up automatically or as a hold action.",
         "Set %1's weapon pickup priority to %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("priority", OpcodeArgValuePickupPriorityEnum::factory),
         }
      ),
      ActionFunction( // 45
         "Push Object Up",
         "Launches an object upward by a random amount. This is used for things like skulls popping out of heads in Headhunter.",
         "Apply a semi-random amount of upward force to %1.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
         }
      ),
      ActionFunction( // 46
         "Set Widget Text",
         "Modifies the text for a HUD widget. You can stitch values together dynamically.",
         "Set text for %1 to: %2.",
         {
            OpcodeArgBase("widget", OpcodeArgValueWidget::factory),
            OpcodeArgBase("text",   OpcodeArgValueStringTokens2::factory),
         }
      ),
      ActionFunction( // 47
         "Set Widget Text (Unknown)",
         "Modifies an unknown text field for a HUD widget. You can stitch values together dynamically.",
         "Set text for %1 to: %2.",
         {
            OpcodeArgBase("widget", OpcodeArgValueWidget::factory),
            OpcodeArgBase("text",   OpcodeArgValueStringTokens2::factory),
         }
      ),
      ActionFunction( // 48
         "Set Meter Parameters",
         "Modify meter options for a HUD widget.",
         "Set meter parameters for %1 to %2.",
         {
            OpcodeArgBase("widget",     OpcodeArgValueWidget::factory),
            OpcodeArgBase("parameters", OpcodeArgValueMeterParameters::factory),
         }
      ),
      ActionFunction( // 49
         "Set Widget Icon",
         "Modify a HUD widget's icon.",
         "Set %1's icon to %2.",
         {
            OpcodeArgBase("widget", OpcodeArgValueWidget::factory),
            OpcodeArgBase("icon",   OpcodeArgValueIconIndex6Bits::factory),
         }
      ),
      ActionFunction( // 50
         "Set Widget Visibility",
         "Modify a HUD widget's icon.",
         "%3 widget %1 for player %2.",
         {
            OpcodeArgBase("widget",  OpcodeArgValueWidget::factory),
            OpcodeArgBase("player",  OpcodeArgValuePlayer::factory),
            OpcodeArgBase("visible", OpcodeArgValueConstBool::factory, "Show", "Hide"),
         }
      ),
      ActionFunction( // 51
         "Play Sound",
         "",
         "For %3, play sound %1. Unknown parameter: %2.",
         {
            OpcodeArgBase("sound",  OpcodeArgValueSound::factory),
            OpcodeArgBase("params", OpcodeArgValueCHUDDestinationEnum::factory),
            OpcodeArgBase("who",    OpcodeArgTeamOrPlayerVariableFactory),
         }
      ),
      ActionFunction( // 52
         "Modify Object Scale",
         "Resizes an object.",
         "Set %1's scale to %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("result", OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 53
         "Set Object Display Text",
         "",
         "Set %1's display text to message: %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("text",   OpcodeArgValueStringTokens2::factory),
         }
      ),
      ActionFunction( // 54
         "Get Object Shields",
         "Returns an object's shields as a percentage (i.e. full = 100).",
         "Set %2 to the percentage of shields %1 has remaining.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("result", OpcodeArgValueScalar::factory, true),
         }
      ),
      ActionFunction( // 55
         "Get Object Health",
         "Returns an object's health as a percentage (i.e. full = 100).",
         "Set %2 to the percentage of health %1 has remaining.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("result", OpcodeArgValueScalar::factory, true),
         }
      ),
      ActionFunction( // 56
         "Set Objective Title for Player",
         "Alters the text of the title card shown at the start of a round.",
         "Set the objective title for %1 to %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("text",   OpcodeArgValueStringTokens2::factory),
         }
      ),
      ActionFunction( // 57
         "Set Objective Description for Player",
         "Alters the description of the title card shown at the start of a round.",
         "Set the objective description for %1 to %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("text",   OpcodeArgValueStringTokens2::factory),
         }
      ),
      ActionFunction( // 58
         "Set Objective Icon for Player",
         "Alters the icon of the title card shown at the start of a round.",
         "Set the objective icon for %1 to %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("icon",   OpcodeArgValueIconIndex7Bits::factory),
         }
      ),
      ActionFunction( // 59
         "Enable/Disable Fireteam Spawning",
         "Fireteam Spawning is more commonly known as \"Bro Spawning,\" and is used in Invasion and in Bro Slayer.",
         "%2 fireteam spawning for %1.",
         {
            OpcodeArgBase("team", OpcodeArgValueTeam::factory),
            OpcodeArgBase("bool", OpcodeArgValueConstBool::factory, "Enable", "Disable"),
         }
      ),
      ActionFunction( // 60
         "Set Respawn Object for Team",
         "",
         "Set %1's respawn object: %2.",
         {
            OpcodeArgBase("team",    OpcodeArgValueTeam::factory),
            OpcodeArgBase("respawn", OpcodeArgValueObject::factory),
         }
      ),
      ActionFunction( // 61
         "Set Respawn Object for Player",
         "",
         "Set %1's respawn object: %2.",
         {
            OpcodeArgBase("player",  OpcodeArgValuePlayer::factory),
            OpcodeArgBase("respawn", OpcodeArgValueObject::factory),
         }
      ),
      ActionFunction( // 62
         "Get Player Fireteam",
         "Returns a player's fireteam number.",
         "Set %2 to the index of %1's fireteam.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("result", OpcodeArgValueScalar::factory, true),
         }
      ),
      ActionFunction( // 63
         "Set Player Fireteam",
         "Sets a player's fireteam number.",
         "Assign %1 to fireteam %2..",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("result", OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 64
         "Modify Object Shields",
         "Modifies an object's shields. Remember that 100 = full.",
         "Modify shields for %1: %2 %3.",
         {
            OpcodeArgBase("object",   OpcodeArgValueObject::factory),
            OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum::factory),
            OpcodeArgBase("operand",  OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 65
         "Modify Object Health",
         "Modifies an object's health. Remember that 100 = full.",
         "Modify health for %1: %2 %3.",
         {
            OpcodeArgBase("object",   OpcodeArgValueObject::factory),
            OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum::factory),
            OpcodeArgBase("operand",  OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 66
         "Get Distance",
         "Returns the distance between two objects.",
         "Set %3 to the distance between %1 and %2.",
         {
            OpcodeArgBase("a", OpcodeArgValueObject::factory),
            OpcodeArgBase("b", OpcodeArgValueObject::factory),
            OpcodeArgBase("result", OpcodeArgValueScalar::factory, true),
         }
      ),
      ActionFunction( // 67
         "Modify Object Max Shields",
         "Modifies an object's max shields. Remember that 100 = full.",
         "Modify max shields for %1: %2 %3.",
         {
            OpcodeArgBase("object",   OpcodeArgValueObject::factory),
            OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum::factory),
            OpcodeArgBase("operand",  OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 68
         "Modify Object Max Health",
         "Modifies an object's max health. Remember that 100 = full.",
         "Modify max health for %1: %2 %3.",
         {
            OpcodeArgBase("object",   OpcodeArgValueObject::factory),
            OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum::factory),
            OpcodeArgBase("operand",  OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 69
         "Set Player Requisition Palette",
         "Switches which Requisition Palette a player has access to. This functionality is not used in Halo: Reach.",
         "Switch %1 to %2.",
         {
            OpcodeArgBase("player",  OpcodeArgValuePlayer::factory),
            OpcodeArgBase("palette", OpcodeArgValueRequisitionPalette::factory),
         }
      ),
      ActionFunction( // 70
         "Set Device Power",
         "",
         "Set the power for %1 to %2.",
         {
            OpcodeArgBase("device", OpcodeArgValueObject::factory),
            OpcodeArgBase("power",  OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 71
         "Get Device Power",
         "",
         "Set %2 to the power for %1.",
         {
            OpcodeArgBase("device", OpcodeArgValueObject::factory),
            OpcodeArgBase("power",  OpcodeArgValueScalar::factory, true),
         }
      ),
      ActionFunction( // 72
         "Set Device Position",
         "",
         "Set the position for %1 to %2.",
         {
            OpcodeArgBase("device",   OpcodeArgValueObject::factory),
            OpcodeArgBase("position", OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 73
         "Get Device Position",
         "",
         "Set %2 to the position for %1.",
         {
            OpcodeArgBase("device",   OpcodeArgValueObject::factory),
            OpcodeArgBase("position", OpcodeArgValueScalar::factory, true),
         }
      ),
      ActionFunction( // 74
         "Modify Player Grenades",
         "Modify the amount of grenades the player is carrying.",
         "Modify %2 grenade count for %1: %3 %4.",
         {
            OpcodeArgBase("player",   OpcodeArgValuePlayer::factory),
            OpcodeArgBase("type",     OpcodeArgValueGrenadeTypeEnum::factory),
            OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum::factory),
            OpcodeArgBase("operand",  OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 75
         "Submit Incident",
         "",
         "Inform the game that incident %1 has occurred, caused by %2 and affecting %3.",
         {
            OpcodeArgBase("incident",  OpcodeArgValueIncidentID::factory),
            OpcodeArgBase("cause?",    OpcodeArgTeamOrPlayerVariableFactory),
            OpcodeArgBase("affected?", OpcodeArgTeamOrPlayerVariableFactory),
         }
      ),
      ActionFunction( // 76
         "Submit Incident with value",
         "",
         "Inform the game that incident %1 has occurred, caused by %2 and affecting %3, with value %4.",
         {
            OpcodeArgBase("incident",  OpcodeArgValueIncidentID::factory),
            OpcodeArgBase("cause?",    OpcodeArgTeamOrPlayerVariableFactory),
            OpcodeArgBase("affected?", OpcodeArgTeamOrPlayerVariableFactory),
            OpcodeArgBase("value",     OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 77
         "Set Player Loadout Palette",
         "Set which loadout palette a player or team has access to.",
         "Switch %1 to %2.",
         {
            OpcodeArgBase("team or player",  OpcodeArgTeamOrPlayerVariableFactory),
            OpcodeArgBase("loadout palette", OpcodeArgValueLoadoutPalette::factory),
         }
      ),
      ActionFunction( // 78
         "Set Device Position Track",
         "",
         "Set %1's device track and position to %2 and %3, respectively.",
         {
            OpcodeArgBase("device",    OpcodeArgValueObject::factory),
            OpcodeArgBase("animation", OpcodeArgValueNameIndex::factory),
            OpcodeArgBase("position",  OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 79
         "Animate Device Position",
         "",
         "Animate %1's position using these values: %2, %3, %4, %5.",
         {
            OpcodeArgBase("device", OpcodeArgValueObject::factory),
            OpcodeArgBase("unknown number A", OpcodeArgValueScalar::factory),
            OpcodeArgBase("unknown number B", OpcodeArgValueScalar::factory),
            OpcodeArgBase("unknown number C", OpcodeArgValueScalar::factory),
            OpcodeArgBase("unknown number D", OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 80
         "Set Device Actual Position",
         "",
         "Set the actual position for %1 to %2.",
         {
            OpcodeArgBase("device",   OpcodeArgValueObject::factory),
            OpcodeArgBase("position", OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 81
         "Unknown-81",
         "",
         "Carry out an unknown action (81) with number %1 and text: %2.",
         {
            OpcodeArgBase("number", OpcodeArgValueScalar::factory),
            OpcodeArgBase("text",   OpcodeArgValueStringTokens2::factory),
         }
      ),
      ActionFunction( // 82
         "Enable/Disable Spawn Zone",
         "",
         "Modify enable state for spawn zone %1: set to %2.",
         {
            OpcodeArgBase("spawn zone", OpcodeArgValueObject::factory),
            OpcodeArgBase("enable (treated as bool)", OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 83
         "Get Player Weapon",
         "Returns a player's primary or secondary weapon, as desired.",
         "Set %3 to %1's %2 weapon.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("which", OpcodeArgValueConstBool::factory, "primary", "secondary"),
            OpcodeArgBase("result", OpcodeArgValueObject::factory, true),
         }
      ),
      ActionFunction( // 84
         "Get Player Armor Ability",
         "Returns a player's armor ability.", // does nothing if no player / no AA? bungie sets the out-variable to no-object before running this
         "Set %2 to %1's Armor Ability.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("result", OpcodeArgValueObject::factory, true),
         }
      ),
      ActionFunction( // 85
         "Enable/Disable Object Garbage Collection",
         "Returns a player's armor ability.",
         "%2 garbage collection of %1.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("state",  OpcodeArgValueConstBool::factory, "Enable", "Disable"),
         }
      ),
      ActionFunction( // 86
         "Unknown-86",
         "",
         "Carry out some unknown (86) action with %1 and %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
         }
      ),
      ActionFunction( // 87
         "Create Object Equidistant",
         "",
         "Create an instance of %3 at the exact midpoint between %1 and %2, and store it in %5. Unknown number: %4.",
         {
            OpcodeArgBase("a", OpcodeArgValueObject::factory),
            OpcodeArgBase("b", OpcodeArgValueObject::factory),
            OpcodeArgBase("type",   OpcodeArgValueMPObjectTypeIndex::factory),
            OpcodeArgBase("number", OpcodeArgValueScalar::factory),
            OpcodeArgBase("result", OpcodeArgValueObject::factory, true),
         }
      ),
      ActionFunction( // 88
         "Deprecated-88",
         "",
         "Do nothing. (Unused argument: %1)",
         {
            OpcodeArgBase("number", OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 86
         "Unknown-89",
         "",
         "Carry out some unknown (89) action with %1 and %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
         }
      ),
      ActionFunction( // 90
         "Unknown-90",
         "Used in Halo: Reach's standard Race variant?",
         "Carry out some unknown (90) action with %1 and %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("bool",   OpcodeArgValueConstBool::factory),
         }
      ),
      ActionFunction( // 91
         "Copy Object Rotation",
         "",
         "Have %1 copy %2's rotation on the %3 axis.",
         {
            OpcodeArgBase("a", OpcodeArgValueObject::factory),
            OpcodeArgBase("b", OpcodeArgValueObject::factory),
            OpcodeArgBase("axis", OpcodeArgValueConstBool::factory, "axis 1", "axis 0"),
         }
      ),
      ActionFunction( // 92
         "Move Object Relative To", // needs verification
         "",
         "Move %1 to point %3 relative to %2.",
         {
            OpcodeArgBase("a", OpcodeArgValueObject::factory),
            OpcodeArgBase("b", OpcodeArgValueObject::factory),
            OpcodeArgBase("point", OpcodeArgValueVector3::factory),
         }
      ),
      ActionFunction( // 93
         "Add Weapon To",
         "Give a weapon to any biped, be it a player or an inanimate script-spawned Spartan, Elite, or Monitor.",
         "Add weapon of type %2 to %1 using mode %3.",
         {
            OpcodeArgBase("biped",  OpcodeArgValueObject::factory),
            OpcodeArgBase("weapon", OpcodeArgValueMPObjectTypeIndex::factory),
            OpcodeArgBase("mode",   OpcodeArgValueAddWeaponEnum::factory),
         }
      ),
      ActionFunction( // 94
         "Remove Weapon From",
         "Take a weapon from any biped, be it a player or an inanimate script-spawned Spartan, Elite, or Monitor.",
         "Remove %1's %2 weapon, %3.",
         {
            OpcodeArgBase("biped",  OpcodeArgValueObject::factory),
            OpcodeArgBase("which",  OpcodeArgValueDropWeaponEnum::factory),
            OpcodeArgBase("delete", OpcodeArgValueConstBool::factory, "and delete it", "but do not delete it"),
         }
      ),
      ActionFunction( // 95
         "Set Scenario Interpolator State",
         "",
         "Set scenario interpolator state: %1, %2.",
         {
            OpcodeArgBase("a", OpcodeArgValueScalar::factory),
            OpcodeArgBase("b", OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 96
         "Unknown-96",
         "Used in CTF to return the flag (none, flag, return Forge label).",
         "Carry out some unknown (96) action with %1 and %3 and store the result in %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("result", OpcodeArgValueObject::factory, true),
            OpcodeArgBase("label",  OpcodeArgValueLabelIndex::factory),
         }
      ),
      ActionFunction( // 97
         "Unknown-97",
         "",
         "Carry out some unknown (96) action with %1 and %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::factory),
            OpcodeArgBase("number", OpcodeArgValueScalar::factory),
         }
      ),
      ActionFunction( // 98
         "Set Boundary Visibility Filter", // TODO: figure out what this even means; current speculation is based on Halo 4's known actions
         "",
         "Make an object's boundary visible to one of its player variables?: %1.",
         {
            OpcodeArgBase("object + player", OpcodeArgValueObjectPlayerVariable::factory),
         }
      ),
   }};
   extern const ActionFunction& actionFunction_runNestedTrigger = actionFunctionList[20];

   bool Action::read(cobb::bitreader& stream) noexcept {
      {
         auto&  list  = actionFunctionList;
         size_t index = stream.read_bits<size_t>(cobb::bitcount(list.size() - 1));
         if (index >= list.size()) {
            printf("Bad action function ID %d.\n", index);
            //
            auto& error = ParseState::get();
            error.signalled = true;
            error.opcode    = ParseState::opcode_type::action;
            error.cause     = ParseState::what::bad_opcode_id;
            error.extra[0]  = index;
            return false;
         }
         this->function = &list[index];
         if (index == 0) { // The "None" condition loads no further data.
            #if MEGALO_DISALLOW_NONE_ACTION == 1
               auto& error = ParseState::get();
               error.signalled = true;
               error.opcode    = ParseState::opcode_type::action;
               error.cause     = ParseState::what::none_opcode_not_allowed;
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
         auto factory = base[i].factory;
         this->arguments[i] = factory(stream);
         if (this->arguments[i]) {
            this->arguments[i]->configure_with_base(base[i]);
            if (!this->arguments[i]->read(stream)) {
               printf("Failed to load argument %d for action %s.\n", i, this->function->name);
               //
               auto& error = ParseState::get();
               error.signalled = true;
               error.opcode    = ParseState::opcode_type::action;
               error.opcode_arg_index = i;
               return false;
            }
         } else {
            printf("Failed to construct argument %d for action %s.\n", i, this->function->name);
            //
            auto& error = ParseState::get();
            error.signalled = true;
            error.opcode    = ParseState::opcode_type::action;
            error.opcode_arg_index = i;
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
            assert(false && "Condition with unknown function -- can't serialize!");
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
         auto factory = base[i].factory;
         //
         // This is really ugly but I've sort of painted myself into a corner here... Some 
         // arguments can take multiple variable types, and currently the variable classes 
         // have no way of "knowing" that that's how they got here.
         //
         if (factory == OpcodeArgAnyVariableFactory) {
            stream.write((uint8_t)arg->get_variable_type(), 3);
         } else if (factory == OpcodeArgTeamOrPlayerVariableFactory) {
            switch (arg->get_variable_type()) {
               case variable_type::team:
                  stream.write(0, 2);
                  break;
               case variable_type::player:
                  stream.write(1, 2);
                  break;
               case variable_type::not_a_variable:
                  stream.write(2, 2);
                  break;
            }
         }
         //
         // Now we can serialize the argument value.
         //
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
}