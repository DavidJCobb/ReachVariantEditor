#include "timer.h"
#include "number.h"

namespace {
   using namespace Megalo;
   using namespace Megalo::variable_scope_indicators::timer;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(OpcodeArgValueTimer::typeinfo, Megalo::variable_type::timer, {
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
         extern VariableScopeIndicatorValue global_timer       = VariableScopeIndicatorValue::make_variable_scope("%w.timer[%i]", "%w.timer[%i]",   &MegaloVariableScopeGlobal, VariableScopeIndicatorValue::index_type::timer);
         extern VariableScopeIndicatorValue player_timer       = VariableScopeIndicatorValue::make_variable_scope("%w.timer[%i]", "%w's timer[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::timer);
         extern VariableScopeIndicatorValue team_timer         = VariableScopeIndicatorValue::make_variable_scope("%w.timer[%i]", "%w's timer[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::timer);
         extern VariableScopeIndicatorValue object_timer       = VariableScopeIndicatorValue::make_variable_scope("%w.timer[%i]", "%w's timer[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::timer);
         extern VariableScopeIndicatorValue round_timer        = VariableScopeIndicatorValue::make_game_value("game.round_timer",        "Round Timer"); // NOT read-only; see Invasion
         extern VariableScopeIndicatorValue sudden_death_timer = VariableScopeIndicatorValue::make_game_value("game.sudden_death_timer", "Sudden Death Timer");
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
   //
   Variable* OpcodeArgValueTimer::create_zero_or_none() const noexcept {
      auto arg = new OpcodeArgValueScalar;
      arg->scope = &variable_scope_indicators::number::constant;
      arg->which = 0;
      arg->index = 0;
      return arg;
   }
   bool OpcodeArgValueTimer::set_to_zero_or_none() noexcept {
      return false;
   }
}