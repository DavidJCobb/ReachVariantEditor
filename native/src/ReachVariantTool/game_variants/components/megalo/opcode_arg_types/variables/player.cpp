#include "player.h"
#include "number.h"
#include "object.h"
#include "team.h"

namespace {
   using namespace Megalo;
   using id = OpcodeArgValuePlayer::scope_indicator_id;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(variable_type::player, {
      VariableScopeIndicatorValue(id::g_p, "%w",            "%w",              &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none),   // global.player, current_player, etc.
      VariableScopeIndicatorValue(id::p_p, "%w.player[%i]", "%w's player[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::player), // player.player
      VariableScopeIndicatorValue(id::o_p, "%w.player[%i]", "%w's player[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::player), // object.player
      VariableScopeIndicatorValue(id::t_p, "%w.player[%i]", "%w's player[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::player), // team.player
   });
}
namespace Megalo {
   OpcodeArgValuePlayer::OpcodeArgValuePlayer() : Variable(scopes) {}
   OpcodeArgTypeinfo OpcodeArgValuePlayer::typeinfo = OpcodeArgTypeinfo(
      "player",
      "Player",
      "A player participating in the match. Note that players are a distinct concept from \"bipeds,\" the character objects that players control.",
      //
      OpcodeArgTypeinfo::typeinfo_type::default,
      OpcodeArgTypeinfo::flags::is_variable | OpcodeArgTypeinfo::flags::can_hold_variables | OpcodeArgTypeinfo::flags::can_be_static,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValuePlayer>,
      {
         Script::Property("biped",       OpcodeArgValueObject::typeinfo, -1, true),
         Script::Property("rating",      OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::p_rating),
         Script::Property("score",       OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::p_score),
         Script::Property("script_stat", OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::p_stat),
         Script::Property("team",        OpcodeArgValueTeam::typeinfo,   OpcodeArgValueTeam::scope_indicator_id::p_owner_team),
         Script::Property("unknown_09",  OpcodeArgValueScalar::typeinfo, OpcodeArgValueScalar::scope_indicator_id::p_unk09),
      },
      16 // static count
   );
}