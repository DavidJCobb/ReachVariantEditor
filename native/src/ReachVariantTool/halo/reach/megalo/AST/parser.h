#pragma once
#include <optional>
#include <QString>
#include "literal_number.h"
#include "operator_metadata.h"

namespace halo::reach::megalo::AST {
   class expression;

   class parser {
      public:

      protected:

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
               template<const binary_operator_tier_definition& Tier> expression* _try_rule_expression_binary();
               expression* _try_rule_expression_unary();
               expression* _try_rule_expression_primary();
                  expression* _try_rule_expression_call_arguments();

   };
}

#include "./parser.inl"