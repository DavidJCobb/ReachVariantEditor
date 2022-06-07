#include "trigger.h"
#include "halo/reach/bitstreams.h"

#include "halo/reach/megalo/load_process_messages/operand/trigger/index_out_of_max_bounds.h"

namespace halo::reach::megalo::operands {
   void trigger::read(bitreader& stream) {
      stream.read(
         index
      );
      if (this->index >= limits::triggers) {
         if constexpr (bitreader::has_load_process) {
            stream.load_process().emit_error<halo::reach::load_process_messages::megalo::operands::trigger::index_out_of_max_bounds>({
               .maximum = limits::triggers - 1,
               .value   = this->index,
            });
         }
      }
   }
}