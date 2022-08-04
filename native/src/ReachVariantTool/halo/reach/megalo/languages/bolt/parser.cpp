#include "parser.h"
#include "./expression.h"

namespace halo::reach::megalo::bolt {
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

   void parser::_try_rule_statement() {
      if (this->_try_rule_keyword())
         return;
      if (this->_try_rule_expression())
         return;
      static_assert(false, "TODO: Handle unrecognized content here");
   }
   bool parser::_try_rule_keyword() {
      if (this->_try_rule_alias())
         return true;
      if (this->_try_rule_block())
         return true;
      if (this->_try_rule_declare())
         return true;
      if (this->_try_rule_enum())
         return true;
      if (this->_try_rule_pragma())
         return true;
      return false;
   }
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
      if (this->_consume_any_token_of_types<token_type::identifier_or_word>()) {
         auto lit = this->_previous_token()->literal;
         //
         // Check if this is a function call:
         //
         if (this->_consume_any_token_of_types<token_type::paren_l>()) { // function-call-argument-list
            //
            // Function call.
            //
            std::vector<function_call_argument> args;
            //
            if (!this->_check_next_token(token_type::paren_r)) {
               do {
                  //
                  // Extract argument.
                  //
                  bool    argument_is_named = false;
                  QString argument_name;
                  if (this->_check_token_type_sequence_ahead<token_type::identifier_or_word, token_type::equal>()) {
                     {  // Consume argument name.
                        auto& next = this->_pull_next_token();
                        assert(next.type == token_type::identifier_or_word);
                        assert(std::holds_alternative<literal_data_identifier_or_word>(next.literal.value));
                        argument_name = std::get<literal_data_identifier_or_word>(next.literal.value).content;
                     }
                     {  // Consume equal sign.
                        auto& next = this->_pull_next_token();
                        assert(next.type == token_type::equal);
                     }
                  }
                  cobb::owned_ptr expr = this->_try_rule_expression();
                  if (!expr) {
                     static_assert(false, "TODO: error and continue to next ',' or ')'");
                  }
                  auto& argument = args.emplace_back();
                  if (argument_is_named)
                     std::swap(argument.name, argument_name);
                  std::swap(argument.value, expr);
               } while (!this->_check_next_token(token_type::comma));
            }
            this->_require_and_consume_token(token_type::paren_r, "Expected ')' after expression.");

            auto* expr = expression::alloc_call(lit);
            std::swap(expr->arguments, args);
            return expr;
         }
         // just an identifier
         return expression::alloc_literal(lit);
      }
      if (this->_consume_any_token_of_types<token_type::paren_l>()) {
         auto* content = this->_try_rule_expression();
         this->_require_and_consume_token(token_type::paren_r, "Expected ')' after expression.");
         return expression::alloc_grouping(content);
      }
   }

}