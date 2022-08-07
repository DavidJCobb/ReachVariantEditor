#pragma once
#include "./base.h"
#include "../token.h"

namespace halo::reach::megalo::bolt::errors {
   //
   // The parser does not know how to proceed; no rules were matched.
   //
   class token_matches_no_grammar_rule : public base {
      public:
         token fault;
   };
}