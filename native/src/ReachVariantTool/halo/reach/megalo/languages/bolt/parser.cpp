#include "parser.h"
#include "helpers/lambda_guard.h"
#include "./errors/base_exception.h"
#include "./errors/function_call_argument_parse_error.h"
#include "./errors/token_matches_no_grammar_rule.h"
#include "./errors/unterminated_function_call_argument_list.h"
#include "./errors/unterminated_grouping_expression.h"
#include "./action_block.h"
#include "./condition_block.h"
#include "./expression.h"

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

   void parser::parse() {
      while (!this->is_at_end()) {
         if (!this->_try_rule_statement()) {
            errors::token_matches_no_grammar_rule error;
            error.fault = this->_peek_next_token();
            error.pos   = error.fault.start;
            throw errors::parse_exception(error);
         }
      }
      #if _DEBUG
         __debugbreak();
         static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: IS THERE ANYTHING ELSE WE NEED TO DO?");
      #endif
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

   bool parser::_try_rule_statement() {
      if (this->_try_rule_keyword())
         return true;
      if (auto* expr = this->_try_rule_expression()) {
         this->current_block->append(*expr);
         return true;
      }
      return false;
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
      #if _DEBUG
         //
         // The grammar is such that if the functions which handle a block should handle both 
         // opening it and closing it; a block opened while this function runs should not still 
         // be open by the time this function exits. Here, we assert that this is true.
         //
         auto* current_block_prior = this->current_block;
         auto  guard = cobb::lambda_guard([current_block_prior, this]() {
            assert(this->current_block == current_block_prior);
         });
      #endif
      auto  events = this->_try_rule_block_event();
      auto* block  = this->_try_rule_block_conditions();
      if (!block) {
         block = this->_try_rule_block_actions();
         if (!block) {
            if (events.has_value()) {
               static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: THROW FATAL ERROR: BLOCK EVENT NOT FOLLOWED BY BLOCK (unexpected token)");
            }
            return false;
         }
      }
      assert(block);
      if (events.has_value())
         block->events_mask = events.value();
      return true;
   }
   std::optional<block::events_mask_type> parser::_try_rule_block_event() {
      if (!this->_consume_token_if_present<token_type::keyword_on>())
         return {};
      //
      block::events_mask_type events = 0;
      bool any = false;
      while (this->_consume_token_if_present<token_type::identifier_or_word>()) {
         any = true;
         //
         auto* t = this->_previous_token();
         assert(t->type == token_type::identifier_or_word);
         assert(std::holds_alternative<literal_data_identifier_or_word>(t->literal.value));
         //
         auto& ident = std::get<literal_data_identifier_or_word>(t->literal.value);

         static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: FINISH ME");
      }
      if (!any) {
         static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: RAISE NON-FATAL ERROR: NO EVENT NAMES FOUND");
      }
      if (!this->_consume_token_if_present<token_type::colon>()) {
         errors::unterminated_block_event_list error;
         error.pos = this->_previous_token()->start;
         throw errors::parse_exception(error);
      }
      return events;
   }
   block* parser::_try_rule_block_actions() {
      if (!this->_consume_any_token_of_types<token_type::keyword_do, token_type::keyword_for, token_type::keyword_function>())
         return false;
      //
      action_block* result = nullptr;
      switch (this->_previous_token()->type) {
         case token_type::keyword_do:
            static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: IMPLEMENT ME");
            break;
         case token_type::keyword_for:
            static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: IMPLEMENT ME");
            break;
         case token_type::keyword_function:
            static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: IMPLEMENT ME");
            break;
      }
      assert(result);
      this->current_block->append(*result);
      this->current_block = result;
      //
      while (this->_try_rule_statement()) {
         ; // Don't need to do anything but loop _try_rule_statement, right now.
      }
      if (!this->_consume_token_if_present<token_type::keyword_end>()) {
         errors::unterminated_block error;
         error.pos = this->_previous_token()->start;
         throw errors::parse_exception(error);
      }
      this->current_block = this->current_block->parent;
      return result;
   }
   block* parser::_try_rule_block_conditions() {
      if (!this->_consume_any_token_of_types<token_type::keyword_if>())
         return false;
      //
      condition_block* result = new condition_block;
      this->current_block->append(*result);
      this->current_block = result;
      //
      static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: IMPLEMENT ME (PULL CONDITION LIST)");
      if (!this->_consume_token_if_present<token_type::keyword_then>()) {
         errors::unterminated_condition_list error;
         error.pos = this->_previous_token()->start;
         throw errors::parse_exception(error);
      }
      //
      // We've extracted the conditions. Now, let's pull the contents.
      //
      while (this->_try_rule_statement()) {
         ; // Don't need to do anything but loop _try_rule_statement, right now.
      }
      //
      // Now we need to handle altif and alt branches (akin to elseif and else branches 
      // in other languages). Note that the result we want to return from this function 
      // is the first block only (the initial "if" branch), so we can't overwrite the 
      // `result` variable.
      //
      while (this->_consume_token_if_present<token_type::keyword_altif>()) {
         condition_block* branch = new condition_block;
         this->current_block = this->current_block->parent;
         this->current_block->append(*branch);
         this->current_block = branch;
         //
         // Next, we need to extract the condition list for this altif branch.
         //
         static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: IMPLEMENT ME (PULL CONDITION LIST)");
         if (!this->_consume_token_if_present<token_type::keyword_then>()) {
            errors::unterminated_condition_list error;
            error.pos = this->_previous_token()->start;
            throw errors::parse_exception(error);
         }
         //
         // Having extracted the condition list, we now need to modify it: an altif 
         // branch should copy the conditions of all preceding (alt)if branches, and 
         // it should invert any conditions not already inverted (conditions that the 
         // previous altif branches have already copied will already be inverted).
         //
         static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: IMPLEMENT ME (MODIFY CONDITION LIST)");
         //
         // Finally, we can extract the altif branch's content.
         //
         while (this->_try_rule_statement()) {
            ; // Don't need to do anything but loop _try_rule_statement, right now.
         }
      }
      if (this->_consume_token_if_present<token_type::keyword_alt>()) {
         condition_block* branch = new condition_block;
         this->current_block = this->current_block->parent;
         this->current_block->append(*branch);
         this->current_block = branch;
         //
         // An alt branch specifies no conditions of its own, but still copies and 
         // inverts the conditions of previous branches via the same process as the 
         // altif branches.
         //
         static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: IMPLEMENT ME (MODIFY CONDITION LIST)");
         //
         // Now, we can extract the alt branch's content.
         //
         while (this->_try_rule_statement()) {
            ; // Don't need to do anything but loop _try_rule_statement, right now.
         }
      }
      //
      // And finally, it's time to close the if-tree.
      //
      if (!this->_consume_token_if_present<token_type::keyword_end>()) {
         errors::unterminated_block error;
         error.pos = this->_previous_token()->start;
         throw errors::parse_exception(error);
      }
      this->current_block = this->current_block->parent;
      return result;
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
               } while (this->_consume_token_if_present<token_type::comma>());
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