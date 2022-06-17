#include "variant_data.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach {
   void firefight_variant_data::read(bitreader& stream) {
      game_variant_data::read(stream);
      stream.read(
         scenario_flags,
         unk0A,
         wave_limit,
         unk0B,
         unk0C,
         elite_kill_bonus,
         starting_lives.spartan,
         starting_lives.elite,
         unk0D,
         max_spartan_extra_lives,
         generator_count,
         base_traits.spartan,
         base_traits.elite,
         base_traits.wave,
         custom_skulls,
         elite_respawn_options,
         rounds,
         bonus_wave.duration,
         bonus_wave.skulls,
         bonus_wave.data
      );
   }
   void firefight_variant_data::write(bitwriter& stream) const {
      game_variant_data::write(stream);
      stream.write(
         scenario_flags,
         unk0A,
         wave_limit,
         unk0B,
         unk0C,
         elite_kill_bonus,
         starting_lives.spartan,
         starting_lives.elite,
         unk0D,
         max_spartan_extra_lives,
         generator_count,
         base_traits.spartan,
         base_traits.elite,
         base_traits.wave,
         custom_skulls,
         elite_respawn_options,
         rounds,
         bonus_wave.duration,
         bonus_wave.skulls,
         bonus_wave.data
      );
   }
}