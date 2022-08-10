#include "parser.h"
#include "./expression.h"
#include "./errors/base_exception.h"
#include "./errors/function_call_argument_parse_error.h"
#include "./errors/token_matches_no_grammar_rule.h"
#include "./errors/unterminated_function_call_argument_list.h"
#include "./errors/unterminated_grouping_expression.h"

namespace {
   constexpr bool just_let_me_compile_it_for_now_so_i_can_test = false ||
         #if _DEBUG
            true
         #else
            false
         #endif
      ;
}

namespace halo::reach::megalo::bolt {
   parser::~parser() {
      for (auto* item : this->errors)
         delete item;
      this->errors.clear();
   }

   void parser::run_debug_test() {
      this->scanner.scan_tokens();
      auto* result = this->_try_rule_expression();
      __debugbreak();
      if (result)
         delete result;
   }

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

   // --- Rules ---

   void parser::_try_rule_statement() {
      if (this->_try_rule_keyword())
         return;
      if (this->_try_rule_expression())
         return;
      //
      errors::token_matches_no_grammar_rule error;
      error.fault = this->_peek_next_token();
      error.pos   = error.fault.start;
      throw errors::parse_exception(error);
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

   bool parser::_try_rule_alias() {
      static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: IMPLEMENT ME");
      return false;
   }
   bool parser::_try_rule_declare() {
      static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: IMPLEMENT ME");
      return false;
   }
   bool parser::_try_rule_enum() {
      static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: IMPLEMENT ME");
      return false;
   }
   bool parser::_try_rule_block() {
      static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: IMPLEMENT ME");
      return false;
   }
   bool parser::_try_rule_block_actions() {
      static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: IMPLEMENT ME");
      return false;
   }
   bool parser::_try_rule_block_event() {
      static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: IMPLEMENT ME");
      static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: return flags-mask of found event types instead of success bool? what about syntax errors e.g. on : do end ?");
      return false;
   }
   bool parser::_try_rule_block_conditions() {
      static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: IMPLEMENT ME");
      return false;
   }
   bool parser::_try_rule_pragma() {
      static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: IMPLEMENT ME");
      return false;
   }

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
                     //
                     // Unable to parse argument. Log an error, and continue to the next ',' or ')'.
                     //
                     auto* error = new errors::function_call_argument_parse_error;
                     error->pos = this->_previous_token()->start;
                     this->errors.push_back(error);
                     //
                     // Skip to the next argument, or exit the argument list if this looks like it was 
                     // the last argument.
                     //
                     auto   i     = this->next_token;
                     auto&  list  = this->scanner.tokens;
                     size_t paren = 1;
                     for (; i < list.size(); ++i) {
                        const auto& t = list[i];
                        if (t.type == token_type::comma && paren == 1) {
                           //
                           // Comma indicating next argument.
                           //
                           this->next_token = i;
                           continue;
                        }
                        if (t.type == token_type::paren_l) {
                           ++paren;
                        } else if (t.type == token_type::paren_r) {
                           --paren;
                           if (paren == 0) {
                              //
                              // End of argument list.
                              //
                              this->next_token = i;
                              break;
                           }
                        }
                     }
                     //
                     // Unterminated argument list?
                     //
                     continue;
                  }
                  auto& argument = args.emplace_back();
                  if (argument_is_named)
                     std::swap(argument.name, argument_name);
                  std::swap(argument.value, expr);
               } while (!this->_consume_token_if_present<token_type::comma>());
            }
            if (!this->_consume_token_if_present<token_type::paren_r>()) {
               errors::unterminated_function_call_argument_list error;
               error.pos = this->_previous_token()->start;
               throw errors::parse_exception(error);
            }
            //
            auto* expr = expression::alloc_call(lit);
            std::swap(expr->arguments, args);
            return expr;
         }
         // just an identifier
         return expression::alloc_literal(lit);
      }
      if (this->_consume_any_token_of_types<token_type::paren_l>()) {
         const auto start_at = this->_previous_token()->start;
         //
         auto* content = this->_try_rule_expression();
         if (!this->_consume_token_if_present<token_type::paren_r>()) {
            errors::unterminated_grouping_expression error;
            error.pos       = this->_previous_token()->start;
            error.opened_at = start_at;
            throw errors::parse_exception(error);
         }
         return expression::alloc_grouping(content);
      }
      return nullptr;
   }

}