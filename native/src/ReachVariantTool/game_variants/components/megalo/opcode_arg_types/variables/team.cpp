#include "team.h"
#include "number.h"

namespace {
   using namespace Megalo;
   using id = OpcodeArgValueTeam::scope_indicator_id;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(Megalo::variable_type::team, {
      VariableScopeIndicatorValue(id::g_t,          "%w",          "%w",            &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::none), // global.team[i], current_team, etc.
      VariableScopeIndicatorValue(id::p_t,          "%w.team[%i]", "%w's team[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::team), // player.team[i]
      VariableScopeIndicatorValue(id::o_t,          "%w.team[%i]", "%w's team[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::team), // object.team[i]
      VariableScopeIndicatorValue(id::t_t,          "%w.team[%i]", "%w's team[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::team), // team.team[i]
      VariableScopeIndicatorValue(id::p_owner_team, "%w.team",     "%w's team",     &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none), // player.team
      VariableScopeIndicatorValue(id::o_owner_team, "%w.team",     "%w's team",     &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::none), // object.team
   });
}
namespace Megalo {
   OpcodeArgValueTeam::OpcodeArgValueTeam() : Variable(scopes) {}
   OpcodeArgTypeinfo OpcodeArgValueTeam::typeinfo = OpcodeArgTypeinfo(
      "team",
      "Team",
      "A team.",
      //
      OpcodeArgTypeinfo::typeinfo_type::default,
      OpcodeArgTypeinfo::flags::is_variable | OpcodeArgTypeinfo::flags::can_hold_variables,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueTeam>,
      {
         Script::Property("score",       OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::t_score),
         Script::Property("script_stat", OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::t_stat),
      },
      'g_00', // "global.team[0]"
      's_00', // "team[0]"
      8 // static count
   );
}