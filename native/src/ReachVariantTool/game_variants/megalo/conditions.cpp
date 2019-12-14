#include "conditions.h"

MegaloConditionFunctionList::MegaloConditionFunctionList() {
   MegaloConditionFunction conditions[] = {
      MegaloConditionFunction("None", "This condition does nothing.", "None."),
      //
      MegaloConditionFunction(
         "Compare",
         "Compares the values of two variables.",
         "Variable %1 %v %3 varable %2.", // Variable [foo] [is] [equal to] variable [bar].
         [](MegaloConditionFunction& cf) {
            cf.arguments.emplace_back("a", reach::value_types::variable_any);
            cf.arguments.emplace_back("b", reach::value_types::variable_any);
            cf.arguments.emplace_back("operator", reach::value_types::compare_operator);
         }
      ),
      //
      MegaloConditionFunction(
         "In Boundary",
         "Checks whether one object is inside of another object's Shape.",
         "%1 %v inside of %2's shape.", // [Object] [is] inside of [object]'s shape.
         [](MegaloConditionFunction& cf) {
            cf.arguments.emplace_back("a", reach::value_types::variable_object);
            cf.arguments.emplace_back("b", reach::value_types::variable_object);
         }
      ),
      //
      MegaloConditionFunction(
         "Killer Type",
         "Checks what killed a player.",
         "%1 %v killed by any of: %2.", // [Player] [was] killed by any of: [guardians, suicide].
         "was",
         "was not",
         [](MegaloConditionFunction& cf) {
            cf.arguments.emplace_back("victim",       reach::value_types::variable_player);
            cf.arguments.emplace_back("killer types", reach::value_types::killer_type_flags);
         }
      ),
      //
      MegaloConditionFunction(
         //
         // Is this "alliance status?"
         //
         "Team Disposition",
         "Unknown.",
         "Team %1 %v disposition %3 with team %2.", // Team [team] [has] disposition [unk_1] with team [team].
         "has",
         "does not have",
         [](MegaloConditionFunction& cf) {
            cf.arguments.emplace_back("a", reach::value_types::variable_team);
            cf.arguments.emplace_back("b", reach::value_types::variable_team);
            cf.arguments.emplace_back("disposition", reach::value_types::team_disposition);
         }
      ),
      //
      MegaloConditionFunction(
         "Timer Is Zero",
         "Checks whether a timer is zero.",
         "Timer %1 %v zero.", // Timer [timer] [is] zero.
         [](MegaloConditionFunction& cf) {
            cf.arguments.emplace_back("timer", reach::value_types::variable_timer);
         }
      ),
      //
      MegaloConditionFunction(
         "Objects Are Same Type",
         "Checks whether two objects are of the same type.", // verify
         "Object %1 %v of the same type as %2.", // Object [object] [is] of the same type as [current object].
         [](MegaloConditionFunction& cf) {
            cf.arguments.emplace_back("object", reach::value_types::variable_object);
            cf.arguments.emplace_back("other",  reach::value_types::explicit_object);
         }
      ),
      //
      MegaloConditionFunction(
         "Team Has Players",
         "Checks whether a team has any players on it.",
         "Team %1 %v one or more players.", // Team [team] [has] one or more players.
         "has",
         "does not have",
         [](MegaloConditionFunction& cf) {
            cf.arguments.emplace_back("team", reach::value_types::variable_team);
         }
      ),
      //
      MegaloConditionFunction(
         "Object Out Of Bounds",
         "Checks whether an object has fallen out of bounds, e.g. into a Soft Kill Zone.",
         "Object %1 %v out of bounds.", // Object [object] [is] out of bounds.
         [](MegaloConditionFunction& cf) {
            cf.arguments.emplace_back("object", reach::value_types::variable_object);
         }
      ),
      //
      MegaloConditionFunction(
         "Deprecated 09",
         "This condition function always returns false.", // TODO: does it return (true) if inverted?
         "Never. (Unused argument: %1)", // Never. (Unused argument: [player])
         [](MegaloConditionFunction& cf) {
            cf.arguments.emplace_back("player", reach::value_types::variable_player);
         }
      ),
      //
      MegaloConditionFunction(
         "Player Assisted Kill Of",
         "Checks whether one player assisted in the slaying of another player. Note that you don't count as \"assisting\" your own kills.",
         "Player %1 %v in the killing of player %2.", // Player [attacker] [assisted] in the killing of player [victim].
         "assisted",
         "did not assist",
         [](MegaloConditionFunction& cf) {
            cf.arguments.emplace_back("attacker", reach::value_types::variable_player);
            cf.arguments.emplace_back("victim",   reach::value_types::variable_player);
         }
      ),
      //
      // TODO: FINISH ME
      //
   };
   //
   this->list = (MegaloConditionFunction*)malloc(sizeof(conditions));
   memcpy(this->list, &list, sizeof(conditions));
   this->size = std::extent<decltype(conditions)>::value;
}
MegaloConditionFunctionList::~MegaloConditionFunctionList() {
   if (this->list)
      free(this->list);
}