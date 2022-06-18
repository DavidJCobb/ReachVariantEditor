#include "hud_widget.h"
#include "halo/reach/bitstreams.h"

#include "halo/reach/megalo/variant_data.h"

#include "halo/reach/megalo/load_process_messages/operand/hud_widget/index_out_of_max_bounds.h"

namespace halo::reach::megalo::operands {
   void hud_widget::read(bitreader& stream) {
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
      auto& list = vd->script.widgets;
      if (index >= limits::script_widgets) {
         stream.emit_error_at<halo::reach::load_process_messages::megalo::operands::hud_widget::index_out_of_max_bounds>(
            {
               .maximum = limits::script_widgets - 1,
               .value   = (size_t)index,
            },
            stream.get_position().rewound_by_bits(index_type::max_bitcount)
         );
         return;
      }
      this->value = list[index];
   }
   void hud_widget::write(bitwriter& stream) const {
      index_type index = -1;
      if (this->value) {
         index = this->value->index;
      }
      stream.write(index);
   }
}