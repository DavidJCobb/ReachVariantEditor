#include "player_traits.h"
#include "halo/reach/bitstreams.h"

#include "halo/reach/megalo/variant_data.h"

#include "halo/reach/megalo/load_process_messages/operand/player_traits/index_out_of_max_bounds.h"

namespace halo::reach::megalo::operands {
   void player_traits::read(bitreader& stream) {
      this->value = nullptr;
      //
      index_type index;
      stream.read(index);
      //
      auto* vd = dynamic_cast<megalo_variant_data*>(stream.get_game_variant_data());
      assert(vd);
      //
      auto& list = vd->script.traits;
      if (index >= limits::script_traits) {
         stream.emit_error_at<halo::reach::load_process_messages::megalo::operands::player_traits::index_out_of_max_bounds>(
            {
               .maximum = limits::script_traits - 1,
               .value   = (size_t)index,
            },
            stream.get_position().rewound_by_bits(index_type::max_bitcount)
         );
         return;
      }
      this->value = list[index];
   }
}