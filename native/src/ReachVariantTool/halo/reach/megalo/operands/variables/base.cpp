#include "base.h"
#include <bit>
#include "top_level_values.h"
#include "halo/reach/bitstreams.h"

#include "halo/reach/megalo/load_process_messages/operand/variable_base/bad_target.h"
#include "halo/reach/megalo/load_process_messages/operand/variable_base/bad_which.h"

namespace halo::reach::megalo::operands::variables {
   namespace impl {
      void base::read_target_id(bitreader& stream, size_t target_count) {
         this->target_id = stream.read_bits(std::bit_width(target_count - 1));
         if (this->target_id >= target_count) {
            if constexpr (bitreader::has_load_process) {
               stream.load_process().throw_fatal<halo::reach::load_process_messages::megalo::operands::variable_base::bad_target>({
                  .target_id = this->target_id,
               });
            }
            return;
         }
      }
      void base::read_which(bitreader& stream, variable_scope which) {
         this->which = 0;
         //
         size_t count = top_level_values::max_of_type(variable_type_for_scope(which));
         if (!count)
            return;
         this->which = stream.read_bits(std::bit_width(count - 1));
         if (this->which >= count) {
            if constexpr (bitreader::has_load_process) {
               stream.load_process().emit_error<halo::reach::load_process_messages::megalo::operands::variable_base::bad_which>({
                  .index   = this->which,
                  .maximum = count - 1,
                  .type    = which,
               });
            }
            return;
         }
      }
      void base::read_index(bitreader& stream, size_t bitcount) {
         if (!bitcount)
            return;
         this->index = stream.read_bits(bitcount);
      }
   }
}