#pragma once
#include "./_base.h"

namespace halo::reach::megalo::AST {
   enum class literal_type {
      none,
      identifier,
      number,
      string,
   };
   
   class literal_base : public item_base {
      protected:
         literal_base(literal_type t) : type(t) {}
      public:
         const literal_type type = literal_type::none;
   };
}