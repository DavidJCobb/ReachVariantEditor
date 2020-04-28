#include "player.h"
#include "number.h"
#include "object.h"
#include "team.h"

namespace {
   using namespace Megalo;
   using namespace Megalo::variable_scope_indicators::player;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(OpcodeArgValuePlayer::typeinfo, Megalo::variable_type::player, {
      &global_player,
      &player_player,
      &object_player,
      &team_player,
   });
}
namespace Megalo {
   namespace variable_scope_indicators {
      namespace player {
         extern VariableScopeIndicatorValueList& as_list() {
            return scopes;
         }
         //
         extern VariableScopeIndicatorValue global_player = VariableScopeIndicatorValue::make_variable_scope("%w",            "%w",              &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none);
         extern VariableScopeIndicatorValue player_player = VariableScopeIndicatorValue::make_variable_scope("%w.player[%i]", "%w's player[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::player);
         extern VariableScopeIndicatorValue object_player = VariableScopeIndicatorValue::make_variable_scope("%w.player[%i]", "%w's player[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::player);
         extern VariableScopeIndicatorValue team_player   = VariableScopeIndicatorValue::make_variable_scope("%w.player[%i]", "%w's player[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::player);
      }
   }
   OpcodeArgValuePlayer::OpcodeArgValuePlayer() : Variable(scopes) {}
   OpcodeArgTypeinfo OpcodeArgValuePlayer::typeinfo = OpcodeArgTypeinfo(
      "player",
      "Player",
      "A player participating in the match. Note that players are a distinct concept from \"bipeds,\" the character objects that players control.",
      //
      OpcodeArgTypeinfo::flags::is_variable | OpcodeArgTypeinfo::flags::can_hold_variables,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValuePlayer>,
      {
         Script::Property("biped",       OpcodeArgValueObject::typeinfo, Script::Property::no_scope, true),
         Script::Property("rating",      OpcodeArgValueScalar::typeinfo, &Megalo::variable_scope_indicators::number::player_rating),
         Script::Property("score",       OpcodeArgValueScalar::typeinfo, &Megalo::variable_scope_indicators::number::player_score),
         Script::Property("script_stat", OpcodeArgValueScalar::typeinfo, &Megalo::variable_scope_indicators::number::player_stat),
         Script::Property("team",        OpcodeArgValueTeam::typeinfo,   &Megalo::variable_scope_indicators::team::player_owner_team),
         Script::Property("unknown_09",  OpcodeArgValueScalar::typeinfo, &Megalo::variable_scope_indicators::number::player_unk09),
      },
      16 // static count
   ).set_variable_which_values(
      &variable_which_values::player::global_0,
      &variable_which_values::player::player_0
   );
   //
   Variable* OpcodeArgValuePlayer::create_zero_or_none() const noexcept {
      auto arg = new OpcodeArgValuePlayer;
      arg->scope = &variable_scope_indicators::player::global_player;
      arg->which = variable_which_values::player::no_player.as_integer();
      arg->index = 0;
      return arg;
   }
}