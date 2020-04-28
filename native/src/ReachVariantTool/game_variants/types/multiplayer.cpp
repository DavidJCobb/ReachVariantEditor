#include "multiplayer.h"
#include "../../helpers/stream.h"
#include "../../helpers/bitwriter.h"

#include "../components/megalo/actions.h"
#include "../components/megalo/conditions.h"
#include "../components/megalo/limits.h"

#include "../../formats/sha1.h"
#include "../../helpers/sha1.h"

#include "../errors.h"
#include "../warnings.h"

bool GameVariantDataMultiplayer::read(cobb::reader& reader) noexcept {
   auto& error_report = GameEngineVariantLoadError::get();
   error_report.reset();
   GameEngineVariantLoadWarningLog::get().clear();
   //
   auto& stream = reader.bits;
   this->_set_up_indexed_dummies();
   //
   stream.read(this->encodingVersion);
   stream.read(this->engineVersion);
   this->variantHeader.read(stream);
   this->isBuiltIn.read(stream);
   {
      auto& o = this->options;
      auto& m = o.misc;
      auto& r = o.respawn;
      auto& s = o.social;
      auto& a = o.map;
      auto& t = o.team;
      auto& l = o.loadouts;
      //
      m.flags.read(stream);
      m.timeLimit.read(stream);
      m.roundLimit.read(stream);
      m.roundsToWin.read(stream);
      m.suddenDeathTime.read(stream);
      m.gracePeriod.read(stream);
      //
      r.flags.read(stream);
      r.livesPerRound.read(stream);
      r.teamLivesPerRound.read(stream);
      r.respawnTime.read(stream);
      r.suicidePenalty.read(stream);
      r.betrayalPenalty.read(stream);
      r.respawnGrowth.read(stream);
      r.loadoutCamTime.read(stream);
      r.traitsDuration.read(stream);
      r.traits.read(stream);
      //
      s.observers.read(stream);
      s.teamChanges.read(stream);
      s.flags.read(stream);
      //
      a.flags.read(stream);
      a.baseTraits.read(stream);
      a.weaponSet.read(stream);
      a.vehicleSet.read(stream);
      a.powerups.red.traits.read(stream);
      a.powerups.blue.traits.read(stream);
      a.powerups.yellow.traits.read(stream);
      a.powerups.red.duration.read(stream);
      a.powerups.blue.duration.read(stream);
      a.powerups.yellow.duration.read(stream);
      //
      t.scoring.read(stream);
      t.species.read(stream);
      t.designatorSwitchType.read(stream);
      for (int i = 0; i < std::extent<decltype(t.teams)>::value; i++)
         t.teams[i].read(stream);
      //
      l.flags.read(stream);
      for (size_t i = 0; i < l.palettes.size(); i++)
         l.palettes[i].read(stream);
   }
   {
      auto& sd = this->scriptData;
      auto& t = sd.traits;
      auto& o = sd.options;
      int count;
      //
      count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_script_traits));
      for (int i = 0; i < count; i++) {
         auto& traits = t[i];
         traits.read(stream);
      }
      //
      count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_script_options));
      for (int i = 0; i < count; i++) {
         auto& option = o[i];
         option.read(stream);
      }
      //
      sd.strings.read(stream);
      //
      for (auto& traits : t)
         traits.postprocess_string_indices(sd.strings);
      for (auto& option : o)
         option.postprocess_string_indices(sd.strings);
   }
   {
      MegaloStringIndexOptional index;
      index.read(stream);
      this->genericName = this->scriptData.strings.get_entry(index);
   }
   this->localizedName.read(stream);
   this->localizedDesc.read(stream);
   this->localizedCategory.read(stream);
   this->engineIcon.read(stream);
   this->engineCategory.read(stream);
   this->mapPermissions.read(stream);
   this->playerRatingParams.read(stream);
   this->scoreToWin.read(stream);
   this->fireteamsEnabled.read(stream);
   this->symmetric.read(stream);
   {
      auto& ot = this->optionToggles;
      auto& e = ot.engine;
      auto& m = ot.megalo;
      //
      e.disabled.read(stream);
      e.hidden.read(stream);
      //
      m.disabled.read(stream);
      m.hidden.read(stream);
   }
   {  // Megalo
      int  count;
      auto& conditions = this->scriptContent.raw.conditions;
      auto& actions    = this->scriptContent.raw.actions;
      auto& triggers   = this->scriptContent.triggers;
      //
      count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_conditions)); // 10 bits
      conditions.resize(count);
      for (size_t i = 0; i < count; i++) {
         if (!conditions[i].read(stream, *this)) {
            error_report.failure_index = i;
            return false;
         }
         if (!stream.is_in_bounds()) {
            error_report.state         = GameEngineVariantLoadError::load_state::failure;
            error_report.failure_point = GameEngineVariantLoadError::load_failure_point::megalo_conditions;
            error_report.failure_index = i;
            error_report.reason        = GameEngineVariantLoadError::load_failure_reason::block_ended_early;
            return false;
         }
      }
      //
      count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_actions)); // 11 bits
      actions.resize(count);
      for (size_t i = 0; i < count; i++) {
         if (!actions[i].read(stream, *this)) {
            error_report.failure_index = i;
            return false;
         }
         if (!stream.is_in_bounds()) {
            error_report.state         = GameEngineVariantLoadError::load_state::failure;
            error_report.failure_point = GameEngineVariantLoadError::load_failure_point::megalo_actions;
            error_report.failure_index = i;
            error_report.reason        = GameEngineVariantLoadError::load_failure_reason::block_ended_early;
            return false;
         }
      }
      //
      count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_triggers));
      triggers.reserve(count);
      for (size_t i = 0; i < count; i++) {
         auto trigger = triggers.emplace_back();
         if (!trigger->read(stream, *this)) {
            error_report.failure_index = i;
            return false;
         }
         if (!stream.is_in_bounds()) {
            error_report.state         = GameEngineVariantLoadError::load_state::failure;
            error_report.failure_point = GameEngineVariantLoadError::load_failure_point::megalo_triggers;
            error_report.failure_index = i;
            error_report.reason        = GameEngineVariantLoadError::load_failure_reason::block_ended_early;
            return false;
         }
         trigger->postprocess_opcodes(conditions, actions);
      }
      //
      count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_script_stats));
      for (size_t i = 0; i < count; i++) {
         this->scriptContent.stats[i].read(stream, *this);
      }
      //
      {  // Script variable declarations
         auto& v = this->scriptContent.variables;
         v.global.read(stream, *this);
         v.player.read(stream, *this);
         v.object.read(stream, *this);
         v.team.read(stream, *this);
      }
      {  // HUD widget declarations
         count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_script_widgets));
         auto& widgets = this->scriptContent.widgets;
         for (size_t i = 0; i < count; i++) {
            widgets[i].read(stream);
         }
      }
      if (!this->scriptContent.entryPoints.read(stream))
         return false;
      this->scriptContent.usedMPObjectTypes.read(stream);
      //
      {  // Forge labels
         auto&  list  = this->scriptContent.forgeLabels;
         size_t count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_script_labels));
         for (size_t i = 0; i < count; i++) {
            list[i].read(stream, *this);
         }
      }
   }
   if (this->encodingVersion >= 0x6B) // TU1 encoding version (stock is 0x6A)
      this->titleUpdateData.read(stream);
   if (this->isForge) {
      auto& fd = this->forgeData;
      fd.flags.read(stream);
      fd.editModeType.read(stream);
      fd.respawnTime.read(stream);
      fd.editorTraits.read(stream);
   }
   if (stream.get_overshoot_bits() > 0) {
      error_report.state  = GameEngineVariantLoadError::load_state::failure;
      error_report.reason = GameEngineVariantLoadError::load_failure_reason::block_ended_early;
      return false;
   }
   error_report.state = GameEngineVariantLoadError::load_state::success;
   //
   this->_tear_down_indexed_dummies();
   //
   return true;
}
void GameVariantDataMultiplayer::write(cobb::bit_or_byte_writer& writer) noexcept {
   writer.synchronize();
   auto& bits = writer.bits;
   //
   this->encodingVersion = encoding_version_tu1; // upgrade, so that TU1 settings are always saved
   //
   bits.write(this->encodingVersion);
   bits.write(this->engineVersion);
   this->variantHeader.write(bits);
   this->isBuiltIn.write(bits);
   {
      auto& o = this->options;
      auto& m = o.misc;
      auto& r = o.respawn;
      auto& s = o.social;
      auto& a = o.map;
      auto& t = o.team;
      auto& l = o.loadouts;
      //
      m.flags.write(bits);
      m.timeLimit.write(bits);
      m.roundLimit.write(bits);
      m.roundsToWin.write(bits);
      m.suddenDeathTime.write(bits);
      m.gracePeriod.write(bits);
      //
      r.flags.write(bits);
      r.livesPerRound.write(bits);
      r.teamLivesPerRound.write(bits);
      r.respawnTime.write(bits);
      r.suicidePenalty.write(bits);
      r.betrayalPenalty.write(bits);
      r.respawnGrowth.write(bits);
      r.loadoutCamTime.write(bits);
      r.traitsDuration.write(bits);
      r.traits.write(bits);
      //
      s.observers.write(bits);
      s.teamChanges.write(bits);
      s.flags.write(bits);
      //
      a.flags.write(bits);
      a.baseTraits.write(bits);
      a.weaponSet.write(bits);
      a.vehicleSet.write(bits);
      a.powerups.red.traits.write(bits);
      a.powerups.blue.traits.write(bits);
      a.powerups.yellow.traits.write(bits);
      a.powerups.red.duration.write(bits);
      a.powerups.blue.duration.write(bits);
      a.powerups.yellow.duration.write(bits);
      //
      t.scoring.write(bits);
      t.species.write(bits);
      t.designatorSwitchType.write(bits);
      for (int i = 0; i < std::extent<decltype(t.teams)>::value; i++)
         t.teams[i].write(bits);
      //
      l.flags.write(bits);
      for (size_t i = 0; i < l.palettes.size(); i++)
         l.palettes[i].write(bits);
   }
   {
      auto& sd = this->scriptData;
      auto& t = sd.traits;
      auto& o = sd.options;
      bits.write(t.size(), cobb::bitcount(Megalo::Limits::max_script_traits));
      for (auto& traits : t)
         traits.write(bits);
      bits.write(o.size(), cobb::bitcount(Megalo::Limits::max_script_options));
      for (auto& option : o)
         option.write(bits);
      sd.strings.write(bits);
   }
   {
      MegaloStringIndexOptional index = this->genericName->index;
      index.write(bits);
   }
   this->localizedName.write(bits);
   this->localizedDesc.write(bits);
   this->localizedCategory.write(bits);
   this->engineIcon.write(bits);
   this->engineCategory.write(bits);
   this->mapPermissions.write(bits);
   this->playerRatingParams.write(bits);
   this->scoreToWin.write(bits);
   this->fireteamsEnabled.write(bits);
   this->symmetric.write(bits);
   {
      auto& ot = this->optionToggles;
      auto& e = ot.engine;
      auto& m = ot.megalo;
      //
      e.disabled.write(bits);
      e.hidden.write(bits);
      //
      m.disabled.write(bits);
      m.hidden.write(bits);
   }
   {  // Megalo
      auto& content = this->scriptContent;
      //
      {  // Script code
         std::vector<Megalo::Condition*> allConditions;
         std::vector<Megalo::Action*>    allActions;
         for (auto& trigger : content.triggers)
            trigger.prep_for_flat_opcode_lists();
         for (auto& trigger : content.triggers)
            trigger.generate_flat_opcode_lists(*this, allConditions, allActions);
         //
         bits.write(allConditions.size(), cobb::bitcount(Megalo::Limits::max_conditions)); // 10 bits
         for (auto opcode : allConditions)
            opcode->write(bits);
         //
         bits.write(allActions.size(), cobb::bitcount(Megalo::Limits::max_actions)); // 11 bits
         for (auto opcode : allActions)
            opcode->write(bits);
         //
         bits.write(content.triggers.size(), cobb::bitcount(Megalo::Limits::max_triggers));
         for (auto& trigger : content.triggers)
            trigger.write(bits);
      }
      //
      bits.write(content.stats.size(), cobb::bitcount(Megalo::Limits::max_script_stats));
      for (auto& stat : content.stats)
         stat.write(bits);
      //
      {  // Script variable declarations
         auto& v = content.variables;
         v.global.write(bits);
         v.player.write(bits);
         v.object.write(bits);
         v.team.write(bits);
      }
      //
      bits.write(content.widgets.size(), cobb::bitcount(Megalo::Limits::max_script_widgets));
      for (auto& widget : content.widgets)
         widget.write(bits);
      //
      content.entryPoints.write(bits);
      content.usedMPObjectTypes.write(bits);
      //
      bits.write(content.forgeLabels.size(), cobb::bitcount(Megalo::Limits::max_script_labels));
      for (auto& label : content.forgeLabels)
         label.write(bits);
   }
   if (this->encodingVersion >= 0x6B) // TU1 encoding version (stock is 0x6A)
      this->titleUpdateData.write(bits);
   if (this->isForge) {
      auto& fd = this->forgeData;
      fd.flags.write(bits);
      fd.editModeType.write(bits);
      fd.respawnTime.write(bits);
      fd.editorTraits.write(bits);
   }
   writer.synchronize();
}
void GameVariantDataMultiplayer::write_last_minute_fixup(cobb::bit_or_byte_writer& writer) const noexcept {
   auto& bytes = writer.bytes;
   writer.synchronize();
   this->variantHeader.write_last_minute_fixup(writer.bits);
   writer.synchronize();
}
GameVariantData* GameVariantDataMultiplayer::clone() const noexcept {
   auto clone = new GameVariantDataMultiplayer(this->isForge);
   //*clone = *this; // TODO: NOT USABLE; UNIQUE_PTR ARRAYS CANNOT BE COPY-ASSIGNED
   //
   {  // Fix up string table references-by-index.
      auto& stringTable = clone->scriptData.strings;
      auto& cd = clone->scriptData;
      auto& cc = clone->scriptContent;
      for (auto& option : cd.options)
         option.postprocess_string_indices(stringTable);
      for (auto& traits : cd.traits)
         traits.postprocess_string_indices(stringTable);
      //for (auto& stat : cc.stats)
      //   stat.postprocess_string_indices(stringTable);
   }
   return clone;
}
//
namespace {
   template<typename list_type> void __set_up_indexed_dummies(list_type& list) {
      size_t max = list.max_count;
      list.reserve(max);
      for (size_t i = 0; i < max; i++)
         list.emplace_back();
   }
   template<typename list_type> bool __tear_down_indexed_dummies(QString& warning, list_type& list, QString item_format) {
      bool   fails = false;
      size_t max   = list.max_count;
      for (int16_t i = max - 1; i >= 0; --i) { // need a signed loop var or --i will never be less than 0
         auto& item = list[i];
         if (item.is_defined)
            break;
         if (item.get_refcount()) {
            fails = true;
            warning += item_format.arg(i);
            item.is_defined = true;
            continue;
         }
         list.erase(i);
      }
      return fails;
   }
}
void GameVariantDataMultiplayer::_set_up_indexed_dummies() {
   __set_up_indexed_dummies(this->scriptData.traits);
   __set_up_indexed_dummies(this->scriptData.options);
   __set_up_indexed_dummies(this->scriptContent.stats);
   __set_up_indexed_dummies(this->scriptContent.widgets);
   __set_up_indexed_dummies(this->scriptContent.forgeLabels);
}
void GameVariantDataMultiplayer::_tear_down_indexed_dummies() {
   QString warning = QObject::tr("The gametype script contained references to the following undefined objects. These objects will be created with blank data.\n", "out-of-bounds index warning on load");
   bool should_log_warning = false;
   //
   should_log_warning |= __tear_down_indexed_dummies(warning, this->scriptData.traits,         QObject::tr("\nScripted player trait set #%1", "out-of-bounds index warning on load"));
   should_log_warning |= __tear_down_indexed_dummies(warning, this->scriptData.options,        QObject::tr("\nScripted option #%1", "out-of-bounds index warning on load"));
   should_log_warning |= __tear_down_indexed_dummies(warning, this->scriptContent.stats,       QObject::tr("\nScripted stat #%1", "out-of-bounds index warning on load"));
   should_log_warning |= __tear_down_indexed_dummies(warning, this->scriptContent.widgets,     QObject::tr("\nScripted HUD widget #%1", "out-of-bounds index warning on load"));
   should_log_warning |= __tear_down_indexed_dummies(warning, this->scriptContent.forgeLabels, QObject::tr("\nForge label #%1", "out-of-bounds index warning on load"));
   //
   if (should_log_warning)
      GameEngineVariantLoadWarningLog::get().push_back(warning);
}