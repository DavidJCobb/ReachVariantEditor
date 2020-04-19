#include "team.h"
#include "number.h"

namespace {
   using namespace Megalo;
   using namespace Megalo::variable_scope_indicators::team;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(Megalo::variable_type::team, {
      &global_team,
      &player_team,
      &object_team,
      &team_team,
      &player_owner_team,
      &object_owner_team,
   });
}
namespace Megalo {
   namespace variable_scope_indicators {
      namespace team {
         extern VariableScopeIndicatorValueList& as_list() {
            return scopes;
         }
         //
         extern VariableScopeIndicatorValue global_team       = VariableScopeIndicatorValue("%w",          "%w",            &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::none);
         extern VariableScopeIndicatorValue player_team       = VariableScopeIndicatorValue("%w.team[%i]", "%w's team[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::team);
         extern VariableScopeIndicatorValue object_team       = VariableScopeIndicatorValue("%w.team[%i]", "%w's team[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::team);
         extern VariableScopeIndicatorValue team_team         = VariableScopeIndicatorValue("%w.team[%i]", "%w's team[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::team);
         extern VariableScopeIndicatorValue player_owner_team = VariableScopeIndicatorValue("%w.team",     "%w's team",     &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none); // NOT read-only
         extern VariableScopeIndicatorValue object_owner_team = VariableScopeIndicatorValue("%w.team",     "%w's team",     &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::none); // NOT read-only
      }
   }
   OpcodeArgValueTeam::OpcodeArgValueTeam() : Variable(scopes) {}
   OpcodeArgTypeinfo OpcodeArgValueTeam::typeinfo = OpcodeArgTypeinfo(
      "team",
      "Team",
      "A team.",
      //
      OpcodeArgTypeinfo::flags::is_variable | OpcodeArgTypeinfo::flags::can_hold_variables,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueTeam>,
      {
         Script::Property("score",       OpcodeArgValueScalar::typeinfo, &Megalo::variable_scope_indicators::number::team_score),
         Script::Property("script_stat", OpcodeArgValueScalar::typeinfo, &Megalo::variable_scope_indicators::number::team_stat),
      },
      'g_00', // "global.team[0]"
      's_00', // "team[0]"
      8 // static count
   );
   //
   Variable* OpcodeArgValueTeam::create_zero_or_none() const noexcept {
      auto arg = new OpcodeArgValueTeam;
      arg->scope = &variable_scope_indicators::team::global_team;
      arg->which = variable_which_values::team::no_team.as_integer();
      arg->index = 0;
      return arg;
   }
}