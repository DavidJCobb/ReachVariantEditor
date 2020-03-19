#include "object.h"
#include "base.h"
#include "../../../../types/multiplayer.h" // game variant class

namespace {
   using namespace MegaloEx;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(Megalo::variable_type::object, {
      VariableScopeIndicatorValue("%w",            "%w",              &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::none),   // global.object, current_object, etc.
      VariableScopeIndicatorValue("%w.object[%i]", "%w's object[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::object), // player.object
      VariableScopeIndicatorValue("%w.object[%i]", "%w's object[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::object), // object.object
      VariableScopeIndicatorValue("%w.object[%i]", "%w's object[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::object), // team.object
      VariableScopeIndicatorValue("%w.biped",            "%w's biped",              &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none),   // player.biped
      VariableScopeIndicatorValue("%w.player[%i].biped", "%w's player[%i]'s biped", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::player), // player.player.biped
      VariableScopeIndicatorValue("%w.player[%i].biped", "%w's player[%i]'s biped", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::player), // object.player.biped
      VariableScopeIndicatorValue("%w.player[%i].biped", "%w's player[%i]'s biped", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::player), // team.player.biped
   });
}
namespace MegaloEx {
   namespace types {
      OpcodeArgTypeinfo object = OpcodeArgTypeinfo(
         QString("Object"),
         QString("A variable referring to a physical object in the game world."),
         OpcodeArgTypeinfo::flags::is_variable | OpcodeArgTypeinfo::flags::is_nestable_variable | OpcodeArgTypeinfo::flags::may_need_postprocessing,
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