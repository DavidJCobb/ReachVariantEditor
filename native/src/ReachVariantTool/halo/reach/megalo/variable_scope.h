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

      template<variable_type V> constexpr size_t maximum_of_type() const {
         switch (V) {
            using enum variable_type;
            case number: return max_numbers;
            case object: return max_objects;
            case player: return max_players;
            case team:   return max_teams;
            case timer:  return max_timers;
         }
         cobb::unreachable();
      }

      constexpr size_t total() const {
         return max_numbers + max_objects + max_players + max_teams + max_timers;
      }
   };

   namespace variable_scopes {
      static constexpr variable_scope_metadata global = {
         .max_numbers = 12,
         .max_objects = 16,
         .max_players =  8,
         .max_teams   =  8,
         .max_timers  =  8,
      };
      static constexpr variable_scope_metadata object = {
         .max_numbers = 8,
         .max_objects = 4,
         .max_players = 4,
         .max_teams   = 2,
         .max_timers  = 4,
      };
      static constexpr variable_scope_metadata player = {
         .max_numbers = 8,
         .max_objects = 4,
         .max_players = 4,
         .max_teams   = 4,
         .max_timers  = 4,
      };
      static constexpr variable_scope_metadata team = {
         .max_numbers = 8,
         .max_objects = 6,
         .max_players = 4,
         .max_teams   = 4,
         .max_timers  = 4,
      };
   }

   template<variable_scope S> constexpr const variable_scope_metadata& variable_scope_metadata_from_enum() {
      switch (S) {
         using enum variable_scope;
         case global: return variable_scopes::global;
         case object: return variable_scopes::object;
         case player: return variable_scopes::player;
         case team:   return variable_scopes::team;
      }
      cobb::unreachable();
   }
}