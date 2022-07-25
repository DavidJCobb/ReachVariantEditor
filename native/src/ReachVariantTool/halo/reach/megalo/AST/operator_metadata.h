#pragma once
#include <array>
#include "halo/util/ct_vector.h"
#include "expression.h"
#include "token_type.h"

namespace halo::reach::megalo::AST {
   // A collection of binary (two-term) operators which share the same precedence.
   struct binary_operator_tier_definition {
      std::array<token_type, 6> operators = {};

      template<typename... Args> requires (std::is_same_v<Args, token_type> && ...) constexpr binary_operator_tier_definition(Args... defs) {
         size_t i = 0;
         (
            (
               operators[i] = defs,
               ++i
            ),
            ...
         );
         for (; i < operators.size(); ++i)
            operators[i] = token_type::none;
      }
   };

   // In order of ascending precedence.
   constexpr auto binary_operator_tiers = std::array{
      binary_operator_tier_definition(
         token_type::operator_compare_eq, // ==
         token_type::operator_compare_ne  // !=
      ),
      binary_operator_tier_definition(
         token_type::angle_bracket_l,     // <
         token_type::angle_bracket_r,     // >
         token_type::operator_compare_le, // <=
         token_type::operator_compare_ge  // >=
      ),
      binary_operator_tier_definition(
         token_type::plus,                // +
         token_type::minus                // -
      ),
      binary_operator_tier_definition(
         token_type::asterisk,            // *
         token_type::slash_fwd            // /
      ),
   };
}