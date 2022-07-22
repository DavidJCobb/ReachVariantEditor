#include "literal_number.h"

namespace halo::reach::megalo::AST {
   literal_number::literal_number(const literal_number& n) : literal_number() {
      *this = n;
   }
   literal_number& literal_number::operator=(const literal_number& n) {
      this->format = n.format;
      switch (n.format) {
         case literal_number_format::none:
            return;
         case literal_number_format::integer:
            this->value.integer = n.value.integer;
            break;
         case literal_number_format::decimal:
            this->value.decimal = n.value.decimal;
            break;
      }
   }
}