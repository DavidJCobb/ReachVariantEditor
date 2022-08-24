#pragma once
#include <vector>
#include <QString>
#include "helpers/owned_ptr.h"
#include "./_base.h"
#include "./function_call_argument.h"
#include "./identifier.h"
#include "./literal.h"
#include "./token_type.h"

namespace halo::reach::megalo::bolt {
   enum class expression_type {
      none,
      unary,
      binary,
      call,
      primary,
      literal,
      grouping, // parenthetical expressions
   };

   struct expression : public item_base, public block_child {
      expression_type type = expression_type::none;
      token_type      op   = token_type::none;

      literal_item lit; // literals only.
      cobb::owned_ptr<identifier> ident; // calls and literals only. call: function to call
      cobb::owned_ptr<expression> lhs;   // binary or parenthetical only.
      cobb::owned_ptr<expression> rhs;   // binary or unary only.
      std::vector<function_call_argument> arguments;

      static expression* alloc_binary(expression* lhs, token_type op, expression* rhs);
      static expression* alloc_call(identifier* callee); // takes ownership
      static expression* alloc_grouping(expression* content); // for parentheticals in expressions
      static expression* alloc_literal(identifier*); // takes ownership
      static expression* alloc_literal(const literal_item& content);
      static expression* alloc_unary(token_type op, expression* subject);
   };
};