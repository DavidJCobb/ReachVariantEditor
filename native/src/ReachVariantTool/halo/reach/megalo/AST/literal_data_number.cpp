#include "literal_data_number.h"

namespace halo::reach::megalo::AST {
   literal_data_number::literal_data_number(const literal_data_number& n) : literal_data_number() {
      *this = n;
   }
   literal_data_number& literal_data_number::operator=(const literal_data_number& n) {
      this->format = n.format;
      switch (n.format) {
         case format_type::none:
            return;
         case format_type::integer:
            this->value.integer = n.value.integer;
            break;
         case format_type::decimal:
            this->value.decimal = n.value.decimal;
            break;
      }
   }

   bool literal_data_number::is_valid() const {
      return this->format != format_type::none;
   }
}