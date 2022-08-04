#pragma once
#include <vector>
#include <QString>
#include "helpers/owned_ptr.h"
#include "./_base.h"
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
   };

   struct expression;
   struct function_call_argument : public item_base {
      QString name; // if it's using named parameters; else, blank
      cobb::owned_ptr<expression> value = nullptr;

      function_call_argument() {}
      function_call_argument(function_call_argument&&) noexcept;
      function_call_argument& operator=(function_call_argument&&);

      function_call_argument(const function_call_argument&) = delete;
      function_call_argument& operator=(const function_call_argument&) = delete;
   };

   struct expression : public item_base, public block_child {
      expression_type type = expression_type::none;
      token_type      op   = token_type::none;

      literal_item lit; // calls and literals only. call: function to call (identifier)
      cobb::owned_ptr<expression> lhs; // binary only.
      cobb::owned_ptr<expression> rhs; // binary or unary only.
      std::vector<function_call_argument> arguments;

      static expression* alloc_binary(expression* lhs, token_type op, expression* rhs);
      static expression* alloc_call(const literal_item& callee);
      static expression* alloc_grouping(expression* content); // for parentheticals in expressions
      static expression* alloc_literal(const literal_item& content);
      static expression* alloc_unary(token_type op, expression* subject);
   };
};