#include "player.h"
#include "base.h"
#include "../../../../types/multiplayer.h" // game variant class

namespace {
   using namespace MegaloEx;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(Megalo::variable_type::player, {
      VariableScopeIndicatorValue("%w",            "%w",              &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none),   // global.player, current_player, etc.
      VariableScopeIndicatorValue("%w.player[%i]", "%w's player[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::player), // player.player
      VariableScopeIndicatorValue("%w.player[%i]", "%w's player[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::player), // object.player
      VariableScopeIndicatorValue("%w.player[%i]", "%w's player[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::player), // team.player
   });
}
namespace MegaloEx {
   namespace types {
      OpcodeArgTypeinfo player = OpcodeArgTypeinfo(
         QString("Player"),
         QString("A variable referring to a player. Note that the player model is an object distinct from the player itself."),
         OpcodeArgTypeinfo::flags::is_variable | OpcodeArgTypeinfo::flags::is_nestable_variable | OpcodeArgTypeinfo::flags::is_static_variable | OpcodeArgTypeinfo::flags::may_need_postprocessing,
         //
         [](arg_functor_state fs, OpcodeArgValue& arg, cobb::uint128_t input_bits) { // loader
            return scopes.load(fs, arg, input_bits);
         },
         [](arg_functor_state fs, OpcodeArgValue& arg, GameVariantData* variant) { // postprocess
            return scopes.postprocess(fs, arg, variant);
         },
         [](arg_functor_state fs, OpcodeArgValue& arg, std::string& out) { // to english
            return scopes.to_english(fs, arg, out);
         },
         [](arg_functor_state fs, OpcodeArgValue& arg, std::string& out) { // to script code
            return scopes.decompile(fs, arg, out);
         },
         nullptr // TODO: "compile" functor
      );
   }
}