#pragma once
#include <vector>
#include <QString>
#include "helpers/owned_ptr.h"
#include "./_base.h"

namespace halo::reach::megalo::AST {
   enum class expression_type {
      none,
      unary,
      binary,
      call,
      primary,
   };
   enum class expression_op {
      none,
      compare_eq,
      compare_ne,
      compare_l,
      compare_le,
      compare_g,
      compare_ge,
      binary_add,
      binary_sub,
      binary_mul,
      binary_div,
      binary_mod,
      binary_shl,
      binary_shr,
      unary_positive,
      unary_negate,
      unary_not,
   };

   struct expression;
   struct function_call_argument : public item_base {
      QString     name; // if it's using named parameters; else, blank
      expression* value = nullptr;
   };

   struct expression : public item_base {
      expression_type type = expression_type::none;
      expression_op   op   = expression_op::none;

      cobb::owned_ptr<expression> lhs; // binary or call  only. binary: LHS; call: function to call
      cobb::owned_ptr<expression> rhs; // binary or unary only.
      std::vector<cobb::owned_ptr<function_call_argument>> arguments;

      static expression* alloc_binary(expression* lhs, expression_op, expression* rhs);
      static expression* alloc_call(expression* callee);
      static expression* alloc_unary(expression_op, expression* subject);
   };
};