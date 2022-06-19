#include "variant_data.h"
#include <bit>
#include "halo/reach/bitstreams.h"
#include "./forge_label.h"
#include "./game_option.h"
#include "./game_stat.h"
#include "./hud_widget.h"
#include "./player_trait_set.h"

#include "./load_process_messages/referenced_undefined_indexed_data.h"

namespace halo::reach {
   megalo_variant_data::~megalo_variant_data() {}

   void megalo_variant_data::read(bitreader& stream) {
      set_up_indexed_dummies(
         script.forge_labels,
         script.options,
         script.stats,
         script.traits,
         script.widgets
      );
      //
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
               item.extract_opcodes(stream, this->raw.conditions, this->raw.actions);
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
      // And now for post-load steps.
      //
      {
         using message_type = halo::reach::load_process_messages::megalo::referenced_undefined_indexed_data;
         //
         std::vector<size_t> referenced_dummies;
         size_t last_referenced_index;
         //
         this->script.forge_labels.tear_down_dummies(&referenced_dummies, &last_referenced_index);
         if (!referenced_dummies.empty()) {
            stream.emit_error<message_type>({
               .indices   = referenced_dummies,
               .max_count = last_referenced_index + 1,
               .type      = message_type::data_type::forge_label,
            });
         }
         this->script.options.tear_down_dummies(&referenced_dummies, &last_referenced_index);
         if (!referenced_dummies.empty()) {
            stream.emit_error<message_type>({
               .indices   = referenced_dummies,
               .max_count = last_referenced_index + 1,
               .type      = message_type::data_type::game_option,
            });
         }
         this->script.stats.tear_down_dummies(&referenced_dummies, &last_referenced_index);
         if (!referenced_dummies.empty()) {
            stream.emit_error<message_type>({
               .indices   = referenced_dummies,
               .max_count = last_referenced_index + 1,
               .type      = message_type::data_type::game_stat,
            });
         }
         this->script.traits.tear_down_dummies(&referenced_dummies, &last_referenced_index);
         if (!referenced_dummies.empty()) {
            stream.emit_error<message_type>({
               .indices   = referenced_dummies,
               .max_count = last_referenced_index + 1,
               .type      = message_type::data_type::player_trait_set,
            });
         }
         this->script.widgets.tear_down_dummies(&referenced_dummies, &last_referenced_index);
         if (!referenced_dummies.empty()) {
            stream.emit_error<message_type>({
               .indices   = referenced_dummies,
               .max_count = last_referenced_index + 1,
               .type      = message_type::data_type::hud_widget,
            });
         }
      }
   }
   void megalo_variant_data::write(bitwriter& stream) const {
      stream.write(
         metadata.version.encoding,
         metadata.version.engine
      );
      game_variant_data::write(stream);
      stream.write(
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
         std::bitset<megalo::limits::triggers> seen_triggers;
         std::vector<size_t> indices;
         for (size_t ti = 0; ti < this->script.triggers.size(); ++ti) {
            if (seen_triggers.test(ti))
               continue;
            indices.push_back(ti);
            seen_triggers.set(ti);
            this->script.triggers[ti].extract_nested_trigger_indices(*this, indices, seen_triggers, true);
         }
         //
         std::vector<const megalo::condition*> all_conditions;
         std::vector<const megalo::action*>    all_actions;
         for (size_t ti : indices) {
            const auto& item = this->script.triggers[ti];
            item.flatten_opcodes(all_conditions, all_actions);
         }
         //
         auto _write_opcodes = [&stream](const auto& list, size_t max_count) {
            assert(list.size() < max_count);
            stream.write_bits(std::bit_width(max_count), list.size());
            for (const auto* item : list)
               stream.write(*item);
         };
         _write_opcodes(all_conditions, megalo::limits::conditions);
         _write_opcodes(all_actions,    megalo::limits::actions);
         //
         {  // Triggers
            auto&  list = this->script.triggers;
            stream.write_bits(std::bit_width(megalo::limits::triggers), list.size());
            //
            for (auto& item : list) {
               stream.write(item);
            }
         }
         stream.write(
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
         stream.write(this->title_update_data.tu1);
      }
      if (false) {
         #if !_DEBUG
            static_assert(false, "TODO: 'is forge' check")
         #endif
         stream.write(
            forge_data.flags,
            forge_data.edit_mode_type,
            forge_data.respawn_time,
            forge_data.editor_traits
         );
      }
      //
      // And now for post-load steps.
      //
      clear_all_indexed_dummy_flags(
         script.forge_labels,
         script.options,
         script.stats,
         script.traits,
         script.widgets
      );
   }
}