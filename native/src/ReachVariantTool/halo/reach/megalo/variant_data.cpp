#include "variant_data.h"
#include <bit>
#include "halo/reach/bitstreams.h"
#include "./forge_label.h"
#include "./game_option.h"
#include "./game_stat.h"
#include "./hud_widget.h"
#include "./player_trait_set.h"

namespace halo::reach {
   void megalo_variant_data::read(bitreader& stream) {
      stream.read(
         metadata.version.encoding,
         metadata.version.engine
      );
      game_variant_data::read(stream);
      stream.read(
         script.traits,
         script.options,
         script.strings,
         script.generic_name,
         metadata.localized_info.name,
         metadata.localized_info.description,
         metadata.localized_info.category,
         metadata.engine_icon,
         metadata.engine_category,
         map_permissions,
         arena_parameters,
         options_mp.score_to_win,
         options_mp.fireteams_enabled,
         options_mp.symmetric,
         option_toggles.engine.disabled,
         option_toggles.engine.hidden,
         option_toggles.megalo.disabled,
         option_toggles.megalo.hidden
      );
      //
      {  // Read Megalo code
         {  // Conditions
            size_t count = stream.read_bits(std::bit_width(megalo::limits::conditions));
            auto&  list  = this->raw.conditions;
            //
            list.resize(count);
            for (size_t i = 0; i < count; ++i) {
               auto& item = list[i];
               stream.read(item);
            }
         }
         {  // Actions
            size_t count = stream.read_bits(std::bit_width(megalo::limits::actions));
            auto&  list  = this->raw.actions;
            //
            list.resize(count);
            for (size_t i = 0; i < count; ++i) {
               auto& item = list[i];
               stream.read(item);
            }
         }
         {  // Triggers
            size_t count = stream.read_bits(std::bit_width(megalo::limits::triggers));
            auto&  list  = this->script.triggers;
            //
            list.resize(count);
            for (size_t i = 0; i < count; ++i) {
               auto& item = list[i];
               stream.read(item);
               item.extract_opcodes(this->raw.conditions, this->raw.actions);
            }
         }
         stream.read(
            script.stats,
            script.variables.global,
            script.variables.player,
            script.variables.object,
            script.variables.team,
            script.widgets,
            script.entry_points,
            script.used_object_types,
            script.forge_labels
         );
      }
      if (this->metadata.version.encoding >= 0x6B) {
         stream.read(this->title_update_data.tu1);
      }
      if (false) {
         #if !_DEBUG
            static_assert(false, "TODO: 'is forge' check")
         #endif
         stream.read(
            forge_data.flags,
            forge_data.edit_mode_type,
            forge_data.respawn_time,
            forge_data.editor_traits
         );
      }

      //
      // post-load steps here
      //
      static_assert(false, "FINISH ME");
   }
}