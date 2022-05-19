#include "round.h"

namespace halo::reach::firefight {
   void round::read(bitreader& stream) {
      stream.read(
         skull_flags,
         waves.initial,
         waves.main,
         waves.boss
      );
   }
}