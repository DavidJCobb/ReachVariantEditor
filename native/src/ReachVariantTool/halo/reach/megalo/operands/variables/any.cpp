#include "any.h"
#include "halo/reach/bitstreams.h"
#include "all_core_types.h"

namespace halo::reach::megalo::operands {
   namespace variables {
      void any::read(bitreader& stream) {
         stream.read(this->type);
         this->value = make_variable_of_type(this->type);
         if (!this->value) {
            if constexpr (bitreader::has_load_process) {
               static_assert(false, "TODO: emit fatal error");
            }
            return;
         }
         this->value->read(stream);
      }
   }
}