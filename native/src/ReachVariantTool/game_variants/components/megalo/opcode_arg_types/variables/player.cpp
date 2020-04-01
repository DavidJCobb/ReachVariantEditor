#include "player.h"
#include "number.h"
#include "object.h"
#include "team.h"

namespace {
   using namespace Megalo;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(variable_type::player, {
      VariableScopeIndicatorValue("g-p", "%w",            "%w",              &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none),   // global.player, current_player, etc.
      VariableScopeIndicatorValue("p-p", "%w.player[%i]", "%w's player[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::player), // player.player
      VariableScopeIndicatorValue("o-p", "%w.player[%i]", "%w's player[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::player), // object.player
      VariableScopeIndicatorValue("t-p", "%w.player[%i]", "%w's player[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::player), // team.player
   });
}
namespace Megalo {
   OpcodeArgValuePlayer::OpcodeArgValuePlayer() : Variable(scopes) {}
   OpcodeArgTypeinfo OpcodeArgValuePlayer::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      OpcodeArgTypeinfo::flags::is_variable | OpcodeArgTypeinfo::flags::can_hold_variables | OpcodeArgTypeinfo::flags::can_be_static ,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValuePlayer>,
      {
         Script::Property("biped",       OpcodeArgValueObject::typeinfo, nullptr, true),
         Script::Property("rating",      OpcodeArgValueScalar::typeinfo, "p-rating"),
         Script::Property("score",       OpcodeArgValueScalar::typeinfo, "p-score"),
         Script::Property("script_stat", OpcodeArgValueScalar::typeinfo, "p-stat"),
         Script::Property("team",        OpcodeArgValueTeam::typeinfo,   "p-owner-team"),
         Script::Property("unknown_09",  OpcodeArgValueScalar::typeinfo, "p-unk09"),
      }
   );
}