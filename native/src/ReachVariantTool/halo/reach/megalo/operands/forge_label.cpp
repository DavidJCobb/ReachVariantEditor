#include "forge_label.h"
#include "halo/reach/bitstreams.h"

#include "halo/reach/megalo/variant_data.h"

#include "halo/reach/megalo/load_process_messages/operand/forge_label/index_out_of_max_bounds.h"

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
         stream.emit_error_at<halo::reach::load_process_messages::megalo::operands::forge_label::index_out_of_max_bounds>(
            {
               .maximum = limits::forge_labels - 1,
               .value   = (size_t)index,
            },
            stream.get_position().rewound_by_bits(decltype(index)::max_bitcount)
         );
         return;
      }
      this->value = list[index];
   }
   void forge_label::write(bitwriter& stream) const {
      index_type index = -1;
      if (this->value)
         index = this->value->index;
      stream.write(index);
      if (this->value)
         this->value->write(stream);
   }
}