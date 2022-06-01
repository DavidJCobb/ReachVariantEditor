#include "player_traits.h"
#include "halo/reach/bitstreams.h"

#include "halo/reach/megalo/variant_data.h"

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
         if constexpr (bitreader::has_load_process) {
            static_assert(false, "TODO: report out-of-bounds player trait set");
         }
         return;
      }
      this->value = list[index];
   }
}