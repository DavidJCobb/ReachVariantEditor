#pragma once
#include <optional>
#include <vector>
#include <QString>
#include "helpers/type_traits/is_std_array_of_type.h"
#include "./errors/base.h"
#include "./util/phrase.h"
#include "./block.h"
#include "./operator_metadata.h"
#include "./scanner.h"

namespace halo::reach::megalo::bolt {
   class expression;

   class parser {
      public:
         parser(const QString& s) : scanner(s) {}
         ~parser();

      public:
         std::vector<errors::base*> errors;
         cobb::owned_ptr<block> root;

         void parse();

         void run_debug_test();
         
      protected:
         static constexpr size_t npos = size_t{ -1 };

         scanner scanner;
         size_t  next_token    = 0;
         block*  current_block = nullptr;

         bool is_at_end() const;

         bool _check_next_token(token_type) const; // peek and check type
         const token& _peek_next_token() const;
         const token& _pull_next_token();
         const token* _previous_token() const;

         template<token_type... TokenTypes>
         bool _check_token_type_sequence_ahead() const;

         template<token_type... TokenTypes>
         size_t _find_any_token_of_types() const;

         template<token_type Type>
         bool _consume_token_if_present();

         template<token_type... TokenTypes>
         bool _consume_any_token_of_types();

         template<auto TokenTypes> requires cobb::is_std_array_of_type<std::decay_t<decltype(TokenTypes)>, token_type>
         bool _consume_any_token_of_types();

         bool _consume_word_if_present(const char*, Qt::CaseSensitivity cs = Qt::CaseInsensitive);

         // Consumes up to `Size` following word/identifier tokens, and writes them to an input array. 
         // Returned words are forced to lowercase.
         template<size_t Size>
         void _extract_and_consume_phrase(std::array<QString, Size>& out_phrase, size_t& count_extracted);

         template<const auto& List> requires cobb::is_std_array_of_type<std::decay_t<decltype(List)>, util::phrase>
         size_t _extract_longest_matching_phrase_of();

         bool _try_rule_statement(); // acts on this->current_block
            bool _try_rule_keyword();
               bool _try_rule_alias();
               bool _try_rule_declare();
               bool _try_rule_enum();
               bool _try_rule_block();
                  std::optional<block::events_mask_type> _try_rule_block_event();
                  block* _try_rule_block_actions();
                  block* _try_rule_block_conditions();
                     void _try_rule_block_condition_list(); // acts on this->current_block
                  void _try_rule_block_body(); // acts on this->current_block
               bool _try_rule_pragma();
            expression* _try_rule_expression();
               template<size_t TierIndex> expression* _try_rule_expression_binary();
               expression* _try_rule_expression_unary();
               expression* _try_rule_expression_primary();
                  expression* _try_rule_expression_call_arguments();

   };
}

#include "./parser.inl"