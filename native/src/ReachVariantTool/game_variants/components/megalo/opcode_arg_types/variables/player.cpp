#include "player.h"

namespace {
   using namespace Megalo;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(variable_type::player, {
      VariableScopeIndicatorValue("%w",            "%w",              &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none),   // global.player, current_player, etc.
      VariableScopeIndicatorValue("%w.player[%i]", "%w's player[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::player), // player.player
      VariableScopeIndicatorValue("%w.player[%i]", "%w's player[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::player), // object.player
      VariableScopeIndicatorValue("%w.player[%i]", "%w's player[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::player), // team.player
   });
}
namespace Megalo {
   OpcodeArgValuePlayer::OpcodeArgValuePlayer() : Variable(scopes) {}
   OpcodeArgTypeinfo OpcodeArgValuePlayer::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      OpcodeArgTypeinfo::flags::is_variable | OpcodeArgTypeinfo::flags::can_hold_variables | OpcodeArgTypeinfo::flags::can_be_static ,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValuePlayer>
   );
}