#include "hud_widget.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo {
   void hud_widget::read(bitreader& stream) {
      stream.read(
         position
      );
   }
}