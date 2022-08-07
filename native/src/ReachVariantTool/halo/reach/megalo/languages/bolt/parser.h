#pragma once
#include <optional>
#include <vector>
#include <QString>
#include "./errors/base.h"
#include "./operator_metadata.h"
#include "./scanner.h"

namespace halo::reach::megalo::bolt {
   class expression;

   class parser {
      public:
         ~parser();

      public:
         std::vector<errors::base*> errors;
         
      protected:
         scanner scanner;
         size_t  next_token = 0;

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

         template<size_t Size, std::array<token_type, Size> TokenTypes>
         bool _consume_any_token_of_types();

         void _try_rule_statement();
            bool _try_rule_keyword();
               bool _try_rule_alias();
               bool _try_rule_declare();
               bool _try_rule_enum();
               bool _try_rule_block();
                  bool _try_rule_block_actions();
                     bool _try_rule_block_event();
                  bool _try_rule_block_conditions();
               bool _try_rule_pragma();
            expression* _try_rule_expression();
               template<size_t TierIndex> expression* _try_rule_expression_binary();
               expression* _try_rule_expression_unary();
               expression* _try_rule_expression_primary();
                  expression* _try_rule_expression_call_arguments();

   };
}

#include "./parser.inl"