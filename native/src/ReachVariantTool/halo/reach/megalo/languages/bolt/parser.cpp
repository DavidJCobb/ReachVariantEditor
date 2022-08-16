#include "parser.h"
#include "helpers/lambda_guard.h"
#include "./errors/base_exception.h"
#include "./errors/block_event_names_not_found.h"
#include "./errors/block_event_not_followed_by_block.h"
#include "./errors/function_call_argument_parse_error.h"
#include "./errors/token_matches_no_grammar_rule.h"
#include "./errors/unknown_block_event_name.h"
#include "./errors/unterminated_block.h"
#include "./errors/unterminated_block_event_list.h"
#include "./errors/unterminated_condition_list.h"
#include "./errors/unterminated_function_call_argument_list.h"
#include "./errors/unterminated_grouping_expression.h"
#include "./action_block.h"
#include "./condition_block.h"
#include "./event_type.h"
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

   bool parser::_consume_word_if_present(const char* word, Qt::CaseSensitivity cs) {
      const auto& next = this->_peek_next_token();
      if (next.type != token_type::identifier_or_word)
         return false;
      //
      assert(std::holds_alternative<literal_data_identifier_or_word>(next.literal.value));
      const auto& value = std::get<literal_data_identifier_or_word>(next.literal.value).content;
      if (value.compare(word, cs) == 0)
         return true;
      return false;
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
               errors::block_event_not_followed_by_block error;
               error.pos = this->_previous_token()->start;
               throw errors::parse_exception(error);
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
      constexpr auto event_list_separator  = token_type::comma;
      constexpr auto event_list_terminator = token_type::colon;

      if (!this->_consume_token_if_present<token_type::keyword_on>())
         return {};
      token_pos list_start = this->_previous_token()->start;

      constexpr auto all_events = []() {
         block::events_mask_type mask = 0;
         for (size_t i = 0; i < event_type_phrases.size(); ++i)
            mask |= 1 << i;
         return mask;
      }();
      
      block::events_mask_type events = 0;
      bool any = false;

      do {
         auto   viable_events = all_events;
         size_t word_count    = 0;

         token_pos name_start;
         while (this->_consume_token_if_present<token_type::identifier_or_word>()) {
            any = true;
            ++word_count;
            if (word_count == 1) {
               name_start = this->_previous_token()->start;
            }

            QString word;
            {
               auto* t = this->_previous_token();
               assert(t->type == token_type::identifier_or_word);
               assert(std::holds_alternative<literal_data_identifier_or_word>(t->literal.value));
               word = std::get<literal_data_identifier_or_word>(t->literal.value).content;
            }
            word = word.toLower();

            for (size_t i = 0; i < event_type_phrases.size(); ++i) {
               auto mask = block::events_mask_type{1} << i;
               if ((viable_events & mask) == 0)
                  continue;

               const auto& phrase = event_type_phrases[i];
               if (word_count > phrase.size()) {
                  viable_events &= ~mask;
                  continue;
               }
               if (phrase[word_count - 1] != word) {
                  viable_events &= ~mask;
                  continue;
               }
            }
         }

         block::events_mask_type target = 0;
         if (viable_events) {
            for (size_t i = 0; i < event_type_phrases.size(); ++i) {
               auto mask = block::events_mask_type{ 1 } << i;
               if ((viable_events & mask) == 0)
                  continue;

               const auto& phrase = event_type_phrases[i];
               if (phrase.size() == word_count) {
                  target = mask;
                  break;
               }
            }
         }
         if (target == 0) {
            auto* error = new errors::unknown_block_event_name;
            error->pos = name_start;
            for (size_t i = 0; i < word_count; ++i) {
               const auto& t = this->scanner.tokens[this->next_token - i];
               assert(t.type == token_type::identifier_or_word);
               assert(std::holds_alternative<literal_data_identifier_or_word>(t.literal.value));

               const auto& word = std::get<literal_data_identifier_or_word>(t.literal.value).content;
               if (i > 0) {
                  error->name += QChar(' ');
               }
               error->name += word;
            }
            this->errors.push_back(error);
            continue;
         }
         events |= target;

      } while (!this->_consume_token_if_present<event_list_separator>());

      if (!any) {
         auto* error = new errors::block_event_names_not_found;
         error->pos = list_start;
         this->errors.push_back(error);
      }
      if (!this->_consume_token_if_present<event_list_terminator>()) {
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
      action_block* result      = nullptr;
      const auto&   block_start = this->_previous_token()->start;
      switch (this->_previous_token()->type) {
         case token_type::keyword_do:
            result = new action_block;
            result->type = action_block::block_type::bare;
            break;
         case token_type::keyword_for:
            if (!this->_consume_word_if_present("each")) {
               static_assert(false, "TODO: THROW ERROR");
            }
            if (this->_consume_word_if_present("object")) {
               if (this->_consume_word_if_present("of")) { // for each object of type <object-type>
                  if (!this->_consume_word_if_present("type")) {
                     static_assert(false, "TODO: THROW ERROR");
                  }
                  static_assert(false, "TODO: IMPLEMENT ME");
               } else if (this->_consume_word_if_present("with")) { // for each object with label <forge-label>
                  if (!this->_consume_word_if_present("label")) {
                     static_assert(false, "TODO: THROW ERROR");
                  }
                  static_assert(false, "TODO: IMPLEMENT ME");
               } else { // for each object
                  static_assert(false, "TODO: IMPLEMENT ME");
               }
            } else if (this->_consume_word_if_present("player")) {
               if (this->_consume_word_if_present("randomly")) {
                  static_assert(false, "TODO: IMPLEMENT ME");
               }
               static_assert(false, "TODO: IMPLEMENT ME");
            } else if (this->_consume_word_if_present("team")) {
               static_assert(false, "TODO: IMPLEMENT ME");
            }
            static_assert(false, "TODO: IMPLEMENT ME");
            break;
         case token_type::keyword_function:
            static_assert(false, "TODO: IMPLEMENT ME");
            break;
      }
      assert(result);
      result->start = block_start;
      this->current_block->append(*result);
      this->current_block = result;
      //
      this->_try_rule_block_body();
      if (!this->_consume_token_if_present<token_type::keyword_end>()) {
         errors::unterminated_block error;
         error.pos     = this->_previous_token()->start;
         error.subject = result;
         throw errors::parse_exception(error);
      }
      result->end = this->_previous_token()->end;
      this->current_block = this->current_block->parent;
      return result;
   }
   block* parser::_try_rule_block_conditions() {
      if (!this->_consume_any_token_of_types<token_type::keyword_if>())
         return false;
      
      condition_block* result = new condition_block;
      result->start = this->_previous_token()->start;
      result->type  = condition_block::block_type::if_block;
      this->current_block->append(*result);
      this->current_block = result;
      
      condition_block* last_opened_block = result; // Used to properly set the end-position for each if/altif/alt branch.

      auto _extract_keyword_then = [this, &last_opened_block]() {
         if (!this->_consume_token_if_present<token_type::keyword_then>()) {
            errors::unterminated_condition_list error;
            error.pos     = this->_previous_token()->start;
            error.subject = last_opened_block;
            throw errors::parse_exception(error);
         }
      };
      auto _open_next_branch = [this, &last_opened_block](condition_block::block_type b) -> condition_block* {
         last_opened_block->end = this->_previous_token()->end;
         last_opened_block = new condition_block;
         last_opened_block->start = this->_previous_token()->start;
         last_opened_block->type  = b;
         this->current_block = this->current_block->parent;
         this->current_block->append(*last_opened_block);
         this->current_block = last_opened_block;
         return last_opened_block;
      };
      
      static_assert(false, "TODO: IMPLEMENT ME (PULL CONDITION LIST)");
      _extract_keyword_then();
      //
      // We've extracted the conditions. Now, let's pull the contents.
      //
      this->_try_rule_block_body();
      //
      // Now we need to handle altif and alt branches (akin to elseif and else branches 
      // in other languages). Note that the result we want to return from this function 
      // is the first block only (the initial "if" branch), so we can't overwrite the 
      // `result` variable.
      //
      while (this->_consume_token_if_present<token_type::keyword_altif>()) {
         auto* branch = _open_next_branch(condition_block::block_type::altif_block);
         //
         // Next, we need to extract the condition list for this altif branch.
         //
         this->_try_rule_block_condition_list();
         _extract_keyword_then();
         //
         // Finally, we can extract the altif branch's content.
         //
         this->_try_rule_block_body();
      }
      if (this->_consume_token_if_present<token_type::keyword_alt>()) {
         auto* branch = _open_next_branch(condition_block::block_type::alt_block);
         this->_try_rule_block_body();
      }
      //
      // And finally, it's time to close the if-tree.
      //
      if (!this->_consume_token_if_present<token_type::keyword_end>()) {
         errors::unterminated_block error;
         error.pos     = this->_previous_token()->start;
         error.subject = last_opened_block;
         throw errors::parse_exception(error);
      }
      last_opened_block->end = this->_previous_token()->end;
      this->current_block = this->current_block->parent;
      return result;
   }
   void parser::_try_rule_block_condition_list() {
      static_assert(false, "TODO: IMPLEMENT ME");
      if (!this->_consume_token_if_present<token_type::keyword_then>()) {
         errors::unterminated_condition_list error;
         error.pos = this->_previous_token()->start;
         throw errors::parse_exception(error);
      }
   }
   void parser::_try_rule_block_body() {
      while (this->_try_rule_statement()) {
         ; // Don't need to do anything but loop _try_rule_statement, right now.
      }
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