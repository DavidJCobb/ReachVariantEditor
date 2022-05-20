#include "team_definition.h"

namespace halo::reach::custom_game_options {
   void team_definition::read(bitreader& stream) {
      stream.read(
         flags,
         name,
         initial_designator,
         spartan_or_elite,
         colors.primary,
         colors.secondary,
         colors.ui,
         fireteam_count
      );
   }
}