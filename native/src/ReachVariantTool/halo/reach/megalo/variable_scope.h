#pragma once
#include "helpers/unreachable.h"
#include "./variable_type.h"

namespace halo::reach::megalo {
   enum class variable_scope {
      global,
      object,
      player,
      team,
   };

   struct variable_scope_metadata {
      size_t max_numbers = 0;
      size_t max_objects = 0;
      size_t max_players = 0;
      size_t max_teams   = 0;
      size_t max_timers  = 0;

      constexpr size_t maximum_of_type(variable_type v) const {
         switch (v) {
            using enum variable_type;
            case number: return max_numbers;
            case object: return max_objects;
            case player: return max_players;
            case team:   return max_teams;
            case timer:  return max_timers;
         }
         return 0;
      }
      template<variable_type v> constexpr size_t maximum_of_type() const { // if MSVC functioned properly, the argument version would be constexpr-usable in all cases...
         switch (v) {
            using enum variable_type;
            case number: return max_numbers;
            case object: return max_objects;
            case player: return max_players;
            case team:   return max_teams;
            case timer:  return max_timers;
         }
         return 0;
      }

      constexpr size_t total() const {
         return max_numbers + max_objects + max_players + max_teams + max_timers;
      }
   };

   namespace variable_scopes {
      constexpr variable_scope_metadata none = {};

      constexpr variable_scope_metadata global = {
         .max_numbers = 12,
         .max_objects = 16,
         .max_players =  8,
         .max_teams   =  8,
         .max_timers  =  8,
      };
      constexpr variable_scope_metadata object = {
         .max_numbers = 8,
         .max_objects = 4,
         .max_players = 4,
         .max_teams   = 2,
         .max_timers  = 4,
      };
      constexpr variable_scope_metadata player = {
         .max_numbers = 8,
         .max_objects = 4,
         .max_players = 4,
         .max_teams   = 4,
         .max_timers  = 4,
      };
      constexpr variable_scope_metadata team = {
         .max_numbers = 8,
         .max_objects = 6,
         .max_players = 4,
         .max_teams   = 4,
         .max_timers  = 4,
      };
   }

   constexpr const variable_scope_metadata& variable_scope_metadata_from_enum(variable_scope s) {
      switch (s) {
         using enum variable_scope;
         case global: return variable_scopes::global;
         case object: return variable_scopes::object;
         case player: return variable_scopes::player;
         case team:   return variable_scopes::team;
      }
      if (std::is_constant_evaluated()) {
         throw;
      }
      return variable_scopes::none;
   }

   constexpr variable_scope variable_scope_enum_from_metadata(const variable_scope_metadata& m) {
      if (&m == &variable_scopes::global)
         return variable_scope::global;
      if (&m == &variable_scopes::object)
         return variable_scope::object;
      if (&m == &variable_scopes::player)
         return variable_scope::player;
      if (&m == &variable_scopes::team)
         return variable_scope::team;
      cobb::unreachable();
   }

   constexpr bool variable_type_has_a_scope(variable_type t) {
      switch (t) {
         using enum variable_type;
         case object:
         case player:
         case team:
            return true;
      }
      return false;
   }
   constexpr variable_scope variable_scope_for_type(variable_type t) {
      switch (t) {
         using enum variable_type;
         case object: return variable_scope::object;
         case player: return variable_scope::player;
         case team:   return variable_scope::team;
      }
      cobb::unreachable();
   }
   constexpr variable_type variable_type_for_scope(variable_scope t) {
      switch (t) {
         using enum variable_scope;
         case object: return variable_type::object;
         case player: return variable_type::player;
         case team:   return variable_type::team;
      }
      cobb::unreachable();
   }
}