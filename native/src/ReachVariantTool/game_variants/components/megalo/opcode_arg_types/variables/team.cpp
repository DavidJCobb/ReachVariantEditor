#include "team.h"
#include "number.h"

namespace {
   using namespace Megalo;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(Megalo::variable_type::team, {
      VariableScopeIndicatorValue("g-t",          "%w",          "%w",            &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::none), // global.team[i], current_team, etc.
      VariableScopeIndicatorValue("p-t",          "%w.team[%i]", "%w's team[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::team), // player.team[i]
      VariableScopeIndicatorValue("o-t",          "%w.team[%i]", "%w's team[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::team), // object.team[i]
      VariableScopeIndicatorValue("t-t",          "%w.team[%i]", "%w's team[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::team), // team.team[i]
      VariableScopeIndicatorValue("p-owner-team", "%w.team",     "%w's team",     &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none), // player.team
      VariableScopeIndicatorValue("o-owner-team", "%w.team",     "%w's team",     &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::none), // object.team
   });
}
namespace Megalo {
   OpcodeArgValueTeam::OpcodeArgValueTeam() : Variable(scopes) {}
   OpcodeArgTypeinfo OpcodeArgValueTeam::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      OpcodeArgTypeinfo::flags::is_variable | OpcodeArgTypeinfo::flags::can_hold_variables | OpcodeArgTypeinfo::flags::can_be_static,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueTeam>,
      {
         Script::Property("score",       OpcodeArgValueScalar::typeinfo, "t-score"),
         Script::Property("script_stat", OpcodeArgValueScalar::typeinfo, "t-stat"),
      }
   );
}