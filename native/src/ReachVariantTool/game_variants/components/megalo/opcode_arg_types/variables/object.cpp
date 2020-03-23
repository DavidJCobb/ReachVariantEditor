#include "object.h"

namespace {
   using namespace Megalo;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(Megalo::variable_type::object, {
      VariableScopeIndicatorValue("%w",            "%w",              &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::none),   // global.object, current_object, etc.
      VariableScopeIndicatorValue("%w.object[%i]", "%w's object[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::object), // player.object
      VariableScopeIndicatorValue("%w.object[%i]", "%w's object[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::object), // object.object
      VariableScopeIndicatorValue("%w.object[%i]", "%w's object[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::object), // team.object
      VariableScopeIndicatorValue("%w.biped",            "%w's biped",              &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none),   // player.biped
      VariableScopeIndicatorValue("%w.player[%i].biped", "%w's player[%i]'s biped", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::player), // player.player.biped
      VariableScopeIndicatorValue("%w.player[%i].biped", "%w's player[%i]'s biped", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::player), // object.player.biped
      VariableScopeIndicatorValue("%w.player[%i].biped", "%w's player[%i]'s biped", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::player), // team.player.biped
   });
}
namespace Megalo {
   OpcodeArgValueObject::OpcodeArgValueObject() : Variable(scopes) {}
   OpcodeArgTypeinfo OpcodeArgValueObject::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      OpcodeArgTypeinfo::flags::is_variable | OpcodeArgTypeinfo::flags::can_hold_variables,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueObject>
   );
}