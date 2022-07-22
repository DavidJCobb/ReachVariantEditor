#pragma once
#include <cstdint>
#include "./_base.h"
#include "./literal.h"

namespace halo::reach::megalo::AST {
   enum class literal_number_format {
      none,
      decimal, // double
      integer, // int32_t
   };

   class literal_number : public literal_base {
      public:
         literal_number() : literal_base(literal_type::number) {};

         literal_number(const literal_number& n);
         literal_number& operator=(const literal_number& n);

      public:
         literal_number_format format = literal_number_format::none;
         union {
            double  decimal;
            int32_t integer = 0;
         } value;
   };
}