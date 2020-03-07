#include "timer.h"
#include "../../../megalo/variables_and_scopes.h"
#include "../../../../helpers/strings.h"
#include "../../../../errors.h"

namespace MegaloEx {
   using namespace Megalo; // hack until we swap everytihng over
}

namespace {
   enum class _scopes {
      global,
      player,
      team,
      object,
      round_time,
      sudden_death_time,
      grace_period_time,
      //
      _count,
   };
   constexpr Megalo::variable_type ce_my_variable_type = Megalo::variable_type::timer;
}

namespace MegaloEx {
   namespace DRAFT {
      //
      // I wanna take a generic approach to the type/scope/which/index system in use for all of the 
      // variable types. To that end:
      //
      //  - Each type has its own list of "scopes." In practice, these will map to all of the 
      //    VariableScope objects (which might be more appropriately named "scope declarations"), as 
      //    well as specific scopes unique to the type (such as player.team for team variables) and 
      //    game-state values (such as the round time or the round timer).
      //
      //  - A scope may or may not have a "which" value, indicating which of the scopes in question 
      //    is being accessed, and an "index" value, indicating which variable in that scope is 
      //    being accessed.
      //
      //     - global.player[w].timer[i] has a "which" value indicating which player is being 
      //       accessed, and an "index" value indicating which timer on that player is being 
      //       accessed. It's worth noting that this example covers more than just global player 
      //       variables; current_player.timer[i] uses the "which" value to indicate current_player.
      //
      //     - global.timer[i] has an "index" value but no "which" value, because there is only one 
      //       global scope.
      //
      //     - game.round_timer has neither a "which" value nor an "index" value; there is only one 
      //       round timer, and the fact of our using its scope is enough.
      //
      //  - The "index" value can also be general-purpose. When a number variable is used to hold a 
      //    constant integer, we store its int16_t value in the "index."
      //
      //     - As such, uint8_t is appropriate for the scope and which, but int16_t must always be 
      //       used for the index.
      //
      //  - This means that the "which," when defined, can be any of the following enums: megalo_teams, 
      //    megalo_players, and megalo_objects. If the "which" is defined, then the "index" must 
      //    necessarily be constrained to VariableScope::index_bits for the "which" in question.
      //
      //    If the "which" is not defined, then "index" can be an index in any collection (e.g. script 
      //    options), or it can be a bare int16_t constant.
      //
      //     - The collections that "index" can index into are: script options; script stats. We'll 
      //       probably need to make the generic approach take lambdas to wrap access to these.
      //
      //     = The current release-build implementation of scalar variables uses "index," not "which," 
      //       to indicate the object/player/team for: player rating; player score; team score; and 
      //       object spawn sequence. This should be considered a design flaw; we should use the "which" 
      //       to indicate the object/player/team that the properties belong to.
      //
      class TypedVariableScope {
         private:
            struct _decoded {
               uint8_t scope;
               uint8_t which;
               int16_t index;
            };
         public:
            uint8_t        id       = 0; // the _scopes value
            VariableScope* base     = nullptr;
            const char*    format   = "%w.timer[%i]";
            bool           readonly = false;
            //
            // TODO: other needed fields; member functions to serve as helpers to the various typeinfo functors
            //
      };
   }
}

