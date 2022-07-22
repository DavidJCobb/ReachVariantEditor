#pragma once
#include "./parser.h"

namespace halo::reach::megalo::AST {
   template<const binary_operator_tier_definition& Tier> expression* parser::_try_rule_expression_binary<Tier>() {
      constexpr auto* Next = Tier.next;

      auto _get_side = [this, Next]() {
         if constexpr (Next)
            return this->try_parse_binary_op<*Next>();
         else
            return this->try_parse_unary_op();
      };

      auto* expr = _get_side();
      while (consume(Tier._operators)) { static_assert(false, "TODO: implement a 'consume' function that will work here");
         auto  op  = previous_token(); static_assert(false, "TODO: implement a 'previous_token' function that will work here");
         auto* rhs = _get_side();
         expr = expression::alloc_binary(expr, op, rhs);
      }
      return expr;
   };
}