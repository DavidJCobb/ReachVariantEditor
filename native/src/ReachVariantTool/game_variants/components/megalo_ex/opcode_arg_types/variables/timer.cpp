#include "timer.h"
#include "base.h"

namespace {
   using namespace MegaloEx;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(Megalo::variable_type::timer, {
      VariableScopeIndicatorValue("%w.timer[%i]", "%w.timer[%i]",   &MegaloVariableScopeGlobal, VariableScopeIndicatorValue::index_type::timer), // global.timer
      VariableScopeIndicatorValue("%w.timer[%i]", "%w's timer[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::timer), // player.timer
      VariableScopeIndicatorValue("%w.timer[%i]", "%w's timer[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::timer), // team.timer
      VariableScopeIndicatorValue("%w.timer[%i]", "%w's timer[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::timer), // object.timer
      VariableScopeIndicatorValue::make_game_value("game.round_timer",        "Round Timer",        VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("game.sudden_death_timer", "Sudden Death Timer", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("game.grace_period_timer", "Grace Period Timer", VariableScopeIndicatorValue::flags::is_readonly),
   });
}
namespace MegaloEx {
   namespace types {
      OpcodeArgTypeinfo timer = OpcodeArgTypeinfo(
         QString("Timer"),
         QString(""),
         OpcodeArgTypeinfo::flags::is_variable, // if 343i patches in any timer-scope-indicators that need postprocessing, remember to add the flag
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