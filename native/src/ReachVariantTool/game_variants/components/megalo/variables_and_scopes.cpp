#include "variables_and_scopes.h"

namespace Megalo {
   namespace variable_which_values {
      using flags = VariableScopeWhichValue::flag;
      //
      namespace object {
         extern const VariableScopeWhichValueList list = VariableScopeWhichValueList({
            &no_object,
            &global_0,
            &global_1,
            &global_2,
            &global_3,
            &global_4,
            &global_5,
            &global_6,
            &global_7,
            &global_8,
            &global_9,
            &global_10,
            &global_11,
            &global_12,
            &global_13,
            &global_14,
            &global_15,
            &current,
            &hud_target,
            &killed,
            &killer,
         });
         //
         extern VariableScopeWhichValue no_object  = VariableScopeWhichValue("no_object", flags::is_none);
         extern VariableScopeWhichValue global_0   = VariableScopeWhichValue("global.object[0]");
         extern VariableScopeWhichValue global_1   = VariableScopeWhichValue("global.object[1]");
         extern VariableScopeWhichValue global_2   = VariableScopeWhichValue("global.object[2]");
         extern VariableScopeWhichValue global_3   = VariableScopeWhichValue("global.object[3]");
         extern VariableScopeWhichValue global_4   = VariableScopeWhichValue("global.object[4]");
         extern VariableScopeWhichValue global_5   = VariableScopeWhichValue("global.object[5]");
         extern VariableScopeWhichValue global_6   = VariableScopeWhichValue("global.object[6]");
         extern VariableScopeWhichValue global_7   = VariableScopeWhichValue("global.object[7]");
         extern VariableScopeWhichValue global_8   = VariableScopeWhichValue("global.object[8]");
         extern VariableScopeWhichValue global_9   = VariableScopeWhichValue("global.object[9]");
         extern VariableScopeWhichValue global_10  = VariableScopeWhichValue("global.object[10]");
         extern VariableScopeWhichValue global_11  = VariableScopeWhichValue("global.object[11]");
         extern VariableScopeWhichValue global_12  = VariableScopeWhichValue("global.object[12]");
         extern VariableScopeWhichValue global_13  = VariableScopeWhichValue("global.object[13]");
         extern VariableScopeWhichValue global_14  = VariableScopeWhichValue("global.object[14]");
         extern VariableScopeWhichValue global_15  = VariableScopeWhichValue("global.object[15]");
         extern VariableScopeWhichValue current    = VariableScopeWhichValue("current_object", flags::is_read_only);
         extern VariableScopeWhichValue hud_target = VariableScopeWhichValue("hud_target_object", flags::is_read_only);
         extern VariableScopeWhichValue killed     = VariableScopeWhichValue("killed_object", flags::is_read_only);
         extern VariableScopeWhichValue killer     = VariableScopeWhichValue("killer_object", flags::is_read_only);
      }
      namespace player {
         extern const VariableScopeWhichValueList list = VariableScopeWhichValueList({
            &no_player,
            &player_0,
            &player_1,
            &player_2,
            &player_3,
            &player_4,
            &player_5,
            &player_6,
            &player_7,
            &player_8,
            &player_9,
            &player_10,
            &player_11,
            &player_12,
            &player_13,
            &player_14,
            &player_15,
            &global_0,
            &global_1,
            &global_2,
            &global_3,
            &global_4,
            &global_5,
            &global_6,
            &global_7,
            &current,
            &hud,
            &hud_target,
            &killer,
         });
         //
         extern VariableScopeWhichValue no_player  = VariableScopeWhichValue("no_player", flags::is_none);
         extern VariableScopeWhichValue player_0   = VariableScopeWhichValue("player[0]", flags::is_read_only);
         extern VariableScopeWhichValue player_1   = VariableScopeWhichValue("player[1]", flags::is_read_only);
         extern VariableScopeWhichValue player_2   = VariableScopeWhichValue("player[2]", flags::is_read_only);
         extern VariableScopeWhichValue player_3   = VariableScopeWhichValue("player[3]", flags::is_read_only);
         extern VariableScopeWhichValue player_4   = VariableScopeWhichValue("player[4]", flags::is_read_only);
         extern VariableScopeWhichValue player_5   = VariableScopeWhichValue("player[5]", flags::is_read_only);
         extern VariableScopeWhichValue player_6   = VariableScopeWhichValue("player[6]", flags::is_read_only);
         extern VariableScopeWhichValue player_7   = VariableScopeWhichValue("player[7]", flags::is_read_only);
         extern VariableScopeWhichValue player_8   = VariableScopeWhichValue("player[8]", flags::is_read_only);
         extern VariableScopeWhichValue player_9   = VariableScopeWhichValue("player[9]", flags::is_read_only);
         extern VariableScopeWhichValue player_10  = VariableScopeWhichValue("player[10]", flags::is_read_only);
         extern VariableScopeWhichValue player_11  = VariableScopeWhichValue("player[11]", flags::is_read_only);
         extern VariableScopeWhichValue player_12  = VariableScopeWhichValue("player[12]", flags::is_read_only);
         extern VariableScopeWhichValue player_13  = VariableScopeWhichValue("player[13]", flags::is_read_only);
         extern VariableScopeWhichValue player_14  = VariableScopeWhichValue("player[14]", flags::is_read_only);
         extern VariableScopeWhichValue player_15  = VariableScopeWhichValue("player[15]", flags::is_read_only);
         extern VariableScopeWhichValue global_0   = VariableScopeWhichValue("global.player[0]");
         extern VariableScopeWhichValue global_1   = VariableScopeWhichValue("global.player[1]");
         extern VariableScopeWhichValue global_2   = VariableScopeWhichValue("global.player[2]");
         extern VariableScopeWhichValue global_3   = VariableScopeWhichValue("global.player[3]");
         extern VariableScopeWhichValue global_4   = VariableScopeWhichValue("global.player[4]");
         extern VariableScopeWhichValue global_5   = VariableScopeWhichValue("global.player[5]");
         extern VariableScopeWhichValue global_6   = VariableScopeWhichValue("global.player[6]");
         extern VariableScopeWhichValue global_7   = VariableScopeWhichValue("global.player[7]");
         extern VariableScopeWhichValue current    = VariableScopeWhichValue("current_player", flags::is_read_only);
         extern VariableScopeWhichValue hud        = VariableScopeWhichValue("hud_player", flags::is_read_only);
         extern VariableScopeWhichValue hud_target = VariableScopeWhichValue("hud_target_player", flags::is_read_only);
         extern VariableScopeWhichValue killer     = VariableScopeWhichValue("killer_player", flags::is_read_only);
      }
      namespace team {
         extern const VariableScopeWhichValueList list = VariableScopeWhichValueList({
            &no_team,
            &team_0,
            &team_1,
            &team_2,
            &team_3,
            &team_4,
            &team_5,
            &team_6,
            &team_7,
            &neutral_team,
            &global_0,
            &global_1,
            &global_2,
            &global_3,
            &global_4,
            &global_5,
            &global_6,
            &global_7,
            &current,
            &hud_player_owner_team,
            &hud_target_player_owner_team,
            &unk_14,
            &unk_15,
         });
         //
         extern VariableScopeWhichValue no_team  = VariableScopeWhichValue("no_team", flags::is_none);
         extern VariableScopeWhichValue team_0   = VariableScopeWhichValue("team[0]", flags::is_read_only);
         extern VariableScopeWhichValue team_1   = VariableScopeWhichValue("team[1]", flags::is_read_only);
         extern VariableScopeWhichValue team_2   = VariableScopeWhichValue("team[2]", flags::is_read_only);
         extern VariableScopeWhichValue team_3   = VariableScopeWhichValue("team[3]", flags::is_read_only);
         extern VariableScopeWhichValue team_4   = VariableScopeWhichValue("team[4]", flags::is_read_only);
         extern VariableScopeWhichValue team_5   = VariableScopeWhichValue("team[5]", flags::is_read_only);
         extern VariableScopeWhichValue team_6   = VariableScopeWhichValue("team[6]", flags::is_read_only);
         extern VariableScopeWhichValue team_7   = VariableScopeWhichValue("team[7]", flags::is_read_only);
         extern VariableScopeWhichValue neutral_team = VariableScopeWhichValue("neutral_team", flags::is_read_only);
         extern VariableScopeWhichValue global_0 = VariableScopeWhichValue("global.team[0]");
         extern VariableScopeWhichValue global_1 = VariableScopeWhichValue("global.team[1]");
         extern VariableScopeWhichValue global_2 = VariableScopeWhichValue("global.team[2]");
         extern VariableScopeWhichValue global_3 = VariableScopeWhichValue("global.team[3]");
         extern VariableScopeWhichValue global_4 = VariableScopeWhichValue("global.team[4]");
         extern VariableScopeWhichValue global_5 = VariableScopeWhichValue("global.team[5]");
         extern VariableScopeWhichValue global_6 = VariableScopeWhichValue("global.team[6]");
         extern VariableScopeWhichValue global_7 = VariableScopeWhichValue("global.team[7]");
         extern VariableScopeWhichValue current  = VariableScopeWhichValue("current_team", flags::is_read_only);
         extern VariableScopeWhichValue hud_player_owner_team        = VariableScopeWhichValue("hud_player.team", flags::is_read_only);
         extern VariableScopeWhichValue hud_target_player_owner_team = VariableScopeWhichValue("hud_target_player.team", flags::is_read_only);
         extern VariableScopeWhichValue unk_14   = VariableScopeWhichValue("unk_14_team");
         extern VariableScopeWhichValue unk_15   = VariableScopeWhichValue("unk_15_team");
      }
   }
   extern const VariableScopeWhichValueList megalo_scope_does_not_have_specifier = VariableScopeWhichValueList({});

   extern const VariableScope MegaloVariableScopeGlobal = VariableScope(megalo_scope_does_not_have_specifier, 12, 8, 8, 8, 16);
   extern const VariableScope MegaloVariableScopePlayer = VariableScope(variable_which_values::player::list, 8, 4, 4, 4, 4);
   extern const VariableScope MegaloVariableScopeObject = VariableScope(variable_which_values::object::list, 8, 4, 2, 4, 4);
   extern const VariableScope MegaloVariableScopeTeam   = VariableScope(variable_which_values::team::list,   8, 4, 4, 4, 6);

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