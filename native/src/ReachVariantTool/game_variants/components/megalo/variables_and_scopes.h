#pragma once
#include <cassert>
#include <vector>
#include "enums.h"

namespace Megalo {
   enum class variable_scope {
      global,
      player,
      object,
      team,
      //
      not_a_scope = -1,
   };
   enum class variable_type { // must match the game
      scalar = 0,
      player = 1,
      object = 2,
      team   = 3,
      timer  = 4,
      //
      not_a_variable = -1, // needed for "All Players" values in team-or-player vars
   };

   extern const SmartEnum megalo_scope_does_not_have_specifier; // there is only one Global scope; you don't need to specify "which Global scope" a variable is in
   extern const SmartEnum megalo_objects;
   extern const SmartEnum megalo_players;
   extern const SmartEnum megalo_teams;

   class VariableScope {
      public:
         constexpr VariableScope(const SmartEnum& list, uint8_t n, uint8_t i, uint8_t e, uint8_t p, uint8_t o) : 
            list(list),
            max_scalars(n),
            max_timers(i),
            max_teams(e),
            max_players(p),
            max_objects(o)
         {}
         //
         const SmartEnum& list;
         uint8_t max_scalars;
         uint8_t max_timers;
         uint8_t max_teams;
         uint8_t max_players;
         uint8_t max_objects;
         //
         const int which_bits() const noexcept { return this->list.index_bits(); }
         const int count_bits(const variable_type vt) const noexcept {
            switch (vt) {
               case variable_type::scalar: return cobb::bitcount(this->max_scalars);
               case variable_type::timer:  return cobb::bitcount(this->max_timers);
               case variable_type::team:   return cobb::bitcount(this->max_teams);
               case variable_type::player: return cobb::bitcount(this->max_players);
               case variable_type::object: return cobb::bitcount(this->max_objects);
            }
            assert(false && "Unrecognized variable type.");
            return 0;
         }
         const int index_bits(const variable_type vt) const noexcept {
            switch (vt) {
               case variable_type::scalar: return cobb::bitcount(this->max_scalars - 1);
               case variable_type::timer:  return cobb::bitcount(this->max_timers  - 1);
               case variable_type::team:   return cobb::bitcount(this->max_teams   - 1);
               case variable_type::player: return cobb::bitcount(this->max_players - 1);
               case variable_type::object: return cobb::bitcount(this->max_objects - 1);
            }
            assert(false && "Unrecognized variable type.");
            return 0;
         }
         //
         bool is_valid_which(int which) const noexcept {
            if (this->list.count == 0)
               return which == 0;
            return this->list.is_in_bounds(which);
         }
   };

   extern const VariableScope MegaloVariableScopeGlobal;
   extern const VariableScope MegaloVariableScopePlayer;
   extern const VariableScope MegaloVariableScopeObject;
   extern const VariableScope MegaloVariableScopeTeam;

   const VariableScope& getScopeObjectForConstant(variable_scope) noexcept;
   variable_scope getScopeConstantForObject(const VariableScope&) noexcept;
}