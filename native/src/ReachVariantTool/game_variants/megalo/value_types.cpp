#include "value_types.h"

bool MegaloValue::read(cobb::bitstream& stream) noexcept {
   assert(this->type && "Cannot read a value that hasn't had its type set!");
   switch (this->type->underlying_type) {
      case MegaloValueUnderlyingType::boolean:
         this->value.boolean = stream.read_bits(1) != 0;
         break;
      default:
         assert(false && "Type not implemented!");
   }
}