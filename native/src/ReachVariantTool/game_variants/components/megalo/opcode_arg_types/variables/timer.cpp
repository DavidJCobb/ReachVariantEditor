#include "timer.h"

namespace {
   using namespace Megalo;
   using namespace Megalo::variable_scope_indicators::timer;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(Megalo::variable_type::timer, {
      &global_timer,
      &player_timer,
      &team_timer,
      &object_timer,
      &round_timer,
      &sudden_death_timer,
      &grace_period_timer,
   });
}
namespace Megalo {
   namespace variable_scope_indicators {
      namespace timer {
         extern VariableScopeIndicatorValueList& as_list() {
            return scopes;
         }
         //
         extern VariableScopeIndicatorValue global_timer       = VariableScopeIndicatorValue("%w.timer[%i]", "%w.timer[%i]",   &MegaloVariableScopeGlobal, VariableScopeIndicatorValue::index_type::timer);
         extern VariableScopeIndicatorValue player_timer       = VariableScopeIndicatorValue("%w.timer[%i]", "%w's timer[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::timer);
         extern VariableScopeIndicatorValue team_timer         = VariableScopeIndicatorValue("%w.timer[%i]", "%w's timer[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::timer);
         extern VariableScopeIndicatorValue object_timer       = VariableScopeIndicatorValue("%w.timer[%i]", "%w's timer[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::timer);
         extern VariableScopeIndicatorValue round_timer        = VariableScopeIndicatorValue::make_game_value("game.round_timer",        "Round Timer"); // NOT read-only; see Invasion
         extern VariableScopeIndicatorValue sudden_death_timer = VariableScopeIndicatorValue::make_game_value("game.sudden_death_timer", "Sudden Death Timer", VariableScopeIndicatorValue::flags::is_readonly); // TODO: test to determine whether this is actually read-only
         extern VariableScopeIndicatorValue grace_period_timer = VariableScopeIndicatorValue::make_game_value("game.grace_period_timer", "Grace Period Timer"); // NOT read-only; see Invasion
      }
   }
   OpcodeArgValueTimer::OpcodeArgValueTimer() : Variable(scopes) {}
   OpcodeArgTypeinfo OpcodeArgValueTimer::typeinfo = OpcodeArgTypeinfo(
      "timer",
      "Timer",
      "A numeric timer. You can control whether timers increase or decrease, and at what rate.",
      //
      OpcodeArgTypeinfo::flags::is_variable,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueTimer>
   );
}