#pragma once
#include <array>
#include "halo/util/ct_vector.h"
#include "expression.h"

namespace halo::reach::megalo::AST {

   // Compile-time metadata for a binary (two-term) operator.
   struct binary_operator_definition {
      const char*   token = nullptr;
      expression_op code  = expression_op::none;
      //
      binary_operator_definition* next = nullptr;
   };

   // A collection of binary (two-term) operators which share the same precedence.
   struct binary_operator_tier_definition {
      std::array<binary_operator_definition, 6> _operators;

      template<typename... Args> constexpr binary_operator_tier_definition(Args&&... defs) {
         size_t i = 0;
         (
            (
               _operators[i] = defs,
               _operators[i].next = (i + 1 < _operators.size() ? &_operators[i + 1] : nullptr),
               ++i
            ),
            ...
         );
      }
   };

   // in order of ascending precedence:
   constexpr auto binary_operator_tiers = std::array{
      binary_operator_tier_definition(
         binary_operator_definition{ "==", expression_op::compare_eq },
         binary_operator_definition{ "==", expression_op::compare_ne }
      ),
      binary_operator_tier_definition(
         binary_operator_definition{ "<",  expression_op::compare_l },
         binary_operator_definition{ ">",  expression_op::compare_g },
         binary_operator_definition{ "<=", expression_op::compare_le },
         binary_operator_definition{ ">=", expression_op::compare_ge }
      ),
      binary_operator_tier_definition(
         binary_operator_definition{ "+", expression_op::binary_add },
         binary_operator_definition{ "-", expression_op::binary_sub }
      ),
      binary_operator_tier_definition(
         binary_operator_definition{ "*", expression_op::binary_mul },
         binary_operator_definition{ "/", expression_op::binary_div }
      ),
   };
}