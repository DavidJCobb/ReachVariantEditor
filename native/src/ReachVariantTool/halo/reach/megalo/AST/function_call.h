#pragma once
#include <vector>
#include <QString>
#include "./_base.h"

namespace halo::reach::megalo::AST {
   class function_call_argument : public item_base {
      public:
         QString content; // TODO: PLACEHOLDER

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
   };

   class function_call : public item_base {
      public:
         QString function_name;
         QString context; // TODO: PLACEHOLDER: the thing (namespace or variable) we're calling a function on
         std::vector<function_call_argument> arguments;
   };
}