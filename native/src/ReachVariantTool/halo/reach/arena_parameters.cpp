#include "arena_parameters.h"
#include <bit>
#include "halo/reach/bitstreams.h"
#include "halo/reach/load_process.h"

#include "halo/reach/load_process_messages/arena_stat_cannot_be_infinity.h"

namespace halo::reach {
   void arena_parameters::read(bitreader& stream) {
      const auto pos = stream.get_position();
      stream.read(
         values,
         show_in_scoreboard
      );
      // arena_stat_cannot_be_infinity
      for (size_t i = 0; i < this->values.size(); ++i) {
         constexpr uint32_t infinity_as_dword = 0x7F800000;
         if (std::bit_cast<uint32_t>(this->values[i]) == infinity_as_dword) {
            stream.emit_error_at<halo::reach::load_process_messages::arena_stat_cannot_be_infinity>(
               {
                  .stat_index = i,
               },
               pos.advanced_by_bits(sizeof(float) * 8 * i)
            );
         }
      }
      // Done.
   }
   void arena_parameters::write(bitwriter& stream) const {
      stream.write(
         values,
         show_in_scoreboard
      );
   }
}