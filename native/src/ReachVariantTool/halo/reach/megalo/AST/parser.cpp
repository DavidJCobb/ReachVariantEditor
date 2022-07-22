#include "parser.h"
#include "expression.h"

namespace halo::reach::megalo::AST {
   expression* parser::_try_rule_expression() {
      return this->_try_rule_expression_binary<binary_operator_tiers[0]>();
   }
}