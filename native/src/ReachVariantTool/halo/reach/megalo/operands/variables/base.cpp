#include "base.h"
#include <bit>
#include "helpers/apply_sign_bit.h"
#include "top_level_values.h"
#include "halo/reach/bitstreams.h"

#include "halo/reach/megalo/load_process_messages/operand/variable_base/bad_target.h"
#include "halo/reach/megalo/load_process_messages/operand/variable_base/bad_which.h"

namespace halo::reach::megalo::operands::variables {
   namespace impl {
      void base::read_target_id(bitreader& stream, size_t target_count) {
         auto bitcount = std::bit_width(target_count - 1);
         //
         this->target_id = stream.read_bits(
            std::max(
               (size_t)1, 
               bitcount
            )
         );
         if (this->target_id >= target_count) {
            stream.throw_fatal_at<halo::reach::load_process_messages::megalo::operands::variable_base::bad_target>(
               {
                  .target_id = this->target_id,
               },
               stream.get_position().rewound_by_bits(bitcount)
            );
            return;
         }
      }
      void base::read_which(bitreader& stream, variable_scope which) {
         this->which = 0;
         //
         size_t count = top_level_values::max_of_type(variable_type_for_scope(which));
         if (!count)
            return;
         auto bitcount = std::bit_width(count - 1);
         this->which = stream.read_bits(bitcount);
         if (this->which >= count) {
            stream.emit_error_at<halo::reach::load_process_messages::megalo::operands::variable_base::bad_which>(
               {
                  .index   = this->which,
                  .maximum = count - 1,
                  .type    = which,
               },
               stream.get_position().rewound_by_bits(bitcount)
            );
            return;
         }
      }
      void base::read_index(bitreader& stream, size_t bitcount) {
         if (!bitcount)
            return;
         this->index = stream.read_bits(bitcount);
      }
      void base::read_immediate(bitreader& stream, size_t bitcount) {
         if (!bitcount)
            return;
         this->index = stream.read_bits(bitcount);
         this->index = cobb::apply_sign_bit(this->index, bitcount);
      }
   }
}