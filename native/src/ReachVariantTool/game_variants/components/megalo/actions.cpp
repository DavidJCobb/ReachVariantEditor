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
         "Creates an object. Note that even if the new object and/or the basis are non-solid, the new object may not be created exactly at the desired position. Attaching and detaching after the object is created is a more reliable way to ensure exact positioning when exact positioning is needed.",
         "Create %1 at offset %6 from %3 with Forge label %4, settings (%5), and name %7. Store a reference to it in %2.",
         {
            OpcodeArgBase("type",   OpcodeArgValueObjectType::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueObject::typeinfo, true),
            OpcodeArgBase("basis",  OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("label",  OpcodeArgValueForgeLabel::typeinfo),
            OpcodeArgBase("flags",  OpcodeArgValueCreateObjectFlags::typeinfo),
            OpcodeArgBase("offset", OpcodeArgValueVector3::typeinfo),
            OpcodeArgBase("name",   OpcodeArgValueVariantStringID::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("place_at_me", "", {0, 3, 4, 5, 6}, 2, OpcodeFuncToScriptMapping::flags::return_value_can_be_discarded)
      ),
      ActionFunction( // 3
         "Delete Object",
         "Delete an object. If the deleted object is a player's current biped, then the player will respawn instantly; in limited-life game variants, they will not lose a life as a result of the deletion. Deleting a player's biped repeatedly for a prolonged period of time will cause the screen to cut to black for them; no HUD widgets will be visible, and most sounds will not be audible.",
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
         "Set Waypoint Timer",
         "Set which timer an object displays, of the timer variables scoped to that object.",
         "Have %1 display %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("timer",  OpcodeArgValueObjectTimerVariable::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_waypoint_timer", "", {1}, 0)
      ),
      ActionFunction( // 8
         "Set Waypoint Distance",
         "",
         "Set the visible range of %1's waypoint to minimum %2, maximum %3.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("min",    OpcodeArgValueScalar::typeinfo), // should be in the range of [-1, 100]
            OpcodeArgBase("max",    OpcodeArgValueScalar::typeinfo), // should be in the range of [-1, 100]
         },
         OpcodeFuncToScriptMapping::make_function("set_waypoint_range", "", {1, 2}, 0)
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
         "Set Weapon Pickup Permissions",
         "Limit who can pick up a weapon. This action does not affect grenades, armor abilities, or vehicles.",
         "Only allow %2 to pick up %1.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("who",    OpcodeArgValuePlayerSet::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_pickup_permissions", "", {1}, 0)
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
         "Set Spawn Location Fireteams",
         "Limit which fireteams can spawn at an object.",
         "Only allow fireteams #%2 to spawn at %1.",
         {
            OpcodeArgBase("spawn location", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("fireteams",      OpcodeArgValueFireteamList::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_spawn_location_fireteams", "", {1}, 0)
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
         OpcodeFuncToScriptMapping::make_function("set_progress_bar", "", {2, 1}, 0) // TODO: should we make this an intrinsic instead? i.e. object_var.timer_var.set_progress_bar_visibility(who) ?
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
         "Modify the rate at which a timer's value changes, and whether the value increases or decreases. If you set the timer to decrease, then its rate will automatically switch to 0% when it reaches zero. If you set the timer to increase, then it will not increase past its initial value (TODO: does this apply if the initial value is zero?).",
         "Have %1 tick at rate %2.",
         {
            OpcodeArgBase("timer", OpcodeArgValueTimer::typeinfo),
            OpcodeArgBase("rate",  OpcodeArgValueTimerRate::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_rate", "", {1}, 0)
      ),
      ActionFunction( // 18
         "Debug Print",
         "Presumably, if this were run in a debug build of Halo: Reach, it'd write a message to some readout or log file.",
         "Log a message if run in a debug build of Halo: Reach: %1.",
         {
            OpcodeArgBase("text", OpcodeArgValueStringTokens2::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("debug_print", "", {0})
      ),
      ActionFunction( // 19
         "Get Item Carrier",
         "Set a player variable to the owner of an object. Testing reveals no cases where this function can fail (i.e. it behaves correctly for dropped objects, none-objects, and objects held by an NPC biped), but Bungie and 343i still manually clear the out-variable before calling it.",
         "Set %2 to the player who is carrying %1.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("try_get_carrier", "get_carrier", {}, 0, OpcodeFuncToScriptMapping::flags::secondary_property_zeroes_result)
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
         "End Round",
         "Ends the round. If the round limit is hit, ends the game as well.",
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
            OpcodeArgBase("silent", OpcodeArgValueConstBool::typeinfo, "without counting toward statistics", "and count its death toward any relevant statistics"),
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
         "Set a number variable to a random value greater than or equal to zero and less than the provided cap (i.e. [0, cap) as the range).",
         "Set %2 to a random number greater than or equal to 0 and less than %1.",
         {
            OpcodeArgBase("cap",    OpcodeArgValueScalar::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("rand", "", {0})
      ),
      ActionFunction( // 26
         "Break into Debugger",
         "Does nothing in Halo: Reach's release build.",
         "If the game is running in a debug build of Halo: Reach, break into the debugger.",
         {},
         OpcodeFuncToScriptMapping::make_function("debug_break", "", {})
      ),
      ActionFunction( // 27
         "Get Object Orientation",
         "Retrieve the object's orientation as an enum between 1 and 6 inclusive, indicating which side of the object is facing up. This can fail, so you should consider resetting the number variable before calling this. Testing with vehicles indicates that the orientation indicates which of the object's local vectors (up, down, etc.) is most closely aligned with world-up, with the vectors numbered from 1 to 6 being: up (i.e. object is upright), left, backward (i.e. nose down), forward (i.e. nose up), left, and down (i.e. object is upside-down). However, testing with pre-placed and pre-rotated weapons and scenery objects always returned 1.",
         "Set %2 to the \"orientation\" value on %1.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("get_orientation", "", {}, 0)
      ),
      ActionFunction( // 28
         "Get Speed",
         "Set a number variable to an object's speed. The speed is premultiplied by 10 and, if less than 0.1, returned as zero. The unit of measurement seems to be feet per second; Race multiplies by 109 and then divides by 100 to convert to kilometers per hour.",
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
         "Debugging: Enable Tracing",
         "Likely does nothing in Halo: Reach's release build.",
         "If the game is running in a debug build, set whether tracing is enabled: %1.",
         {
            OpcodeArgBase("bool", OpcodeArgValueConstBool::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("debug_set_tracing_enabled", "", {0})
      ),
      ActionFunction( // 33
         "Attach Objects",
         "Attach one object to another. The subject will be moved to the target's position plus the offset before being attached. An additional parameter controls whether the offset exists in the target's reference frame (i.e. whether the target's rotation affects the subject's final position); however, the subject will not be rotated (to match the target or otherwise) even if a relative position offset is used.",
         "Attach %1 to %2 at %4 offset %3.",
         {
            OpcodeArgBase("subject",  OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("target",   OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("offset",   OpcodeArgValueVector3::typeinfo),
            OpcodeArgBase("relative", OpcodeArgValueAttachPositionEnum::typeinfo),
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
         "Get a player's position on the scoreboard, across all players and teams. This function returns 0 if the player used is (no_player).",
         "Set %2 to %1's position on the scoreboard.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("get_scoreboard_pos", "", {}, 0)
      ),
      ActionFunction( // 36
         "Get Team Scoreboard Position",
         "Get a team's scoreboard position.",
         "Set %2 to %1's position on the scoreboard.",
         {
            OpcodeArgBase("team",   OpcodeArgValueTeam::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueScalar::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("get_scoreboard_pos", "", {}, 0)
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
         "Modify Player Requisition Money",
         "Modifies the amount of money a player has available to spend on requisitions.",
         "Increment %1's requisition money by %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("number", OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("increment_req_money_by", "", {1}, 0)
      ),
      ActionFunction( // 39
         "Set Player Requisition Purchase Modes",
         "",
         "Set the requisition purchase modes for %1 to %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("flags",  OpcodeArgValuePlayerReqPurchaseModes::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_req_purchase_modes", "", {1}, 0)
      ),
      ActionFunction( // 40
         "Get Player Vehicle",
         "Sets an object variable to the vehicle that a player is currently using.", // does nothing if no player or no vehicle? bungie sets the out-variable to no-object before running this
         "Set %2 to the vehicle that %1 is currently using.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("result", OpcodeArgValueObject::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("try_get_vehicle", "get_vehicle", {}, 0, OpcodeFuncToScriptMapping::flags::secondary_property_zeroes_result)
      ),
      ActionFunction( // 41
         "Force Player Into Vehicle",
         "Forces the player into a vehicle. The player is instantly teleported in. This action will not work if the player is already in a vehicle, or if the target vehicle is destroyed. It does work if the target vehicle is overturned, but the player will be instantly ejected without ever having spent any actual time in the vehicle.",
         "Force %1 into any unoccupied seat in %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("vehicle", OpcodeArgValueObject::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("force_into_vehicle", "", {1}, 0)
      ),
      ActionFunction( // 42
         "Set Player Biped", // Used for Halo Chess's bump-possession?
         "Forces a player to use a specific object as their biped, leaving their original biped behind. This action does nothing if the chosen object is not a valid biped (i.e. the BIPD tag). Once a Megalo-spawned Spartan biped is assigned to a player, its voice will match the player's chosen Spartan gender regardless of the biped's own gender presentation.",
         "Set %1's biped to %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("biped",  OpcodeArgValueObject::typeinfo),
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
         "Modify whether a weapon is picked up automatically or as a hold action. This only works on weapons, not grenades.",
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
         "Modifies the text for a HUD widget. You can stitch values together dynamically. Note that if you display a player's name by way of a player variable, the variable must have \"high\" network priority for the name to display properly off-host.",
         "Set text for %1 to: %2.",
         {
            OpcodeArgBase("widget", OpcodeArgValueWidget::typeinfo),
            OpcodeArgBase("text",   OpcodeArgValueStringTokens2::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_text", "", {1}, 0)
      ),
      ActionFunction( // 47
         "Set Widget Value Text",
         "Modifies an unknown text field for a HUD widget. You can stitch values together dynamically.",
         "Set text for %1 to: %2.",
         {
            OpcodeArgBase("widget", OpcodeArgValueWidget::typeinfo),
            OpcodeArgBase("text",   OpcodeArgValueStringTokens2::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_value_text", "", {1}, 0)
      ),
      ActionFunction( // 48
         "Set Meter Parameters",
         "Modify meter options for a HUD widget. Any variables specified as part of the meter parameters will be passed by reference: the meter will update as their values change, even if this action isn't run again.", // TODO: VERIFY THIS BEHAVIOR
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
            OpcodeArgBase("icon",   OpcodeArgValueHUDWidgetIcon::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_icon", "", {1}, 0)
      ),
      ActionFunction( // 50
         "Set Widget Visibility",
         "Control whether a given player can see a HUD widget.",
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
         "For %3, play sound %1. Immediate? %2.",
         {
            OpcodeArgBase("sound",     OpcodeArgValueSound::typeinfo),
            OpcodeArgBase("immediate", OpcodeArgValueConstBool::typeinfo),
            OpcodeArgBase("who",       OpcodeArgValuePlayerOrGroup::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("play_sound_for", "", {2, 0, 1}, OpcodeFuncToScriptMapping::game_namespace)
      ),
      ActionFunction( // 52
         "Modify Object Scale",
         "Resizes an object. An enlarged object will correctly register hits from bullets and impacts with grenades (including being stuck), but splash damage will not take scaling into account and will apply (or not) based on the object's original size.",
         "Set %1's scale to %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("scale",  OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_scale", "", {1}, 0)
      ),
      ActionFunction( // 53
         "Set Waypoint Text",
         "Sets the text of an object's waypoint. Any variables used with a format string will be passed by reference, not by value, if the variable in question doesn't have network priority \"high,\" and this may lead to results contrary to what you expect.",
         "Set %1's waypoint text to message: %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("text",   OpcodeArgValueStringTokens2::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_waypoint_text", "", {1}, 0)
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
            OpcodeArgBase("icon",   OpcodeArgValueEngineIcon::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_round_card_icon", "", {1}, 0)
      ),
      ActionFunction( // 59
         "Enable/Disable Co-Op Spawning",
         "Co-op Spawning is more commonly known as \"Bro Spawning,\" and is used in Invasion and in Bro Slayer.",
         "%2 co-op spawning for %1.",
         {
            OpcodeArgBase("team", OpcodeArgValueTeam::typeinfo),
            OpcodeArgBase("bool", OpcodeArgValueConstBool::typeinfo, "Enable", "Disable"),
         },
         OpcodeFuncToScriptMapping::make_function("set_co_op_spawning", "", {1}, 0)
      ),
      ActionFunction( // 60
         "Set Primary Respawn Object for Team",
         "",
         "Set %1's primary respawn object: %2.",
         {
            OpcodeArgBase("team",    OpcodeArgValueTeam::typeinfo),
            OpcodeArgBase("respawn", OpcodeArgValueObject::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_primary_respawn_object", "", {1}, 0)
      ),
      ActionFunction( // 61
         "Set Primary Respawn Object for Player",
         "",
         "Set %1's primary respawn object: %2.",
         {
            OpcodeArgBase("player",  OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("respawn", OpcodeArgValueObject::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_primary_respawn_object", "", {1}, 0)
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
         "Returns the distance between two objects. Returned distances are measured in feet; for a decent conversion to meters, multiply by 7 and divide by 23.",
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
         "Animate %1's position to target %2 over %3 seconds. Acceleration seconds: %4; deceleration seconds: %5.",
         {
            OpcodeArgBase("device", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("animation target", OpcodeArgValueScalar::typeinfo),
            OpcodeArgBase("animation duration seconds", OpcodeArgValueScalar::typeinfo),
            OpcodeArgBase("acceleration seconds", OpcodeArgValueScalar::typeinfo),
            OpcodeArgBase("deceleration seconds", OpcodeArgValueScalar::typeinfo),
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
         "Insert Theater Film Marker",
         "",
         "Insert a marker into the saved Film of this match, with number %1 and text: %2.",
         {
            OpcodeArgBase("number", OpcodeArgValueScalar::typeinfo),
            OpcodeArgBase("text",   OpcodeArgValueStringTokens2::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("insert_theater_film_marker", "", {0, 1})
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
            OpcodeArgBase("which",  OpcodeArgValueWeaponSlotEnum::typeinfo),
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
         OpcodeFuncToScriptMapping::make_function("try_get_armor_ability", "get_armor_ability", {}, 0, OpcodeFuncToScriptMapping::flags::secondary_property_zeroes_result)
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
         "Get Player Target Object",
         "Reportedly only works on objects that trigger a red reticle, and reportedly only on host.",
         "Set %2 to the object that %1 has in their crosshairs.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("get_crosshair_target", "", {}, 0)
      ),
      ActionFunction( // 87
         "Create Object Equidistant", // KSoft.Tool now calls this "create_tunnel"; I'm not sure why, because testing confirms it does what I think it does.
         "",
         "Create an instance of %3 at the exact midpoint between %1 and %2, and store it in %5. Radius: %4.",
         {
            OpcodeArgBase("a", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("b", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("type",   OpcodeArgValueObjectType::typeinfo),
            OpcodeArgBase("radius", OpcodeArgValueScalar::typeinfo), // meaning of this arg is unknown; tested different values in-game with no visible effect
            OpcodeArgBase("result", OpcodeArgValueObject::typeinfo, true),
         },
         OpcodeFuncToScriptMapping::make_function("place_between_me_and", "", {1, 2, 3}, 0)
      ),
      ActionFunction( // 88
         "Deprecated-88",
         "A debug function that does nothing in Halo: Reach's release build.",
         "Do nothing. (Unused argument: %1)",
         {
            OpcodeArgBase("number", OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("debug_force_player_view_count", "", {0})
      ),
      ActionFunction( // 89
         "Add Weapon to Player",
         "This function attempts to add a weapon to a player, without having to access their biped.",
         "Give weapon %2 to %1.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("weapon", OpcodeArgValueObject::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("add_weapon", "", {1}, 0)
      ),
      ActionFunction( // 90
         "Set Co-op Spawning for Player",
         "Used in Halo: Reach's standard Race variant?",
         "Set whether co-op spawning is enabled for %1: %2.",
         {
            OpcodeArgBase("player",  OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("enabled", OpcodeArgValueConstBool::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("set_co_op_spawning", "", {1}, 0)
      ),
      ActionFunction( // 91
         "Copy Object Rotation",
         "",
         "Have %1 copy %2's rotation. Use absolute rotations? %3.",
         {
            OpcodeArgBase("a", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("b", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("absolute", OpcodeArgValueConstBool::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("copy_rotation_from", "", {1, 2}, 0)
      ),
      ActionFunction( // 92
         "Point Object Toward Object",
         "Makes one object face toward another. Only seems to affect the subject's heading; pitch and roll rotations don't seem to occur. If you face an object toward itself, you can use the offset position to rotate it.",
         "Make %1 point toward the location in space %3 away from %2.",
         {
            OpcodeArgBase("a", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("b", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("offset", OpcodeArgValueVector3::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("face_toward", "", {1, 2}, 0)
      ),
      ActionFunction( // 93
         "Add Weapon To Biped",
         "Give a weapon to any biped, be it a player or an inanimate script-spawned Spartan, Elite, or Monitor. If the biped's owner player has Weapon Pickup disabled in their player traits, you must use \"force\" to add the weapon.",
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
         "Record Griefer Penalty",
         "The specific function of this action is not known. Presumably it has something to do with keeping track of bad behavior in-game.",
         "Record a griefer penalty for %1, using value %2.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("number", OpcodeArgValueScalar::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("record_griefer_penalty", "", {1}, 0)
      ),
      ActionFunction( // 98
         "Set Shape Owner",
         "Used by KOTH FFA to set a Hill's shape color to the player that controls the Hill. Takes a single argument consisting of a globally-scoped object and a player member on that object.",
         "Change an object's boundary color to match one of its player variables: %1.",
         {
            OpcodeArgBase("object + player", OpcodeArgValueObjectPlayerVariable::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_doubly_contextual_call("apply_shape_color_from_player_member", "", {0}, variable_scope::object, 0) // or perhaps "make_object_owner_property" if it lasts beyond one tick?
      ),
   }};
   extern const ActionFunction& actionFunction_runNestedTrigger = actionFunctionList[20];

   bool Action::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
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
         this->arguments[i] = factory();
         if (this->arguments[i]) {
            this->arguments[i]->configure_with_base(base[i]);
            if (!this->arguments[i]->read(stream, mp)) {
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
   void Action::reset() noexcept {
      Opcode::reset(); // call super
      #if _DEBUG
         this->debug_str.clear();
      #endif
   }
}