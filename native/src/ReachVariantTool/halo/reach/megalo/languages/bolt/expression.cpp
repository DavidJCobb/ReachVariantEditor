#include "expression.h"

namespace halo::reach::megalo::bolt {
   /*static*/ expression* expression::alloc_binary(expression* lhs, token_type op, expression* rhs) {
      auto* out = new expression;
      out->type = expression_type::binary;
      out->lhs  = lhs;
      out->op   = op;
      out->rhs  = rhs;
      return out;
   }
   /*static*/ expression* expression::alloc_call(const literal_item& callee) {
      auto* out = new expression;
      out->type = expression_type::call;
      out->lit  = callee;
      return out;
   }
   /*static*/ expression* expression::alloc_grouping(expression* content) {
      auto* out = new expression;
      out->type = expression_type::grouping;
      out->lhs  = content;
      return out;
   }
   /*static*/ expression* expression::alloc_literal(const literal_item& content) {
      auto* out = new expression;
      out->type = expression_type::literal;
      out->lit  = content;
      return out;
   }
   /*static*/ expression* expression::alloc_unary(token_type op, expression* subject) {
      auto* out = new expression;
      out->type = expression_type::unary;
      out->op   = op;
      out->rhs  = subject;
      return out;
   }
}