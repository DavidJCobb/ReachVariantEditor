#include "timer.h"

namespace {
   using namespace Megalo;
   using id = OpcodeArgValueTimer::scope_indicator_id;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(Megalo::variable_type::timer, {
      VariableScopeIndicatorValue(id::g_t, "%w.timer[%i]", "%w.timer[%i]",   &MegaloVariableScopeGlobal, VariableScopeIndicatorValue::index_type::timer), // global.timer
      VariableScopeIndicatorValue(id::p_t, "%w.timer[%i]", "%w's timer[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::timer), // player.timer
      VariableScopeIndicatorValue(id::t_t, "%w.timer[%i]", "%w's timer[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::timer), // team.timer
      VariableScopeIndicatorValue(id::o_t, "%w.timer[%i]", "%w's timer[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::timer), // object.timer
      VariableScopeIndicatorValue::make_game_value(id::round,  "game.round_timer",        "Round Timer",        VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::sudden, "game.sudden_death_timer", "Sudden Death Timer", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::grace,  "game.grace_period_timer", "Grace Period Timer", VariableScopeIndicatorValue::flags::is_readonly),
   });
}
namespace Megalo {
   OpcodeArgValueTimer::OpcodeArgValueTimer() : Variable(scopes) {}
   OpcodeArgTypeinfo OpcodeArgValueTimer::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      OpcodeArgTypeinfo::flags::is_variable,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueTimer>
   );
}