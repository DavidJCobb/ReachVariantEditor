#include "object.h"
#include "number.h"
#include "team.h"

namespace {
   using namespace Megalo;
   using namespace Megalo::variable_scope_indicators::object;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(Megalo::variable_type::object, {
      &global_object,
      &player_object,
      &object_object,
      &team_object,
      &player_biped,
      &player_player_biped,
      &object_player_biped,
      &team_player_biped,
   });
}
namespace Megalo {
   namespace variable_scope_indicators {
      namespace object {
         extern VariableScopeIndicatorValueList& as_list() {
            return scopes;
         }
         //
         extern VariableScopeIndicatorValue global_object       = VariableScopeIndicatorValue("%w",                  "%w",                      &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::none);
         extern VariableScopeIndicatorValue player_object       = VariableScopeIndicatorValue("%w.object[%i]",       "%w's object[%i]",         &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::object);
         extern VariableScopeIndicatorValue object_object       = VariableScopeIndicatorValue("%w.object[%i]",       "%w's object[%i]",         &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::object);
         extern VariableScopeIndicatorValue team_object         = VariableScopeIndicatorValue("%w.object[%i]",       "%w's object[%i]",         &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::object);
         extern VariableScopeIndicatorValue player_biped        = VariableScopeIndicatorValue("%w.biped",            "%w's biped",              &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none,   VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue player_player_biped = VariableScopeIndicatorValue("%w.player[%i].biped", "%w's player[%i]'s biped", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::player, VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue object_player_biped = VariableScopeIndicatorValue("%w.player[%i].biped", "%w's player[%i]'s biped", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::player, VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue team_player_biped   = VariableScopeIndicatorValue("%w.player[%i].biped", "%w's player[%i]'s biped", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::player, VariableScopeIndicatorValue::flags::is_readonly);
      }
   }
   OpcodeArgValueObject::OpcodeArgValueObject() : Variable(scopes) {}
   OpcodeArgTypeinfo OpcodeArgValueObject::typeinfo = OpcodeArgTypeinfo(
      "object",
      "Object",
      "A physical object existing in the game world.",
      //
      OpcodeArgTypeinfo::flags::is_variable | OpcodeArgTypeinfo::flags::can_hold_variables,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueObject>,
      {
         Script::Property("spawn_sequence", OpcodeArgValueScalar::typeinfo, &Megalo::variable_scope_indicators::number::spawn_sequence),
         Script::Property("team",           OpcodeArgValueTeam::typeinfo,   &Megalo::variable_scope_indicators::team::object_owner_team),
      },
      'g_00' // global.object[0]
   );
}