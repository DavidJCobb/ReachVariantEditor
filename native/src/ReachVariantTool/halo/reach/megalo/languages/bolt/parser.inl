#pragma once
#include "./parser.h"

namespace halo::reach::megalo::bolt {
   template<token_type... TokenTypes>
   bool parser::_check_token_type_sequence_ahead() const {
      auto& list = this->scanner.tokens;
      auto  i    = this->next_token;
      if (i + sizeof...(TokenTypes) >= list.size())
         return false;
      bool result = true;
      (
         (result ? (result = list[i++].type == TokenTypes) : false),
         ...
      );
      return result;
   }

   template<token_type... TokenTypes>
   size_t parser::_find_any_token_of_types() const {
      auto& list = this->scanner.tokens;
      auto  i    = this->next_token;
      for (; i < list.size(); ++i) {
         bool match = ((match ? match : list[i].type == TokenTypes), ...);
         if (match)
            return i;
      }
      return size_t{ -1 };
   }

   template<token_type Type>
   bool parser::_consume_token_if_present() {
      if (this->_check_next_token(Type)) {
         this->_pull_next_token();
         return true;
      }
      return false;
   }

   template<token_type... TokenTypes>
   bool parser::_consume_any_token_of_types() {
      bool result = false;
      (
         (result ?
            (this->_check_next_token(TokenTypes) ? (result = true) : false)
         :
            false
         ),
         ...
      );
      if (result)
         ++this->next_token;
      return result;
   }
   template<auto TokenTypes> requires cobb::is_std_array_of_type<std::decay_t<decltype(TokenTypes)>, token_type>
   bool parser::_consume_any_token_of_types() {
      for (auto t : TokenTypes) {
         if (t == token_type::none)
            break;
         if (this->_check_next_token(t)) {
            ++this->next_token;
            return true;
         }
      }
      return false;
   }

   template<size_t TierIndex> expression* parser::_try_rule_expression_binary() {
      constexpr auto& tier = binary_operator_tiers[TierIndex];

      auto _get_side = [this]() {
         if constexpr (TierIndex + 1 < binary_operator_tiers.size())
            return this->_try_rule_expression_binary<TierIndex + 1>();
         else
            return this->_try_rule_expression_unary();
      };

      auto* expr = _get_side();
      while (this->_consume_any_token_of_types<tier.operators>()) {
         auto  op  = this->_previous_token();
         auto* rhs = _get_side();
         expr = expression::alloc_binary(expr, op->type, rhs);
      }
      return expr;
   };
}