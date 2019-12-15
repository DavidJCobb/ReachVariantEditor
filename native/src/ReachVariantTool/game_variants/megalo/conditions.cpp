#include "conditions.h"

void MegaloOpcodeBaseArgument::to_string(std::string& out, const MegaloValue& value) const noexcept {
   out.clear();
   assert(this->type && "MegaloOpcodeBaseArgument should have a type pointer!");
   if (this->type->is_complex()) {
      value.complex.to_string(out);
      return;
   }
   if (this->type->is_simple()) {
      value.simple.to_string(out);
      return;
   }
   #if _DEBUG
      out = "<PRINTING OF SPECIAL TYPES NOT YET IMPLEMENTED>";
   #else
      static_assert(false, "IMPLEMENT ME");
   #endif
}

extern std::vector<MegaloConditionFunction> g_conditionFunctionList = {
   MegaloConditionFunction("None", "This condition does nothing.", "None."),
   //
   MegaloConditionFunction(
      "Compare",
      "Compares the values of two variables.",
      "Variable %1 %v %3 variable %2.", // Variable [foo] [is] [equal to] variable [bar].
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
   MegaloConditionFunction(
      "Object Has Label",
      "Checks whether an object has a given Forge label.",
      "Object %1 %v label %2.", // Object [object] [has] label [INF_HAVEN].
      "has",
      "does not have",
      [](MegaloConditionFunction& cf) {
         cf.arguments.emplace_back("object", reach::value_types::variable_object);
         cf.arguments.emplace_back("label",  reach::value_types::object_label);
      }
   ),
   //
   MegaloConditionFunction(
      "Player Is Not Respawning",
      "Checks whether a player is not currently respawning.",
      "Player %1 %v respawning.", // Player [player] [is not] respawning.
      "is not",
      "is",
      [](MegaloConditionFunction& cf) {
         cf.arguments.emplace_back("player", reach::value_types::variable_player);
      }
   ),
   //
   MegaloConditionFunction(
      "Equipment In Use",
      "",
      "Equipment %1 %v in use.", // Equipment [object] [is] in use.
      [](MegaloConditionFunction& cf) {
         cf.arguments.emplace_back("object", reach::value_types::variable_object);
      }
   ),
   //
   MegaloConditionFunction(
      "Species Is Spartan",
      "Checks whether a player is a Spartan.",
      "Player %1 %v a Spartan.", // Player [player] [is] a Spartan.
      [](MegaloConditionFunction& cf) {
         cf.arguments.emplace_back("player", reach::value_types::variable_player);
      }
   ),
   //
   MegaloConditionFunction(
      "Species Is Elite",
      "Checks whether a player is an Elite.",
      "Player %1 %v an Elite.", // Player [player] [is] an Elite.
      [](MegaloConditionFunction& cf) {
         cf.arguments.emplace_back("player", reach::value_types::variable_player);
      }
   ),
   //
   MegaloConditionFunction(
      "Species Is Monitor",
      "Checks whether a player is a Monitor.",
      "Player %1 %v a Monitor.", // Player [player] [is] a Monitor.
      [](MegaloConditionFunction& cf) {
         cf.arguments.emplace_back("player", reach::value_types::variable_player);
      }
   ),
   //
   MegaloConditionFunction(
      "Unknown 17 (Is Matchmaking?)",
      "Checks whether this match is taking place in Matchmaking?",
      "This match %v taking place in Matchmaking." // This match [is] taking place in Matchmaking.
   ),
};


void MegaloCondition::to_string(std::string& out) const noexcept {
   out.clear();
   //
   auto function = this->function;
   if (!function)
      return;
   auto format = function->format;
   if (!format)
      format = "";
   auto length = strlen(format);
   for (size_t i = 0; i < length; i++) {
      unsigned char c = format[i];
      if (c != '%') {
         out += c;
         continue;
      }
      if (++i >= length)
         break;
      c = format[i];
      if (c == '%') {
         out += c;
         continue;
      }
      if (c == 'v') {
         out += '[';
         if (this->inverted)
            out += function->verb_no;
         else
            out += function->verb_yes;
         out += ']';
      } else if (c >= '1' && c <= '9') {
         c -= '1';
         if (c > this->arguments.size()) {
            out += '%';
            out += (char)(c + '1');
            continue;
         }
         auto& arg_base = function->arguments[c];
         out += '[';
         out += arg_base.name; // PLACEHOLDER; TODO: ACTUALLY SHOW THE VALUE
         out += ':';
         std::string value;
         arg_base.to_string(value, this->arguments[c]);
         out += value;
         out += ']';
      }
   }
}
bool MegaloCondition::read(cobb::bitstream& stream) noexcept {
   uint8_t type = stream.read_bits<uint8_t>(cobb::bitcount(g_conditionFunctionList.size() - 1)); // 5 bits
   if (type == 0)
      return true;
   if (type >= g_conditionFunctionList.size()) {
      printf("WARNING: Condition function ID %u is out of bounds.\n", type);
      return false;
   }
   this->function = &g_conditionFunctionList[type];
   stream.read(this->inverted); // 1 bit
   this->or_group     = stream.read_bits(cobb::bitcount(reach::megalo::max_conditions - 1)); //  9 bits
   this->child_action = stream.read_bits(cobb::bitcount(reach::megalo::max_actions    - 1)); // 10 bits
   //
   auto arg_count = this->function->arguments.size();
   this->arguments.resize(arg_count);
   for (uint8_t i = 0; i < arg_count; i++) {
      auto& arg_base = this->function->arguments[i];
      auto  arg_type = arg_base.type;
      assert(arg_type && "A MegaloConditionFunction should never have an argument whose MegaloValueType is nullptr!");
      //
      auto& arg = this->arguments[i];
      arg.type = arg_type;
      arg.read(stream);
   }
   //
   #if _DEBUG
      this->to_string(this->debug_str);
   #endif
   return true;
}