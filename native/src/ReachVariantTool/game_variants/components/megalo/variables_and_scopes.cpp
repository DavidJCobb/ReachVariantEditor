#include "variables_and_scopes.h"

namespace Megalo {

   const DetailedEnum megalo_scope_does_not_have_specifier = DetailedEnum({});
   const DetailedEnum megalo_objects = DetailedEnum({
      DetailedEnumValue("no_object",         DetailedEnumValueInfo::make_signature('none')),
      DetailedEnumValue("global.object[0]",  DetailedEnumValueInfo::make_signature('g_00')),
      DetailedEnumValue("global.object[1]",  DetailedEnumValueInfo::make_signature('g_01')),
      DetailedEnumValue("global.object[2]",  DetailedEnumValueInfo::make_signature('g_02')),
      DetailedEnumValue("global.object[3]",  DetailedEnumValueInfo::make_signature('g_03')),
      DetailedEnumValue("global.object[4]",  DetailedEnumValueInfo::make_signature('g_04')),
      DetailedEnumValue("global.object[5]",  DetailedEnumValueInfo::make_signature('g_05')),
      DetailedEnumValue("global.object[6]",  DetailedEnumValueInfo::make_signature('g_06')),
      DetailedEnumValue("global.object[7]",  DetailedEnumValueInfo::make_signature('g_07')),
      DetailedEnumValue("global.object[8]",  DetailedEnumValueInfo::make_signature('g_08')),
      DetailedEnumValue("global.object[9]",  DetailedEnumValueInfo::make_signature('g_09')),
      DetailedEnumValue("global.object[10]", DetailedEnumValueInfo::make_signature('g_10')),
      DetailedEnumValue("global.object[11]", DetailedEnumValueInfo::make_signature('g_11')),
      DetailedEnumValue("global.object[12]", DetailedEnumValueInfo::make_signature('g_12')),
      DetailedEnumValue("global.object[13]", DetailedEnumValueInfo::make_signature('g_13')),
      DetailedEnumValue("global.object[14]", DetailedEnumValueInfo::make_signature('g_14')),
      DetailedEnumValue("global.object[15]", DetailedEnumValueInfo::make_signature('g_15')),
      DetailedEnumValue("current_object",    DetailedEnumValueInfo::make_signature('loop')),
      DetailedEnumValue("hud_target_object", DetailedEnumValueInfo::make_signature('hudt')),
      DetailedEnumValue("killed_object",     DetailedEnumValueInfo::make_signature('kild')),
      DetailedEnumValue("killer_object",     DetailedEnumValueInfo::make_signature('kilr')),
   });
   const DetailedEnum megalo_players = DetailedEnum({
      DetailedEnumValue("no_player",         DetailedEnumValueInfo::make_signature('none')),
      DetailedEnumValue("player[0]",         DetailedEnumValueInfo::make_signature('s_00')),
      DetailedEnumValue("player[1]",         DetailedEnumValueInfo::make_signature('s_01')),
      DetailedEnumValue("player[2]",         DetailedEnumValueInfo::make_signature('s_02')),
      DetailedEnumValue("player[3]",         DetailedEnumValueInfo::make_signature('s_03')),
      DetailedEnumValue("player[4]",         DetailedEnumValueInfo::make_signature('s_04')),
      DetailedEnumValue("player[5]",         DetailedEnumValueInfo::make_signature('s_05')),
      DetailedEnumValue("player[6]",         DetailedEnumValueInfo::make_signature('s_06')),
      DetailedEnumValue("player[7]",         DetailedEnumValueInfo::make_signature('s_07')),
      DetailedEnumValue("player[8]",         DetailedEnumValueInfo::make_signature('s_08')),
      DetailedEnumValue("player[9]",         DetailedEnumValueInfo::make_signature('s_09')),
      DetailedEnumValue("player[10]",        DetailedEnumValueInfo::make_signature('s_10')),
      DetailedEnumValue("player[11]",        DetailedEnumValueInfo::make_signature('s_11')),
      DetailedEnumValue("player[12]",        DetailedEnumValueInfo::make_signature('s_12')),
      DetailedEnumValue("player[13]",        DetailedEnumValueInfo::make_signature('s_13')),
      DetailedEnumValue("player[14]",        DetailedEnumValueInfo::make_signature('s_14')),
      DetailedEnumValue("player[15]",        DetailedEnumValueInfo::make_signature('s_15')),
      DetailedEnumValue("global.player[0]",  DetailedEnumValueInfo::make_signature('g_00')),
      DetailedEnumValue("global.player[1]",  DetailedEnumValueInfo::make_signature('g_01')),
      DetailedEnumValue("global.player[2]",  DetailedEnumValueInfo::make_signature('g_02')),
      DetailedEnumValue("global.player[3]",  DetailedEnumValueInfo::make_signature('g_03')),
      DetailedEnumValue("global.player[4]",  DetailedEnumValueInfo::make_signature('g_04')),
      DetailedEnumValue("global.player[5]",  DetailedEnumValueInfo::make_signature('g_05')),
      DetailedEnumValue("global.player[6]",  DetailedEnumValueInfo::make_signature('g_06')),
      DetailedEnumValue("global.player[7]",  DetailedEnumValueInfo::make_signature('g_07')),
      DetailedEnumValue("current_player",    DetailedEnumValueInfo::make_signature('loop')),
      DetailedEnumValue("hud_player",        DetailedEnumValueInfo::make_signature('hudp')),
      DetailedEnumValue("hud_target_player", DetailedEnumValueInfo::make_signature('hudt')),
      DetailedEnumValue("killer_player",     DetailedEnumValueInfo::make_signature('kilr')),
   });
   const DetailedEnum megalo_teams = DetailedEnum({ // Apparently, in Reach's memory, this type's "none" value is -1... but it's still serialized as 0. We could honor that but why? Just handle it as 0 lol
      DetailedEnumValue("no_team",           DetailedEnumValueInfo::make_signature('none')),
      DetailedEnumValue("team[0]",           DetailedEnumValueInfo::make_signature('s_00')),
      DetailedEnumValue("team[1]",           DetailedEnumValueInfo::make_signature('s_01')),
      DetailedEnumValue("team[2]",           DetailedEnumValueInfo::make_signature('s_02')),
      DetailedEnumValue("team[3]",           DetailedEnumValueInfo::make_signature('s_03')),
      DetailedEnumValue("team[4]",           DetailedEnumValueInfo::make_signature('s_04')),
      DetailedEnumValue("team[5]",           DetailedEnumValueInfo::make_signature('s_05')),
      DetailedEnumValue("team[6]",           DetailedEnumValueInfo::make_signature('s_06')),
      DetailedEnumValue("team[7]",           DetailedEnumValueInfo::make_signature('s_07')),
      DetailedEnumValue("neutral_team",      DetailedEnumValueInfo::make_signature('neut')),
      DetailedEnumValue("global.team[0]",    DetailedEnumValueInfo::make_signature('g_00')),
      DetailedEnumValue("global.team[1]",    DetailedEnumValueInfo::make_signature('g_01')),
      DetailedEnumValue("global.team[2]",    DetailedEnumValueInfo::make_signature('g_02')),
      DetailedEnumValue("global.team[3]",    DetailedEnumValueInfo::make_signature('g_03')),
      DetailedEnumValue("global.team[4]",    DetailedEnumValueInfo::make_signature('g_04')),
      DetailedEnumValue("global.team[5]",    DetailedEnumValueInfo::make_signature('g_05')),
      DetailedEnumValue("global.team[6]",    DetailedEnumValueInfo::make_signature('g_06')),
      DetailedEnumValue("global.team[7]",    DetailedEnumValueInfo::make_signature('g_07')),
      DetailedEnumValue("current_team",      DetailedEnumValueInfo::make_signature('loop')),
      DetailedEnumValue("hud_player.team",   DetailedEnumValueInfo::make_signature('hudp')),
      DetailedEnumValue("hud_target_player.team", DetailedEnumValueInfo::make_signature('hudt')),
      DetailedEnumValue("unk_14_team",       DetailedEnumValueInfo::make_signature('u_14')),
      DetailedEnumValue("unk_15_team",       DetailedEnumValueInfo::make_signature('u_15')),
   });

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