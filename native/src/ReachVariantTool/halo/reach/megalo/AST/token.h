#pragma once
#include <QString>
#include "helpers/owned_ptr.h"
#include "./_base.h"
#include "./token_pos.h"
#include "./token_type.h"

namespace halo::reach::megalo::AST {
   class literal_base;

   class token : public item_base {
      public:
         token_type type = token_type::none;
         QString lexeme; // the parsed string
         cobb::owned_ptr<literal_base> literal;
         token_pos pos;
   };
}