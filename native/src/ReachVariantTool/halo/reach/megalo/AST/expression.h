#pragma once
#include <vector>
#include <QString>
#include "helpers/owned_ptr.h"
#include "./_base.h"
#include "./literal.h"
#include "./token_type.h"

namespace halo::reach::megalo::AST {
   enum class expression_type {
      none,
      unary,
      binary,
      call,
      primary,
      literal,
   };

   struct expression;
   struct function_call_argument : public item_base {
      QString     name; // if it's using named parameters; else, blank
      expression* value = nullptr;
   };

   struct expression : public item_base {
      expression_type type = expression_type::none;
      token_type      op   = token_type::none;

      literal_item lit; // literals only.
      cobb::owned_ptr<expression> lhs; // binary or call  only. binary: LHS; call: function to call
      cobb::owned_ptr<expression> rhs; // binary or unary only.
      std::vector<cobb::owned_ptr<function_call_argument>> arguments;

      static expression* alloc_binary(expression* lhs, token_type op, expression* rhs);
      static expression* alloc_call(expression* callee);
      static expression* alloc_grouping(expression* content); // for parentheticals in expressions
      static expression* alloc_literal(literal_base* content);
      static expression* alloc_unary(token_type op, expression* subject);
   };
};