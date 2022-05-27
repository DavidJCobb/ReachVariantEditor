#include "forge_label.h"
#include "halo/reach/bitstreams.h"

#include "halo/reach/megalo/variant_data.h"

namespace halo::reach::megalo::operands {
   void forge_label::read(bitreader& stream) {
      this->value = nullptr;
      //
      index_type index;
      stream.read(index);
      if (index == -1)
         return;
      //
      auto* vd = dynamic_cast<megalo_variant_data*>(stream.get_game_variant_data());
      assert(vd);
      //
      auto& list = vd->script.forge_labels;
      if (index >= limits::forge_labels) {
         if constexpr (bitreader::has_load_process) {
            static_assert(false, "TODO: report out-of-bounds label");
         }
         return;
      }
      this->value = list[index];
   }
}