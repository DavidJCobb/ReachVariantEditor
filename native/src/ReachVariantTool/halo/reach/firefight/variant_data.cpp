#include "variant_data.h"

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
      if (stream.get_overshoot_bits() > 0) {
         if constexpr (bitreader::has_load_process) {
            #if !_DEBUG
               static_assert(false, "TODO: Improve error reporting for this case. Current error-report only allows byte offsets, and we can't get the block size from here.");
            #endif
            stream.load_process().emit_error({
               .data = halo::common::load_errors::file_block_unexpected_end{
                  .block     = {},
                  .overshoot = stream.get_overshoot_bits(),
               }
            });
         }
      }
   }
}