#include "trigger.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo::operands {
   void trigger::read(bitreader& stream) {
      stream.read(
         index
      );
      if (this->index >= limits::triggers) {
         if constexpr (bitreader::has_load_process) {
            static_assert(false, "TODO: emit non-critical error here");
         }
      }
   }
}