#pragma once
#include <QString>
#include "helpers/owned_ptr.h"
#include "./_base.h"

namespace halo::reach::megalo::bolt {
   struct expression;

   struct function_call_argument : public item_base {
      QString name; // if it's using named parameters; else, blank
      cobb::owned_ptr<expression> value = nullptr;

      function_call_argument() {}
      function_call_argument(function_call_argument&&) noexcept;
      function_call_argument& operator=(function_call_argument&&) noexcept;

      function_call_argument(const function_call_argument&) = delete;
      function_call_argument& operator=(const function_call_argument&) = delete;
   };

   //
   // Allowed types:
   // 
   //  - expression
   //  - number literal
   //  - string literal
   //  - timer rate
   //     - a number literal with an optional "%" suffix; must be a valid timer rate value
   //  - variable
   //     - variables
   //     - indexed data (player traits, HUD widgets, etc.)
   //  - word
   //     - any unquoted identifier consisting of letters, numbers, and underscores
   //     - that's an oversimplification; see implementation for "extracting a word" in old parser
   //
}