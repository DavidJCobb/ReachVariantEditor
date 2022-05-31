#include "base.h"
#include <bit>
#include "top_level_values.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo::operands::variables {
   namespace impl {
      void base::read_target_id(bitreader& stream, size_t target_count) {
         this->target_id = stream.read_bits(std::bit_width(target_count - 1));
         if (this->target_id >= target_count) {
            if constexpr (bitreader::has_load_process) {
               static_assert(false, "TODO: emit a fatal error.");
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
               static_assert(false, "TODO: emit a non-fatal error here");
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