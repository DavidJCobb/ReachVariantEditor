#include "parser.h"
#include "helpers/lambda_guard.h"
#include "./config/constant_expression_evaluation_implemented.h"
#include "./config/reference_expression_evaluation_implemented.h"
#include "./errors/base_exception.h"
#include "./errors/block_event_names_not_found.h"
#include "./errors/block_event_not_followed_by_block.h"
#include "./errors/function_call_argument_parse_error.h"
#include "./errors/identifier_with_member_access_not_allowed_here.h"
#include "./errors/missing_syntax_element.h"
#include "./errors/pragma_compiler_language_not_bolt.h"
#include "./errors/pragma_compiler_language_inconsistent.h"
#include "./errors/pragma_requires_data.h"
#include "./errors/token_matches_no_grammar_rule.h"
#include "./errors/unknown_block_event_name.h"
#include "./errors/unknown_for_loop_type.h"
#include "./errors/unknown_variable_network_priority.h"
#include "./errors/unterminated_block_event_list.h"
#include "./errors/unterminated_grouping_expression.h"
#include "./action_block.h"
#include "./alias.h"
#include "./condition_block.h"
#include "./declaration.h"
#include "./event_type.h"
#include "./expression.h"
#include "./pragma_type.h"
#include "./user_defined_enum.h"

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
      this->scanner.scan_tokens();
      static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: Running the scanner multiple times will add multiple EOF tokens; running the parser multiple times will therefore do the same.");
      static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: It'd be good to allow incremental (re)scanning and (re)parsing; could explore using the AST gen for a syntax highlighter then.");

      this->root = new action_block;
      this->current_block = this->root.get();

      bool stream_is_complete = false;
      while (!this->is_at_end()) {
         if (this->_try_rule_statement())
            continue;
         if (this->_consume_token_if_present<token_type::eof>()) {
            stream_is_complete = true;
            break;
         }
         errors::token_matches_no_grammar_rule error;
         error.fault = this->_peek_next_token();
         error.pos   = error.fault.start;
         throw errors::parse_exception(error);
      }
      #if _DEBUG
         __debugbreak();
         assert(this->root == this->current_block); // An unclosed block should've triggered a thrown exception.
         static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: IS THERE ANYTHING ELSE WE NEED TO DO?");
      #endif
   }

   void parser::run_debug_test() {
      this->parse();
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
      if (next.type != token_type::word) {
         return false;
      }
      if (next.word.compare(word, cs) == 0) {
         ++this->next_token;
         return true;
      }
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
      if (!this->_consume_token_if_present<token_type::keyword_alias>()) {
         return false;
      }

      bool      valid_name = true;
      QString   name;
      token_pos name_start;
      token_pos name_end;

      cobb::owned_ptr<identifier> name_ident = this->_try_rule_identifier();
      if (!name_ident) {
         valid_name = false;
      } else {
         if (name_ident->has_member_access()) {
            valid_name = false;
            //
            auto* error = new errors::identifier_with_member_access_not_allowed_here(syntax_element::user_defined_function_definition_name);
            error->pos = name_ident->start;
            this->errors.push_back(error);
         } else {
            assert(name_ident->parts.size());
            auto& part = name_ident->parts[0];
            name       = part.name;
            name_start = part.start;
            name_end   = part.end;
         }
      }

      if (!this->_consume_token_if_present<token_type::equal>()) [[unlikely]] {
         auto error = errors::missing_syntax_element(syntax_element::alias_equal_sign);
         error.pos         = this->_peek_next_token().start;
         error.fault_token = this->_peek_next_token();
         throw errors::parse_exception(error);
      }

      auto* value = this->_try_rule_expression();
      if (!value) {
         auto error = errors::missing_syntax_element(syntax_element::alias_value);
         error.pos         = this->_peek_next_token().start;
         error.fault_token = this->_peek_next_token();
         throw errors::parse_exception(error);
      }
      static_assert(config::code_needs_update_for_constant_expression_evaluation_implemented, "TODO: For constant expressions, compute the integer result and store it directly.");
      static_assert(config::code_needs_update_for_reference_expression_evaluation_implemented,
         "TODO: Validate expressions: they must produce a single value or a reference to a single variable. "
         "This in turn requires that we implement the ability to compute an expression expecting it to return a reference."
      );

      if (!valid_name) {
         return true;
      }

      auto* new_alias = new alias;
      new_alias->name     = name;
      new_alias->name_pos = {
         .start = name_start,
         .end   = name_end,
      };
      new_alias->value    = value;

      this->current_block->append(*new_alias);

      return true;
   }
   bool parser::_try_rule_declare() {
      if (!this->_consume_token_if_present<token_type::keyword_declare>()) {
         return false;
      }

      declaration* decl = nullptr;
      {
         cobb::owned_ptr<identifier> declared_var = this->_try_rule_identifier();
         if (!declared_var) [[unlikely]] {
            auto error = errors::missing_syntax_element(syntax_element::variable_declaration_variable);
            error.pos = this->_peek_next_token().start;
            error.fault_token = this->_peek_next_token();
            throw errors::parse_exception(error);
         }
         //
         decl = new declaration;
         decl->ident = std::move(declared_var);
         this->variable_declarations.push_back(decl);
      }

      if (this->_consume_phrase_if_present("with", "network", "priority")) {
         if (!this->_consume_token_if_present<token_type::word>()) {
            auto error = errors::missing_syntax_element(syntax_element::variable_declaration_network_priority);
            error.pos         = this->_peek_next_token().start;
            error.fault_token = this->_peek_next_token();
            throw errors::parse_exception(error);
         }

         const auto& pri = this->_previous_token()->word;
         if (pri.compare("none", Qt::CaseInsensitive) == 0) {
            decl->network_priority = variable_network_priority::none;
         } else if (pri.compare("low", Qt::CaseInsensitive) == 0) {
            decl->network_priority = variable_network_priority::low;
         } else if (pri.compare("high", Qt::CaseInsensitive) == 0) {
            decl->network_priority = variable_network_priority::high;
         } else {
            auto* error = new errors::unknown_variable_network_priority;
            error->pos      = this->_previous_token()->start;
            error->subject  = decl;
            error->priority = pri;
            this->errors.push_back(error);
         }
      }

      if (this->_consume_token_if_present<token_type::equal>()) {

         static_assert(config::code_needs_update_for_constant_expression_evaluation_implemented, "TODO: Compute the result of a constant expression, and store that directly, instead of storing the expression");
         //
         decl->initial_value = this->_try_rule_expression();
         if (!decl->initial_value) {
            auto error = errors::missing_syntax_element(syntax_element::variable_declaration_default_value);
            error.pos         = this->_peek_next_token().start;
            error.fault_token = this->_peek_next_token();
            throw errors::parse_exception(error);
         }
      }

      return true;
   }
   bool parser::_try_rule_enum() {
      if (!this->_consume_token_if_present<token_type::keyword_enum>()) {
         return false;
      }

      bool      valid_name = true;
      QString   name;
      token_pos name_start;
      token_pos name_end;

      cobb::owned_ptr<identifier> name_ident = this->_try_rule_identifier();
      if (!name_ident) [[unlikely]] {
         auto error = errors::missing_syntax_element(syntax_element::user_defined_enum_name);
         error.pos         = this->_peek_next_token().start;
         error.fault_token = this->_peek_next_token();
         throw errors::parse_exception(error);
      } else {
         if (name_ident->has_member_access()) {
            valid_name = false;
            //
            auto* error = new errors::identifier_with_member_access_not_allowed_here(syntax_element::user_defined_enum_name);
            error->pos = name_ident->start;
            this->errors.push_back(error);
         } else {
            assert(name_ident->parts.size());
            auto& part = name_ident->parts[0];
            name       = part.name;
            name_start = part.start;
            name_end   = part.end;
         }
      }

      auto* ude = new user_defined_enum;
      ude->name     = name;
      ude->name_pos = {
         .start = name_start,
         .end   = name_end,
      };
      this->current_block->append(*ude);

      cobb::owned_ptr<identifier> member_name_ident;
      while (member_name_ident = this->_try_rule_identifier()) {
         bool discard = member_name_ident->has_member_access();
         if (discard) {
            auto* error = new errors::identifier_with_member_access_not_allowed_here(syntax_element::user_defined_enum_member_value);
            error->pos = member_name_ident->start;
            this->errors.push_back(error);
         }
         //
         auto& member = ude->members.emplace_back();
         member.name = member_name_ident->parts[0].name;
         if (this->_consume_token_if_present<token_type::equal>()) {
            auto* value = this->_try_rule_expression();
            if (!name_ident) {
               auto error = errors::missing_syntax_element(syntax_element::user_defined_enum_member_value);
               error.pos         = this->_peek_next_token().start;
               error.fault_token = this->_peek_next_token();
               throw errors::parse_exception(error);
            }
            static_assert(config::code_needs_update_for_constant_expression_evaluation_implemented, "TODO: Compute result of constant expression; store result directly as constant value");
            member.value = value;
         } else {
            static_assert(config::code_needs_update_for_constant_expression_evaluation_implemented, "TODO: Set constant value to previous value plus one, if not first element; else, set value to 0");
         }
         //
         if (discard) {
            //
            // Enum member had an invalid name. Pop it.
            //
            ude->members.pop_back();
            static_assert(just_let_me_compile_it_for_now_so_i_can_test, "TODO: Can we re-tool the code above to just never create the member if the name was bad?");
         }
      }

      if (!this->_consume_token_if_present<token_type::keyword_end>()) {
         auto error = errors::missing_syntax_element(syntax_element::user_defined_enum_end);
         error.pos         = this->_peek_next_token().start;
         error.fault_token = this->_peek_next_token();
         throw errors::parse_exception(error);
      }

      return true;
   }
   bool parser::_try_rule_block() {
      #if _DEBUG
         //
         // The grammar is such that the functions which handle a block should handle both 
         // opening it and closing it; a block opened while this function runs should not 
         // still be open by the time this function exits. Here, we assert that this is true.
         // 
         // We only assert if no (more) exceptions have been thrown, to ensure that we don't 
         // fail the assertion if a fatal error occurs while parsing the block or its contents.
         //
         auto* current_block_prior = this->current_block;
         auto  guard = cobb::lambda_guard_on_success([current_block_prior, this]() {
            assert(this->current_block == current_block_prior);
         });
      #endif
      auto  events = this->_try_rule_block_event();
      auto* block  = this->_try_rule_block_conditions();
      if (!block) {
         block = this->_try_rule_block_actions();
         if (!block) {
            if (events.has_value()) [[unlikely]] {
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
         while (this->_consume_token_if_present<token_type::word>()) {
            any = true;
            ++word_count;
            if (word_count == 1) {
               name_start = this->_previous_token()->start;
            }

            QString word = this->_previous_token()->word.toLower();

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
               assert(t.type == token_type::word);
               const auto& word = t.word;
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
         return nullptr;
      
      action_block* result = new action_block;
      result->start = this->_previous_token()->start;
      this->current_block->append(*result);

      switch (this->_previous_token()->type) {
         case token_type::keyword_do:
            result->type = action_block::block_type::bare;
            break;
         case token_type::keyword_for:
            {
               using bt = action_block::block_type;

               auto type = bt::bare;

               std::array<QString, 4> words;
               size_t count;

               this->_extract_and_consume_phrase(words, count);
               if (count >= 2 && count <= 4) {
                  if (words[0] == "each") {
                     if (words[1] == "object") {
                        if (count == 2)
                           type = bt::for_each_object;
                        else if (count >= 4) {
                           if (words[2] == "with" && words[3] == "label")
                              type = bt::for_each_object_with_label;
                           else if (words[2] == "of" && words[3] == "type")
                              type = bt::for_each_object_of_type;
                        }
                     } else if (words[1] == "player") {
                        if (count == 2)
                           type = bt::for_each_player;
                        else if (count == 3) {
                           if (words[2] == "randomly")
                              type = bt::for_each_player_randomly;
                        }
                     } else if (words[1] == "team") {
                        if (count == 2)
                           type = bt::for_each_team;
                     }
                  }
               }

               result->type = type;
               switch (type) {
                  [[unlikely]] case bt::bare:
                     {
                        constexpr size_t max_tokens_to_skip = 10;

                        size_t skipped = 0;
                        for (; skipped < max_tokens_to_skip; ++skipped) {
                           if (auto* ident = this->_try_rule_identifier()) {
                              delete ident;
                              continue;
                           }
                           if (!this->_consume_any_token_of_types<token_type::number, token_type::string>()) {
                              break;
                           }
                        }
                        if (skipped >= max_tokens_to_skip) {
                           errors::unknown_for_loop_type error;
                           error.pos     = result->start;
                           error.subject = result;
                           throw errors::parse_exception(error);
                        } else {
                           auto* error = new errors::unknown_for_loop_type;
                           error->pos     = result->start;
                           error->subject = result;
                           this->errors.push_back(error);
                        }
                     }
                     break;
                  case bt::for_each_object_with_label:
                     if (auto* ident = this->_try_rule_identifier()) {
                        result->forge_label = ident;
                     } else if (this->_consume_any_token_of_types<token_type::number, token_type::string>()) {
                        auto* t = this->_previous_token();
                        result->forge_label = t->literal;
                     } else {
                        auto* error = new errors::missing_syntax_element(syntax_element::for_loop_forge_label);
                        error->pos         = result->start;
                        error->fault_token = *this->_previous_token();
                        error->subject     = result;
                        this->errors.push_back(error);
                     }
                     break;
                  case bt::for_each_object_of_type:
                     if (auto* ident = this->_try_rule_identifier()) {
                        result->object_type = ident;
                     } else if (this->_consume_token_if_present<token_type::number>()) {
                        auto* t = this->_previous_token();
                        result->object_type = t->literal;
                     } else {
                        auto* error = new errors::missing_syntax_element(syntax_element::for_loop_object_type);
                        error->pos         = result->start;
                        error->fault_token = *this->_previous_token();
                        error->subject     = result;
                        this->errors.push_back(error);
                     }
                     break;
               }
               if (!this->_consume_token_if_present<token_type::keyword_do>()) {
                  auto error = errors::missing_syntax_element(syntax_element::for_loop_do);
                  error.pos         = result->start;
                  error.fault_token = *this->_previous_token();
                  error.subject     = result;
                  throw errors::parse_exception(error);
               }
            }
            break;
         case token_type::keyword_function:
            {
               result->type = action_block::block_type::function;

               {
                  identifier* ident = this->_try_rule_identifier();
                  if (!ident) [[unlikely]] {
                     auto* error = new errors::missing_syntax_element(syntax_element::user_defined_function_definition_name);
                     error->pos         = this->_peek_next_token().start;
                     error->fault_token = *this->_previous_token();
                     error->subject     = result;
                     this->errors.push_back(error);
                  } else {
                     if (ident->has_member_access()) [[unlikely]] {
                        auto* error = new errors::identifier_with_member_access_not_allowed_here(syntax_element::user_defined_function_definition_name, *ident);
                        this->errors.push_back(error);
                     }
                  }
                  result->function_name = ident;
               }

               if (!this->_consume_token_if_present<token_type::paren_l>()) {
                  auto error = errors::missing_syntax_element(syntax_element::user_defined_function_definition_paren_l);
                  error.pos         = this->_peek_next_token().start;
                  error.fault_token = *this->_previous_token();
                  error.subject     = result;
                  throw errors::parse_exception(error);
               }
               //
               // TODO: In the future, we could consume any specified arguments. Bolt doesn't allow 
               // user-defined functions to have arguments,  but there's no reason that *must* be a 
               // fatal parse error.
               //
               if (!this->_consume_token_if_present<token_type::paren_r>()) {
                  auto error = errors::missing_syntax_element(syntax_element::user_defined_function_definition_paren_r);
                  error.pos         = this->_peek_next_token().start;
                  error.fault_token = *this->_previous_token();
                  error.subject     = result;
                  throw errors::parse_exception(error);
               }
            }
            break;
      }

      this->current_block = result;
      this->_try_rule_block_body();
      if (!this->_consume_token_if_present<token_type::keyword_end>()) {
         auto error = errors::missing_syntax_element(syntax_element::block_end);
         error.pos         = this->_peek_next_token().start;
         error.fault_token = *this->_previous_token();
         error.subject     = result;
         throw errors::parse_exception(error);
      }
      result->end = this->_previous_token()->end;
      this->current_block = this->current_block->parent;

      return result;
   }
   block* parser::_try_rule_block_conditions() {
      if (!this->_consume_any_token_of_types<token_type::keyword_if>())
         return nullptr;
      
      condition_block* result = new condition_block;
      result->start = this->_previous_token()->start;
      result->type  = condition_block::block_type::if_block;
      this->current_block->append(*result);
      this->current_block = result;
      
      condition_block* last_opened_block = result; // Used to properly set the end-position for each if/altif/alt branch.

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
      
      this->_try_rule_block_condition_list();
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
         auto error = errors::missing_syntax_element(syntax_element::block_end);
         error.fault_token = *this->_previous_token();
         error.subject     = result;
         throw errors::parse_exception(error);
      }
      last_opened_block->end = this->_previous_token()->end;
      this->current_block = this->current_block->parent;
      return result;
   }
   void parser::_try_rule_block_condition_list() {
      auto* current_branch = dynamic_cast<condition_block*>(this->current_block);
      assert(current_branch != nullptr);

      bool found_any = false;
      bool next_is_or = false;

      do {
         bool negate_current = false;
         while (this->_consume_token_if_present<token_type::keyword_not>()) {
            found_any |= true;
            negate_current = !negate_current;
         }
         auto* expr = this->_try_rule_expression();
         if (expr) {
            found_any |= true;
         }

         auto& cnd = current_branch->conditions.emplace_back();
         cnd.expr = expr;
         cnd.negate = negate_current;
         cnd.or_with_previous = next_is_or;

         if (!this->_consume_any_token_of_types<token_type::keyword_and, token_type::keyword_or>()) {
            if (!expr) {
               auto* error = new errors::missing_syntax_element(syntax_element::condition_list_item);
               error->fault_token = *this->_previous_token();
               error->subject     = current_branch;
               this->errors.push_back(error);
            }
            break;
         }
         if (!expr) {
            auto* error = new errors::missing_syntax_element(syntax_element::condition_list_item);
            error->fault_token = *this->_previous_token();
            error->subject     = current_branch;
            this->errors.push_back(error);
         }
         next_is_or = this->_previous_token()->type == token_type::keyword_or;
      } while (true);

      if (!this->_consume_token_if_present<token_type::keyword_then>()) {
         auto error = errors::missing_syntax_element(syntax_element::condition_list_terminator);
         error.fault_token = *this->_previous_token();
         error.subject     = current_branch;
         throw errors::parse_exception(error);
      }

      if (!found_any) {
         auto* error = new errors::missing_syntax_element(syntax_element::condition_list_item);
         error->fault_token = *this->_previous_token();
         error->subject     = current_branch;
         this->errors.push_back(error);
      }
   }
   void parser::_try_rule_block_body() {
      while (this->_try_rule_statement()) {
         ; // Don't need to do anything but loop _try_rule_statement, right now.
      }
   }

   bool parser::_try_rule_pragma() {
      if (!this->_consume_token_if_present<token_type::keyword_pragma>()) {
         return false;
      }
      token_pos start_pos = this->_previous_token()->start;

      pragma_type type = pragma_type::unspecified;
      QString     data;
      {
         QString name;
         if (!this->_consume_token_if_present<token_type::word>()) {
            errors::missing_syntax_element error{syntax_element::pragma_name};
            error.fault_token = this->_peek_next_token();
            throw errors::parse_exception(error);
         }
         name = this->_previous_token()->word;

         type = pragma_type::unrecognized;

         if (name.compare("compiler_language", Qt::CaseInsensitive) == 0) {
            type = pragma_type::compiler_language;
         }
      }

      if (this->_consume_token_if_present<token_type::string>()) {
         auto& lit = this->_previous_token()->literal;
         assert(std::holds_alternative<literal_data_string>(lit.value));
         data = std::get<literal_data_string>(lit.value).content;
      } else {
         if (pragma_type_requires_data(type)) {
            errors::pragma_requires_data error{ type };
            error.pos = start_pos;
            throw errors::parse_exception(error);
         }
      }

      {
         using namespace pragma_data;
         switch (type) {
            [[likely]] case pragma_type::unrecognized:
               break;
            case pragma_type::compiler_language:
               {
                  compiler_language value = compiler_language::unrecognized;

                  if (data.compare("bolt", Qt::CaseInsensitive) == 0) {
                     value = compiler_language::bolt;
                  }

                  auto& stored = this->pragma_data.language;
                  if (stored.has_value()) {
                     if (stored.value() != value) {
                        errors::pragma_compiler_language_inconsistent error;
                        error.prior = stored.value();
                        error.here = value;
                        error.pos = start_pos;
                        throw errors::parse_exception(error);
                     }
                  }
                  if (value == compiler_language::unrecognized) {
                     errors::pragma_compiler_language_not_bolt error;
                     error.specified = value;
                     error.pos = start_pos;
                     throw errors::parse_exception(error);
                  }
                  stored = value;
               }
               break;
         }
      }

      return true;
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
      if (auto* ident = this->_try_rule_identifier()) {
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
                  if (this->_check_token_type_sequence_ahead<token_type::word, token_type::equal>()) {
                     argument_is_named = true;
                     {  // Consume argument name.
                        auto& next = this->_pull_next_token();
                        assert(next.type == token_type::word);
                        argument_name = next.word;
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
               errors::missing_syntax_element error{syntax_element::function_call_argument_list_terminator};
               error.fault_token = this->_peek_next_token();
               throw errors::parse_exception(error);
            }
            //
            auto* expr = expression::alloc_call(ident);
            std::swap(expr->arguments, args);
            return expr;
         }
         // just an identifier
         return expression::alloc_literal(ident);
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

   identifier* parser::_try_rule_identifier() {
      if (!this->_consume_any_token_of_types<token_type::word>()) {
         return nullptr;
      }

      cobb::owned_ptr<identifier> result = new identifier; // owned_ptr so that we destroy it properly if exceptions are thrown

      auto _extract_part_index = [this, &result](identifier::part& part) -> bool {
         if (!this->_consume_token_if_present<token_type::square_bracket_l>()) {
            return false;
         }
         auto* expr = this->_try_rule_expression();
         if (!expr) {
            if (this->_consume_token_if_present<token_type::square_bracket_r>()) {
               auto* error = new errors::missing_syntax_element(syntax_element::identifier_part_index);
               error->fault_token = *this->_previous_token();
               error->subject     = result.get();
               this->errors.push_back(error);
            } else {
               auto error = errors::missing_syntax_element(syntax_element::identifier_part_index_end);
               error.fault_token = this->_peek_next_token();
               error.subject     = result.get();
               throw errors::parse_exception(error);
            }
         } else {
            part.index = expr;
            if (!this->_consume_token_if_present<token_type::square_bracket_r>()) {
               auto error = errors::missing_syntax_element(syntax_element::identifier_part_index_end);
               error.fault_token = this->_peek_next_token();
               error.subject     = result.get();
               throw errors::parse_exception(error);
            }
         }
         return true;
      };

      auto _extract_last_part = [this, &result, &_extract_part_index]() {
         const auto& prev = *this->_previous_token();
         auto&       part = result->parts.emplace_back();
         part.start = prev.start;
         part.end   = prev.end;
         part.name  = prev.word;
         //
         while (_extract_part_index(part)) {
            ;
         }
      };

      _extract_last_part();
      while (this->_consume_token_if_present<token_type::period>()) {
         if (!this->_consume_any_token_of_types<token_type::word>()) {
            auto* period_token = this->_previous_token();

            identifier::part dummy;
            
            bool has_parts = false;
            while (_extract_part_index(dummy)) {
               has_parts = true;
            }
            if (has_parts) {
               auto* error = new errors::missing_syntax_element(syntax_element::identifier_part_name);
               error->fault_token = *period_token;
               error->subject     = result.get();
               this->errors.push_back(error);
               //
               result->parts.push_back(std::move(dummy));
               continue;
            }

            auto error = errors::missing_syntax_element(syntax_element::identifier_part);
            error.fault_token = this->_peek_next_token();
            error.subject     = result.get();
            throw errors::parse_exception(error);
         }
         _extract_last_part();
      }

      return result.release();
   }

}