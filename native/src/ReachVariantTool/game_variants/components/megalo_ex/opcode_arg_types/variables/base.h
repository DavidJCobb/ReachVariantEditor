#pragma once
#include <functional>
#include "../../../megalo/variables_and_scopes.h"
#include "../../opcode_arg.h"
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
      //    necessarily be constrained to VariableScope::index_bits(type) for the "which" in question.
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
      class VariableScopeIndicatorValue;
      struct _DecodedVariable {
         //
         // Something intermediate. Once we've extracted the scope/which/index from a loaded variable's 
         // data, we can then just index into everything however we need to.
         //
         uint8_t scope; // bitcount: VariableScopeIndicatorValueList::scopes.size()
         uint8_t which = 0;
         int16_t index = 0;
         VariableScopeIndicatorValue* scope_instance = nullptr;
      };

      class VariableScopeIndicatorValue {
         private:
            using _decoded = _DecodedVariable;
            //
         public:
            struct flags {
               flags() = delete;
               enum type : uint8_t {
                  none = 0,
                  is_readonly = 0x01,
               };
            };
            using flags_t = std::underlying_type_t<flags::type>;
            //
            using decode_functor_t      = std::function<bool(_decoded& data, arg_rel_obj_list_t& relObjs, std::string& out)>; // returns success/failure
            using postprocess_functor_t = std::function<cobb::indexed_refcountable* (GameVariantData* variant, uint32_t index)>; // for indexed data, access the indexed list and return a bare pointer; caller will jam that into a refcounted pointer
            //
            enum class index_type : uint8_t {
               none, // there is no "index" value
               number,
               object,
               player,
               team,
               timer,
               indexed_data, // e.g. script options, script stats; specify bitcount in (index_bitcount)
            };
            //
            uint8_t        id             = 0; // the _scopes value
            flags_t        flags          = flags::none;
            index_type     index_type     = index_type::none;
            uint8_t        index_bitcount = 0; // use non-zero values for index_type::indexed_data
            VariableScope* base           = nullptr; // used to deduce whether a value's (which) is from megalo_objects, megalo_players, or megalo_teams
            const char*    format         = "%w.timer[%i]";
            //
            decode_functor_t      index_decode      = nullptr;
            decode_functor_t      index_to_english  = nullptr;
            postprocess_functor_t index_postprocess = nullptr;
            //
            inline bool has_index() const noexcept { return this->index_type != index_type::none; }
            inline bool has_which() const noexcept { return this->base != nullptr; }
            inline bool is_readonly() const noexcept { return this->flags & flags::is_readonly; }
            //
            int which_bits() const noexcept {
               if (this->base)
                  return this->base->which_bits();
               return 0;
            }
            int index_bits() const noexcept {
               switch (this->index_type) {
                  case index_type::none:
                     return 0;
                  case index_type::number:
                     assert(this->base, "Our scope-indicator definitions are bad. Variables cannot be unscoped.");
                     return this->base->index_bits(Megalo::variable_type::scalar);
                  case index_type::object:
                     assert(this->base, "Our scope-indicator definitions are bad. Variables cannot be unscoped.");
                     return this->base->index_bits(Megalo::variable_type::object);
                  case index_type::player:
                     assert(this->base, "Our scope-indicator definitions are bad. Variables cannot be unscoped.");
                     return this->base->index_bits(Megalo::variable_type::player);
                  case index_type::team:
                     assert(this->base, "Our scope-indicator definitions are bad. Variables cannot be unscoped.");
                     return this->base->index_bits(Megalo::variable_type::team);
                  case index_type::timer:
                     assert(this->base, "Our scope-indicator definitions are bad. Variables cannot be unscoped.");
                     return this->base->index_bits(Megalo::variable_type::timer);
                  case index_type::indexed_data:
                     assert(this->index_bitcount, "Our scope-indicator definitions are bad. Definition is set to use indexed data, but offers no bitcount.");
                     return this->index_bitcount;
               }
               #if _DEBUG
                  assert(false, "We've reached unreachable code!");
               #else
                  __assume(0); // suppress "not all paths return a value" by telling MSVC this is unreachable
               #endif
            }
      };
      class VariableScopeIndicatorValueList {
         private:
            using _decoded = _DecodedVariable;
            //
         private:
            _decoded _decode_to_struct(arg_functor_state fs, cobb::bitarray128& data) {
               _decoded out;
               auto of = fs.bit_offset;
               auto bc = cobb::bitcount(this->scopes.size() - 1);
               out.scope = data.excerpt(of, bc);
               if (out.scope >= this->scopes.size())
                  return out;
               of += bc;
               auto& scope = this->scopes[out.scope];
               out.scope_instance = &scope;
               if (scope.has_which()) {
                  bc = scope.which_bits();
                  out.which = data.excerpt(of, bc);
                  of += bc;
               }
               if (scope.has_index()) {
                  bc = scope.index_bits();
                  out.index = data.excerpt(of, bc);
                  of += bc;
               }
               return out;
            }
            //
         public:
            std::vector<VariableScopeIndicatorValue> scopes;
            inline int scope_bits() const noexcept {
               return cobb::bitcount(this->scopes.size() - 1);
            }
            //
            // TODO: helper functions analogous to the various functors used by OpcodeArgValue
            //
            /*
            using load_functor_t        = std::function<bool(arg_functor_state, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits)>; // loads data from binary stream; returns success/failure
            using decode_functor_t      = std::function<bool(arg_functor_state, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out)>; // returns success/failure
            using compile_functor_t     = std::function<arg_consume_result(arg_functor_state, OpcodeArgValue&, arg_rel_obj_list_t& relObjs, const std::string&, Compiler&)>;
            using postprocess_functor_t = std::function<bool(arg_functor_state, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, GameVariantData*)>;
            */
            bool load(arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) {
               //
               // Call from OpcodeArgValue::load_functor_t.
               //
               uint8_t si = (uint64_t)data.consume(input_bits, this->scope_bits());
               if (si >= this->scopes.size())
                  return false; // TODO: specific error messaging
               auto& scope = this->scopes[si];
               if (scope.has_which())
                  data.consume(input_bits, scope.which_bits());
               if (scope.has_index()) {
                  auto bc = scope.index_bits();
                  if (scope.index_type == VariableScopeIndicatorValue::index_type::indexed_data) {
                     auto& range = relObjs.ranges[fs.obj_index];
                     range.start = data.size;
                     range.count = bc;
                  }
                  data.consume(input_bits, bc);
               }
               return true;
            }
            bool postprocess(arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, GameVariantData* variant) {
               //
               // Call from OpcodeArgValue::postprocess_functor_t.
               //
               uint8_t si = (uint64_t)data.excerpt(fs.bit_offset, this->scope_bits());
               if (si >= this->scopes.size())
                  return false; // TODO: specific error messaging
               auto& scope = this->scopes[si];
               if (scope.index_type != VariableScopeIndicatorValue::index_type::indexed_data) // there's nothing to do postprocess for
                  return true;
               auto& range = relObjs.ranges[fs.obj_index];
               int16_t index = (uint64_t)data.excerpt(range.start, range.count);
               //
               assert(scope.index_postprocess, "Our scope-indicator definitions are bad. A scope uses indexed-data but has no postprocess functor.");
               relObjs.pointers[fs.obj_index] = (scope.index_postprocess)(variant, index);
            }
            bool decompile(arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) {
               //
               // Call from OpcodeArgValue::decode_functor_t, for decompiling.
               //
               auto decoded = this->_decode_to_struct(fs, data);
               if (!decoded.scope_instance)
                  return false;
               auto& scope = *decoded.scope_instance;
               return (scope.index_decode)(decoded, relObjs, out);
            }
            bool to_english(arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) {
               //
               // Call from OpcodeArgValue::decode_functor_t, for stringifying to English.
               //
               auto decoded = this->_decode_to_struct(fs, data);
               if (!decoded.scope_instance)
                  return false;
               auto& scope = *decoded.scope_instance;
               return (scope.index_to_english)(decoded, relObjs, out);
            }
      };
   }
}