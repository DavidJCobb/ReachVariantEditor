#pragma once
#include <optional>
#include <QString>
#include "literal_number.h"
#include "operator_metadata.h"
#include "scanner.h"

namespace halo::reach::megalo::AST {
   class expression;

   class parser {
      public:

      protected:
         scanner scanner;
         size_t  next_token = 0;

         bool is_at_end() const;

         bool _check_next_token(token_type) const; // peek and check type
         const token& _peek_next_token() const;
         const token& _pull_next_token();
         const token* _previous_token() const;

         template<token_type... TokenTypes>
         bool _consume_any_token_of_types();

         template<size_t Size, std::array<token_type, Size> TokenTypes>
         bool _consume_any_token_of_types();

         void _require_and_consume_token(token_type, const char* error_message_translation_key);

         void _try_rule_statement();
            bool _try_rule_keyword();
               bool _try_rule_alias();
               bool _try_rule_declare();
               bool _try_rule_enum();
               bool _try_rule_block();
                  bool _try_rule_block_actions();
                     bool _try_rule_block_event();
                  bool _try_rule_block_conditions();
            expression* _try_rule_expression();
               template<size_t TierIndex> expression* _try_rule_expression_binary();
               expression* _try_rule_expression_unary();
               expression* _try_rule_expression_primary();
                  expression* _try_rule_expression_call_arguments();

   };
}

#include "./parser.inl"