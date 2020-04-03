#pragma once
#include <cassert>
#include <vector>
#include "enums.h"
#include "../../../formats/detailed_enum.h"

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

   extern const DetailedEnum megalo_scope_does_not_have_specifier; // there is only one Global scope; you don't need to specify "which Global scope" a variable is in
   extern const DetailedEnum megalo_objects;
   extern const DetailedEnum megalo_players;
   extern const DetailedEnum megalo_teams;

   class VariableScope {
      public:
         constexpr VariableScope(const DetailedEnum& list, uint8_t n, uint8_t i, uint8_t e, uint8_t p, uint8_t o) :
            list(list),
            max_scalars(n),
            max_timers(i),
            max_teams(e),
            max_players(p),
            max_objects(o)
         {}
         //
         const DetailedEnum& list;
         uint8_t max_scalars;
         uint8_t max_timers;
         uint8_t max_teams;
         uint8_t max_players;
         uint8_t max_objects;
         //
         const int max_variables_of_type(const variable_type vt) const noexcept {
            switch (vt) {
               case variable_type::scalar: return this->max_scalars;
               case variable_type::timer:  return this->max_timers;
               case variable_type::team:   return this->max_teams;
               case variable_type::player: return this->max_players;
               case variable_type::object: return this->max_objects;
            }
            return 0;
         }
         const int which_bits() const noexcept { return this->list.index_bits(); }
         const int count_bits(const variable_type vt) const noexcept {
            return cobb::bitcount(this->max_variables_of_type(vt));
         }
         const int index_bits(const variable_type vt) const noexcept {
            return cobb::bitcount(this->max_variables_of_type(vt) - 1);
         }
         //
         bool is_valid_which(int which) const noexcept {
            if (this->list.size() == 0)
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