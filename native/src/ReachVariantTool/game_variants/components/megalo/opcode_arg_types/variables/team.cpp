#include "team.h"

namespace {
   using namespace Megalo;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(Megalo::variable_type::team, {
      VariableScopeIndicatorValue("%w",          "%w",            &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::none), // global.team[i], current_team, etc.
      VariableScopeIndicatorValue("%w.team[%i]", "%w's team[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::team), // player.team[i]
      VariableScopeIndicatorValue("%w.team[%i]", "%w's team[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::team), // object.team[i]
      VariableScopeIndicatorValue("%w.team[%i]", "%w's team[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::team), // team.team[i]
      VariableScopeIndicatorValue("%w.team",     "%w's team",     &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none), // player.team
      VariableScopeIndicatorValue("%w.team",     "%w's team",     &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::none), // object.team
   });
}
namespace Megalo {
   OpcodeArgValueTeam::OpcodeArgValueTeam() : Variable(scopes) {}
   OpcodeArgTypeinfo OpcodeArgValueTeam::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      OpcodeArgTypeinfo::flags::is_variable | OpcodeArgTypeinfo::flags::can_hold_variables,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueTeam>
   );
}