namespace MegaloEx {
   namespace types {
      OpcodeArgTypeinfo timer = OpcodeArgTypeinfo(
         QString("Timer"),
         QString(""),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            auto scope = (_scopes)data.consume(input_bits, cobb::bitcount((int)_scopes::_count - 1));
            const VariableScope* variable_scope = nullptr;
            switch (scope) {
               case _scopes::global:
                  variable_scope = &MegaloVariableScopeGlobal;
                  break;
               case _scopes::player:
                  variable_scope = &MegaloVariableScopePlayer;
                  break;
               case _scopes::object:
                  variable_scope = &MegaloVariableScopeObject;
                  break;
               case _scopes::team:
                  variable_scope = &MegaloVariableScopeTeam;
                  break;
               case _scopes::round_time:
               case _scopes::sudden_death_time:
               case _scopes::grace_period_time:
                  break;
               default:
                  {
                     auto& error = GameEngineVariantLoadError::get();
                     error.state    = GameEngineVariantLoadError::load_state::failure;
                     error.reason   = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
                     error.detail   = GameEngineVariantLoadError::load_failure_detail::bad_variable_subtype;
                     error.extra[0] = (int32_t)variable_type::timer;
                     error.extra[1] = (int32_t)scope;
                  }
                  return false;
            }
            if (variable_scope) {
               uint16_t which = (uint16_t)data.consume(input_bits, variable_scope->which_bits());
               uint16_t index = (uint16_t)data.consume(input_bits, variable_scope->index_bits(ce_my_variable_type));
               if (!variable_scope->is_valid_which(which)) {
                  auto& error = GameEngineVariantLoadError::get();
                  error.state = GameEngineVariantLoadError::load_state::failure;
                  error.reason = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
                  error.detail = GameEngineVariantLoadError::load_failure_detail::bad_variable_scope;
                  error.extra[0] = (int32_t)getScopeConstantForObject(*variable_scope);
                  error.extra[1] = which;
                  error.extra[2] = index;
                  error.extra[3] = (int32_t)variable_type::timer;
                  return false;
               }
            }
            return true;
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            auto scope  = (_scopes)data.excerpt(fs.bit_offset, cobb::bitcount((int)_scopes::_count - 1));
            auto offset = fs.bit_offset + cobb::bitcount((int)_scopes::_count - 1);
            const VariableScope* variable_scope = nullptr;
            switch (scope) {
               case _scopes::global:
                  variable_scope = &MegaloVariableScopeGlobal;
                  break;
               case _scopes::player:
                  variable_scope = &MegaloVariableScopePlayer;
                  break;
               case _scopes::object:
                  variable_scope = &MegaloVariableScopeObject;
                  break;
               case _scopes::team:
                  variable_scope = &MegaloVariableScopeTeam;
                  break;
               case _scopes::round_time:
                  out = "Round Timer";
                  return true;
               case _scopes::sudden_death_time:
                  out = "Sudden Death Timer";
                  return true;
               case _scopes::grace_period_time:
                  out = "Grace Period Timer";
                  return true;
               default:
                  out = "?????";
                  return false;
            }
            //
            auto     which_bits = variable_scope->which_bits();
            auto     index_bits = variable_scope->index_bits(ce_my_variable_type);
            uint16_t which = data.excerpt(offset, which_bits);
            offset += which_bits;
            uint16_t index = data.excerpt(offset, index_bits);
            //
            const char* which_scope;
            switch (scope) {
               case _scopes::global:
                  // which_bits is always zero for the global scope, and which is unused
                  cobb::sprintf(out, "Global.Timer[%d]", index);
                  break;
               case _scopes::player:
                  which_scope = megalo_players[which];
                  if (which_scope)
                     cobb::sprintf(out, "%s.Timer[%u]", which_scope, index);
                  else
                     cobb::sprintf(out, "INVALID_PLAYER[%u].Timer[%u]", which, index);
                  break;
               case _scopes::object:
                  which_scope = megalo_objects[which];
                  if (which_scope)
                     cobb::sprintf(out, "%s.Timer[%u]", which_scope, index);
                  else
                     cobb::sprintf(out, "INVALID_OBJECT[%u].Timer[%u]", which, index);
                  break;
               case _scopes::team:
                  which_scope = megalo_teams[which];
                  if (which_scope)
                     cobb::sprintf(out, "%s.Timer[%u]", which_scope, index);
                  else
                     cobb::sprintf(out, "INVALID_TEAM[%u].Timer[%u]", which, index);
                  break;
            }
            return true;
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            auto scope  = (_scopes)data.excerpt(fs.bit_offset, cobb::bitcount((int)_scopes::_count - 1));
            auto offset = fs.bit_offset + cobb::bitcount((int)_scopes::_count - 1);
            const VariableScope* variable_scope = nullptr;
            switch (scope) {
               case _scopes::global:
                  variable_scope = &MegaloVariableScopeGlobal;
                  break;
               case _scopes::player:
                  variable_scope = &MegaloVariableScopePlayer;
                  break;
               case _scopes::object:
                  variable_scope = &MegaloVariableScopeObject;
                  break;
               case _scopes::team:
                  variable_scope = &MegaloVariableScopeTeam;
                  break;
               case _scopes::round_time:
                  out = "game.round_timer";
                  return true;
               case _scopes::sudden_death_time:
                  out = "game.sudden_death_timer";
                  return true;
               case _scopes::grace_period_time:
                  out = "game.grace_period_timer";
                  return true;
               default:
                  out = "?????";
                  return false;
            }
            //
            auto     which_bits = variable_scope->which_bits();
            auto     index_bits = variable_scope->index_bits(ce_my_variable_type);
            uint16_t which = data.excerpt(offset, which_bits);
            offset += which_bits;
            uint16_t index = data.excerpt(offset, index_bits);
            //
            const char* which_scope;
            switch (scope) {
               case _scopes::global:
                  // which_bits is always zero for the global scope, and which is unused
                  cobb::sprintf(out, "global.timer[%d]", index);
                  break;
               case _scopes::player:
                  which_scope = megalo_players[which];
                  if (which_scope)
                     cobb::sprintf(out, "%s.timer[%u]", which_scope, index);
                  else
                     cobb::sprintf(out, "$invalid_player[%u].Timer[%u]", which, index);
                  break;
               case _scopes::object:
                  which_scope = megalo_objects[which];
                  if (which_scope)
                     cobb::sprintf(out, "%s.timer[%u]", which_scope, index);
                  else
                     cobb::sprintf(out, "$invalid_object[%u].timer[%u]", which, index);
                  break;
               case _scopes::team:
                  which_scope = megalo_teams[which];
                  if (which_scope)
                     cobb::sprintf(out, "%s.timer[%u]", which_scope, index);
                  else
                     cobb::sprintf(out, "$invalid_team[%u].timer[%u]", which, index);
                  break;
            }
            return true;
         },
         nullptr // TODO: "compile" functor
      );
   }
}