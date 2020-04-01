#include "object.h"
#include "number.h"
#include "team.h"

namespace {
   using namespace Megalo;
   using id = OpcodeArgValueObject::scope_indicator_id;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(Megalo::variable_type::object, {
      VariableScopeIndicatorValue(id::g_o,       "%w",            "%w",              &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::none),   // global.object, current_object, etc.
      VariableScopeIndicatorValue(id::p_o,       "%w.object[%i]", "%w's object[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::object), // player.object
      VariableScopeIndicatorValue(id::o_o,       "%w.object[%i]", "%w's object[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::object), // object.object
      VariableScopeIndicatorValue(id::t_o,       "%w.object[%i]", "%w's object[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::object), // team.object
      VariableScopeIndicatorValue(id::p_biped,   "%w.biped",            "%w's biped",              &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none),   // player.biped
      VariableScopeIndicatorValue(id::p_p_biped, "%w.player[%i].biped", "%w's player[%i]'s biped", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::player), // player.player.biped
      VariableScopeIndicatorValue(id::o_p_biped, "%w.player[%i].biped", "%w's player[%i]'s biped", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::player), // object.player.biped
      VariableScopeIndicatorValue(id::t_p_biped, "%w.player[%i].biped", "%w's player[%i]'s biped", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::player), // team.player.biped
   });
}
namespace Megalo {
   OpcodeArgValueObject::OpcodeArgValueObject() : Variable(scopes) {}
   OpcodeArgTypeinfo OpcodeArgValueObject::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      OpcodeArgTypeinfo::flags::is_variable | OpcodeArgTypeinfo::flags::can_hold_variables,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueObject>,
      {
         Script::Property("spawn_sequence", OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::spawn_sequence),
         Script::Property("team",           OpcodeArgValueTeam::typeinfo,   OpcodeArgValueTeam::scope_indicator_id::o_owner_team),
      }
   );
}