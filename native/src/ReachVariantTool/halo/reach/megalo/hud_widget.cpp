#include "hud_widget.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo {
   void hud_widget::read(bitreader& stream) {
      stream.read(
         position
      );
   }
   void hud_widget::write(bitwriter& stream) const {
      stream.write(
         position
      );
   }
}