#pragma once
#include <QString>
#include "helpers/owned_ptr.h"
#include "./_base.h"
#include "./literal.h"
#include "./token_pos.h"
#include "./token_type.h"

namespace halo::reach::megalo::bolt {
   class token : public item_base {
      public:
         token_type type = token_type::none;
         QString lexeme; // the parsed string
         literal_item literal;
         token_pos pos;
   };
}