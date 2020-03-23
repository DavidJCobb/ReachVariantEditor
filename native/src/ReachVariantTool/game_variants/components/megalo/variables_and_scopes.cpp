#include "variables_and_scopes.h"

namespace Megalo {
   extern const SmartEnum megalo_scope_does_not_have_specifier = SmartEnum(nullptr, 0);
   //
   #define megalo_define_scope(name, ...) namespace { namespace __scope_members { const char* _##name##[] = { __VA_ARGS__ }; } }; extern const SmartEnum name = SmartEnum(__scope_members::_##name##, std::extent<decltype(__scope_members::_##name##)>::value);
   #define megalo_define_scope_with_offset(name, ...) namespace { namespace __scope_members { const char* _##name##[] = { __VA_ARGS__ }; } }; extern const SmartEnum name = SmartEnum(__scope_members::_##name##, std::extent<decltype(__scope_members::_##name##)>::value, true);
   //
   megalo_define_scope(megalo_objects,
      "no_object",
      "global.object[0]",
      "global.object[1]",
      "global.object[2]",
      "global.object[3]",
      "global.object[4]",
      "global.object[5]",
      "global.object[6]",
      "global.object[7]",
      "global.object[8]",
      "global.object[9]",
      "global.object[10]",
      "global.object[11]",
      "global.object[12]",
      "global.object[13]",
      "global.object[14]",
      "global.object[15]",
      "current_object",
      "HUD_target_object",
      "killed_object",
      "killer_object",
   );
   megalo_define_scope(megalo_players,
      "no_player",
      "player[0]",
      "player[1]",
      "player[2]",
      "player[3]",
      "player[4]",
      "player[5]",
      "player[6]",
      "player[7]",
      "player[8]",
      "player[9]",
      "player[10]",
      "player[11]",
      "player[12]",
      "player[13]",
      "player[14]",
      "player[15]",
      "global.player[0]",
      "global.player[1]",
      "global.player[2]",
      "global.player[3]",
      "global.player[4]",
      "global.player[5]",
      "global.player[6]",
      "global.player[7]",
      "current_player",
      "HUD_player",
      "HUD_target_player",
      "killer_player",
   );
   megalo_define_scope_with_offset(megalo_teams,
      "no_team",
      "team[0]",
      "team[1]",
      "team[2]",
      "team[3]",
      "team[4]",
      "team[5]",
      "team[6]",
      "team[7]",
      "neutral_team",
      "global.team[0]",
      "global.team[1]",
      "global.team[2]",
      "global.team[3]",
      "global.team[4]",
      "global.team[5]",
      "global.team[6]",
      "global.team[7]",
      "current_team",
      "HUD_player.team",
      "HUD_target_player.team",
      "unk_14_team",
      "unk_15_team",
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