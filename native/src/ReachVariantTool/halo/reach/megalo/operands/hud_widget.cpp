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
         if constexpr (bitreader::has_load_process) {
            stream.load_process().emit_error<halo::reach::load_process_messages::megalo::operands::hud_widget::index_out_of_max_bounds>({
               .maximum = limits::script_widgets - 1,
               .value   = (size_t)index,
            });
         }
         return;
      }
      this->value = list[index];
   }
}