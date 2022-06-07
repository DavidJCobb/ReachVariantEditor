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

   extern variables::unknown_type* make_variable_of_type(variable_type v);
}