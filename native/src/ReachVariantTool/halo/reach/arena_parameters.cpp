#include "arena_parameters.h"
#include <bit>
#include "halo/reach/bitstreams.h"
#include "halo/reach/load_process.h"

namespace halo::reach {
   void arena_parameters::read(bitreader& stream) {
      stream.read(
         values,
         show_in_scoreboard
      );
      if constexpr (bitreader::has_load_process) {
         // arena_stat_cannot_be_infinity
         for (size_t i = 0; i < this->values.size(); ++i) {
            constexpr uint32_t infinity_as_dword = 0x7F800000;
            if (std::bit_cast<uint32_t>(this->values[i]) == infinity_as_dword) {
               stream.load_process().emit_error({
                  .data = halo::reach::load_errors::arena_stat_cannot_be_infinity{
                     .stat_index = i,
                  }
               });
            }
         }
      }
   }
}