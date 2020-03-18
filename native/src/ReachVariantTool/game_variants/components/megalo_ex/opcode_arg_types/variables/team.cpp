#include "team.h"
#include "base.h"
#include "../../../../types/multiplayer.h" // game variant class

namespace {
   using namespace MegaloEx;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(Megalo::variable_type::team, {
      VariableScopeIndicatorValue("%w",          "%w",            &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::none), // global.team[i], current_team, etc.
      VariableScopeIndicatorValue("%w.team[%i]", "%w's team[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::team), // player.team[i]
      VariableScopeIndicatorValue("%w.team[%i]", "%w's team[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::team), // object.team[i]
      VariableScopeIndicatorValue("%w.team[%i]", "%w's team[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::team), // team.team[i]
      VariableScopeIndicatorValue("%w.team",     "%w's team",     &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none), // player.team
      VariableScopeIndicatorValue("%w.team",     "%w's team",     &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::none), // object.team
   });
}
namespace MegaloEx {
   namespace types {
      OpcodeArgTypeinfo object = OpcodeArgTypeinfo(
         QString("Team"),
         QString("A variable referring to a team."),
         OpcodeArgTypeinfo::flags::is_variable | OpcodeArgTypeinfo::flags::is_nestable_variable | OpcodeArgTypeinfo::flags::is_static_variable | OpcodeArgTypeinfo::flags::may_need_postprocessing,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            return scopes.load(fs, data, relObjs, input_bits);
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, GameVariantData* variant) { // postprocess
            return scopes.postprocess(fs, data, relObjs, variant);
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            return scopes.to_english(fs, data, relObjs, out);
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            return scopes.decompile(fs, data, relObjs, out);
         },
         nullptr // TODO: "compile" functor
      );
   }
}