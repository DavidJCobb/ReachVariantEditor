#pragma once
#include <functional>
#include "../../../megalo/variables_and_scopes.h"
#include "../../opcode_arg.h"
#include "../../../../helpers/strings.h"
#include "../../../../errors.h"

/*
// MEGALO VARIABLES
//
// All of the variable types are organized in a common manner: there is a scope-indicator value; 
// a "which" value; and an "index" value.
//
// The scope-indicator is an enum (with different values for each variable type) which indicates 
// where we are pulling a variable from. It can indicate a variable scope (global, object, player, 
// or team), but it can also indicate a property (e.g. a player's owner team) or a game state 
// value (e.g. the round timer, or the friendly fire bool).
//
// The "which" value, when present, indicates which variable serves as our scope. The "index" 
// value, when present, indicates which variable we are accessing. These values are only present 
// if the scope-indicator in question actually uses them.
//
// Some examples:
//
//                                     | SCOPE             | WHICH            | INDEX
//    ---------------------------------+-------------------+------------------+--------
//    global.number[3]                 | global            | <none>           | 3
//    global.player[1].number[2]       | player            | global.player[1] | 2
//    current_player.number[2]         | player            | current_player   | 2
//    game.betrayal_booting            | betrayal_booting  | <none>           | <none>
//    global.player[1].team            | player_owner_team | global.player[1] | <none>
//    global.player[1].object[0].biped | player_biped      | global.player[1] | 0
//
// Each variable type has its own enum of scope-indicators, since the scope-indicator does 
// double duty and handles game state values and properties along with nested and non-nested 
// variables. Notably, while all variable types have variable-scopes in their respective scope-
// indicator enums, the variable-scopes aren't always listed in the same order.
//
// The "which" value can only refer to the variable to which access is scoped, i.e. a globally-
// accessible object, player, or team. As such, it is always an entry in the (megalo_objects), 
// (megalo_players), or (megalo_teams) enums defined in <variables_and_scopes.h>.
//
// I've chosen to handle all of this generically. For each variable type I will be defining an 
// instance of VariableScopeIndicatorValueList, which will contain VariableScopeIndicatorValue 
// instances. Each VariableScopeIndicatorValue instance will specify whether and how to handle 
// a "which" value and an "index" value, as well as functors (if needed) for postprocessing. 
// Helper functions on the VariableScopeIndicatorValueList will be available to the various 
// OpcodeArgTypeinfo functors, such that we will not need variable-type-specific code for 
// loading, stringifying, or postprocessing variables that appear as opcode arguments.
*/

namespace MegaloEx {
   using namespace Megalo; // hack until we swap everything over
}

namespace MegaloEx {
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
      const VariableScopeIndicatorValue* scope_instance = nullptr;
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
               is_readonly = 0x01, // for the compiler; indicates that values in this scope-indicator cannot be written to at run-time
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
         const char*    format         = "%w.timer[%i]"; // format string for decompiled code; interpreted manually; %w = the "which" as a string; %i = index as a number
         const char*    format_english = "%w's timer[%i]";
         //
         decode_functor_t      index_decompile   = nullptr; // if present, overrides default handling i.e. format strings above
         decode_functor_t      index_to_english  = nullptr; // if present, overrides default handling i.e. format strings above
         postprocess_functor_t index_postprocess = nullptr;
         //
         inline bool has_index() const noexcept { return this->index_type != index_type::none; }
         inline bool has_which() const noexcept { return this->base != nullptr; }
         inline bool is_readonly() const noexcept { return this->flags & flags::is_readonly; }
         //
         int which_bits() const noexcept;
         int index_bits() const noexcept;
   };
   class VariableScopeIndicatorValueList {
      private:
         using _decoded = _DecodedVariable;
         //
      private:
         _decoded _decode_to_struct(arg_functor_state fs, cobb::bitarray128& data) const;
         //
      public:
         std::vector<VariableScopeIndicatorValue> scopes;
         inline int scope_bits() const noexcept {
            return cobb::bitcount(this->scopes.size() - 1);
         }
         //
         // The following functions are made available to call from the various functors on OpcodeArgTypeinfo: respectively: 
         // load_functor_t; postprocess_functor_t; decode_functor_t; and decode_functor_t (yes, twice; decompiling and printing 
         // in English have the same functor type).
         //
         bool load(arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) const;
         bool postprocess(arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, GameVariantData* variant) const;
         bool decompile(arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) const;
         bool to_english(arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) const;
   };
}