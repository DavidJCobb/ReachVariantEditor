#include "parser.h"
#include "expression.h"

namespace halo::reach::megalo::AST {
   bool parser::is_at_end() const {
      return this->next_token >= this->scanner.tokens.size();
   }

   bool parser::_check_next_token(token_type t) const {
      if (this->is_at_end())
         return false;
      return this->_peek_next_token().type == t;
   };
   const token& parser::_peek_next_token() const {
      return this->scanner.tokens[this->next_token];
   }
   const token& parser::_pull_next_token() {
      if (!this->is_at_end())
         ++this->next_token;
      return *this->_previous_token();
   }
   const token* parser::_previous_token() const {
      if (this->next_token == 0)
         return nullptr;
      return &this->scanner.tokens[this->next_token - 1];
   }

   void parser::_require_and_consume_token(token_type t, const char* error_message_translation_key) {
      if (this->_check_next_token(t)) {
         this->_pull_next_token();
         return;
      }
      throw error(_peek_next_token(), error_message_translation_key);
   }

   // --- Rules ---

   void parser::_try_rule_statement();
   bool parser::_try_rule_keyword();
   bool parser::_try_rule_alias();
   bool parser::_try_rule_declare();
   bool parser::_try_rule_enum();
   bool parser::_try_rule_block();
   bool parser::_try_rule_block_actions();
   bool parser::_try_rule_block_event();
   bool parser::_try_rule_block_conditions();

   expression* parser::_try_rule_expression() {
      return this->_try_rule_expression_binary<0>();
   }
   expression* parser::_try_rule_expression_unary() {
      if (this->_consume_any_token_of_types<token_type::exclamation, token_type::plus, token_type::minus>()) {
         auto* token_op = this->_previous_token();
         auto* rhs      = this->_try_rule_expression_unary();
         return expression::alloc_unary(token_op->type, rhs);
      }
      return this->_try_rule_expression_primary();
   }
   expression* parser::_try_rule_expression_primary() {
      if (this->_consume_any_token_of_types<token_type::number, token_type::string>()) {
         return expression::alloc_literal(this->_previous_token()->literal);
      }
      if (this->_consume_any_token_of_types<token_type::paren_l>()) {
         auto* content = this->_try_rule_expression();
         this->_require_and_consume_token(token_type::paren_r, "Expected ')' after expression.");
         return expression::alloc_grouping(content);
      }
   }

}