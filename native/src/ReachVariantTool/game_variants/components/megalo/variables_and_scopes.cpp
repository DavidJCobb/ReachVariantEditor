#include "variables_and_scopes.h"

namespace Megalo {
   extern const SmartEnum megalo_scope_does_not_have_specifier = SmartEnum(nullptr, 0);
   //
   #define megalo_define_scope(name, ...) namespace { namespace __scope_members { const char* _##name##[] = { __VA_ARGS__ }; } }; extern const SmartEnum name = SmartEnum(__scope_members::_##name##, std::extent<decltype(__scope_members::_##name##)>::value);
   #define megalo_define_scope_with_offset(name, ...) namespace { namespace __scope_members { const char* _##name##[] = { __VA_ARGS__ }; } }; extern const SmartEnum name = SmartEnum(__scope_members::_##name##, std::extent<decltype(__scope_members::_##name##)>::value, true);
   //
   megalo_define_scope(megalo_objects,
      "no object",
      "Global.Object[0]",
      "Global.Object[1]",
      "Global.Object[2]",
      "Global.Object[3]",
      "Global.Object[4]",
      "Global.Object[5]",
      "Global.Object[6]",
      "Global.Object[7]",
      "Global.Object[8]",
      "Global.Object[9]",
      "Global.Object[10]",
      "Global.Object[11]",
      "Global.Object[12]",
      "Global.Object[13]",
      "Global.Object[14]",
      "Global.Object[15]",
      "current object",
      "HUD target object",
      "killed object",
      "killer object",
   );
   megalo_define_scope(megalo_players,
      "no player",
      "Player 1",
      "Player 2",
      "Player 3",
      "Player 4",
      "Player 5",
      "Player 6",
      "Player 7",
      "Player 8",
      "Player 9",
      "Player 10",
      "Player 11",
      "Player 12",
      "Player 13",
      "Player 14",
      "Player 15",
      "Player 16",
      "Global.Player[0]",
      "Global.Player[1]",
      "Global.Player[2]",
      "Global.Player[3]",
      "Global.Player[4]",
      "Global.Player[5]",
      "Global.Player[6]",
      "Global.Player[7]",
      "current player",
      "HUD player",
      "HUD target",
      "killer player",
   );
   megalo_define_scope_with_offset(megalo_teams,
      "no team",
      "Team 1",
      "Team 2",
      "Team 3",
      "Team 4",
      "Team 5",
      "Team 6",
      "Team 7",
      "Team 8",
      "Neutral",
      "Global.Team[0]",
      "Global.Team[1]",
      "Global.Team[2]",
      "Global.Team[3]",
      "Global.Team[4]",
      "Global.Team[5]",
      "Global.Team[6]",
      "Global.Team[7]",
      "current team",
      "HUD player's team",
      "HUD target's team",
      "unk_14 team",
      "unk_15 team",
   );

   extern const VariableScope MegaloVariableScopeGlobal = VariableScope(megalo_scope_does_not_have_specifier, 12, 8, 8, 8, 16);
   extern const VariableScope MegaloVariableScopePlayer = VariableScope(megalo_players, 8, 4, 4, 4, 4);
   extern const VariableScope MegaloVariableScopeObject = VariableScope(megalo_objects, 8, 4, 2, 4, 4);
   extern const VariableScope MegaloVariableScopeTeam   = VariableScope(megalo_teams,   8, 4, 4, 4, 6);

   const VariableScope& getScopeObjectForConstant(variable_scope s) noexcept {
      switch (s) {
         case variable_scope::global:
            return MegaloVariableScopeGlobal;
         case variable_scope::player:
            return MegaloVariableScopePlayer;
         case variable_scope::object:
            return MegaloVariableScopeObject;
         case variable_scope::team:
            return MegaloVariableScopeTeam;
      }
      assert(false && "Unknown variable scope!");
      __assume(0); // suppress "not all paths return a value" by telling MSVC this is unreachable
   }
   variable_scope getScopeConstantForObject(const VariableScope& s) noexcept {
      if (&s == &MegaloVariableScopeGlobal)
         return variable_scope::global;
      if (&s == &MegaloVariableScopePlayer)
         return variable_scope::player;
      if (&s == &MegaloVariableScopeObject)
         return variable_scope::object;
      if (&s == &MegaloVariableScopeTeam)
         return variable_scope::team;
      return variable_scope::not_a_scope;
   }
}