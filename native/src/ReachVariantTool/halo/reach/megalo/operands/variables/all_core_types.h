#pragma once
#include "number.h"
#include "object.h"
#include "player.h"
#include "team.h"
#include "timer.h"

namespace halo::reach::megalo::operands {
   namespace impl {
      template<variable_type V> struct variable_operand_type;
      template<> struct variable_operand_type<variable_type::number> {
         using type = variables::number;
      };
      template<> struct variable_operand_type<variable_type::object> {
         using type = variables::object;
      };
      template<> struct variable_operand_type<variable_type::player> {
         using type = variables::player;
      };
      template<> struct variable_operand_type<variable_type::team> {
         using type = variables::team;
      };
      template<> struct variable_operand_type<variable_type::timer> {
         using type = variables::timer;
      };
   }
   template<variable_type V> using variable_operand_type = typename impl::variable_operand_type<V>::type;

   extern variables::unknown_type* make_variable_of_type(variable_type v) {
      switch (v) {
         case variable_type::number: return new variables::number;
         case variable_type::object: return new variables::object;
         case variable_type::player: return new variables::player;
         case variable_type::team:   return new variables::team;
         case variable_type::timer:  return new variables::timer;
      }
      if (std::is_constant_evaluated()) {
         throw;
      }
      return nullptr;
   }
}