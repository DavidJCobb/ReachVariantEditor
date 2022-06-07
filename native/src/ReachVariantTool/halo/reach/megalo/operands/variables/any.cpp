#include "any.h"
#include "halo/reach/bitstreams.h"
#include "all_core_types.h"

#include "halo/reach/megalo/load_process_messages/operand/any/bad_type.h"

namespace halo::reach::megalo::operands {
   namespace variables {
      void any::read(bitreader& stream) {
         stream.read(this->type);
         this->value = make_variable_of_type(this->type);
         if (!this->value) {
            if constexpr (bitreader::has_load_process) {
               stream.load_process().throw_fatal<halo::reach::load_process_messages::megalo::operands::any::bad_type>({
                  .type_value = (size_t)this->value,
               });
            }
            return;
         }
         this->value->read(stream);
      }
   }
}