#include "incident.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo::operands {
   void incident::read(bitreader& stream) {
      bitnumber_type bn;
      stream.read(bn);
      //
      auto to_signed = (int32_t)(bn); // HACK until bitnumber's sign handling is made less awful
      //
      this->value = value_type::from_int(to_signed);
   }
}