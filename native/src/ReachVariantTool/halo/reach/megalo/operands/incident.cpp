#include "incident.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo::operands {
   void incident::read(bitreader& stream) {
      bitnumber_type bn;
      stream.read(bn);
      this->value = value_type::from_int((value_type::underlying_type)bn);
   }
   void incident::write(bitwriter& stream) const {
      bitnumber_type bn = this->value;
      stream.write(bn);
   }
}