#pragma once
#include <array>
#include "halo/util/ct_vector.h"
#include "./expression.h"
#include "./token_type.h"

namespace halo::reach::megalo::bolt {
   constexpr auto all_assignment_operators = std::array{
      token_type::equal,               // =
      token_type::operator_assign_add, // +=
      token_type::operator_assign_and, // &=
      token_type::operator_assign_div, // /=
      token_type::operator_assign_mod, // %=
      token_type::operator_assign_mul, // *=
      token_type::operator_assign_not, // ~=
      token_type::operator_assign_or,  // |=
      token_type::operator_assign_shl, // <<=
      token_type::operator_assign_shr, // >>=
      token_type::operator_assign_sub, // -=
      token_type::operator_assign_xor, // ^=
   };

   // A collection of binary (two-term) operators which share the same precedence.
   struct binary_operator_tier_definition {
      std::array<token_type, 12> operators = {};

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

      template<size_t Size> requires (Size <= std::tuple_size_v<decltype(operators)>) constexpr binary_operator_tier_definition(const std::array<token_type, Size>& items) {
         size_t i = 0;
         for (; i < Size; ++i)
            operators[i] = items[i];
         for (; i < operators.size(); ++i)
            operators[i] = token_type::none;
      }
   };

   // In order of ascending precedence.
   constexpr auto binary_operator_tiers = std::array{
      binary_operator_tier_definition(all_assignment_operators),
      binary_operator_tier_definition(    // ALL NON-RELATIONAL COMPARISONS
         token_type::operator_compare_eq, // == // is equal to
         token_type::operator_compare_ne  // != // is not equal to
      ),
      binary_operator_tier_definition(    // ALL RELATIONAL COMPARISONS
         token_type::angle_bracket_l,     // <
         token_type::angle_bracket_r,     // >
         token_type::operator_compare_le, // <=
         token_type::operator_compare_ge  // >=
      ),
      binary_operator_tier_definition(
         token_type::pipe                 // | // binary OR
      ),
      binary_operator_tier_definition(
         token_type::caret                // ^ // binary XOR
      ),
      binary_operator_tier_definition(
         token_type::ampersand            // & // binary AND
      ),
      binary_operator_tier_definition(    // ALL BITSHIFTS
         token_type::operator_binary_shl, // << // binary shift left
         token_type::operator_binary_shr  // >> // binary shift right
      ),
      binary_operator_tier_definition(
         token_type::plus,                // + // add
         token_type::minus                // - // subtract
      ),
      binary_operator_tier_definition(
         token_type::asterisk,            // * // multiply
         token_type::percent,             // % // remainder
         token_type::slash_fwd            // / // divide
      ),
   };
}