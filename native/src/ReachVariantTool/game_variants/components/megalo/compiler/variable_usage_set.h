#pragma once
#include <bitset>
#include "../variables_and_scopes.h"

namespace Megalo::Script {
   struct variable_usage_set {
      struct {
         std::bitset<12> numbers;
         std::bitset<16> objects;
         std::bitset<8>  players;
         std::bitset<8>  teams;
         std::bitset<8>  timers;
      } global;
      struct {
         std::bitset<10> numbers;
         std::bitset<8>  objects;
         std::bitset<3>  players;
         std::bitset<6>  teams;
      } temporary;
      
      constexpr bool is_variable_marked(variable_scope scope, variable_type type, size_t index) const noexcept {
         switch (scope) {
            case variable_scope::global:
               {
                  auto& scope = this->global;
                  switch (type) {
                     case variable_type::scalar: return scope.numbers.test(index);
                     case variable_type::object: return scope.objects.test(index);
                     case variable_type::player: return scope.players.test(index);
                     case variable_type::team:   return scope.teams.test(index);
                     case variable_type::timer:  return scope.timers.test(index);
                  }
               }
               break;
            case variable_scope::temporary:
               {
                  auto& scope = this->temporary;
                  switch (type) {
                     case variable_type::scalar: return scope.numbers.test(index);
                     case variable_type::object: return scope.objects.test(index);
                     case variable_type::player: return scope.players.test(index);
                     case variable_type::team:   return scope.teams.test(index);
                  }
               }
               break;
         }
         return false;
      }
      constexpr void mark_variable(variable_scope scope, variable_type type, size_t index) noexcept {
         switch (scope) {
            case variable_scope::global:
               {
                  auto& scope = this->global;
                  switch (type) {
                     case variable_type::scalar: scope.numbers.set(index); return;
                     case variable_type::object: scope.objects.set(index); return;
                     case variable_type::player: scope.players.set(index); return;
                     case variable_type::team:   scope.teams.set(index);   return;
                     case variable_type::timer:  scope.timers.set(index);  return;
                  }
               }
               break;
            case variable_scope::temporary:
               {
                  auto& scope = this->temporary;
                  switch (type) {
                     case variable_type::scalar: scope.numbers.set(index); return;
                     case variable_type::object: scope.objects.set(index); return;
                     case variable_type::player: scope.players.set(index); return;
                     case variable_type::team:   scope.teams.set(index);   return;
                  }
               }
               break;
         }
      }
      
      constexpr variable_usage_set& operator&=(const variable_usage_set& other) {
         this->global.numbers &= other.global.numbers;
         this->global.objects &= other.global.objects;
         this->global.players &= other.global.players;
         this->global.teams   &= other.global.teams;
         this->global.timers  &= other.global.timers;
         this->temporary.numbers &= other.temporary.numbers;
         this->temporary.objects &= other.temporary.objects;
         this->temporary.players &= other.temporary.players;
         this->temporary.teams   &= other.temporary.teams;
         return *this;
      }

   };
}