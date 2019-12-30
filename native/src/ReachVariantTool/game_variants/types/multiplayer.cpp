#include "multiplayer.h"
#include "../../helpers/bitreader.h"
#include "../../helpers/bitwriter.h"

#include "../components/megalo/actions.h"
#include "../components/megalo/conditions.h"
#include "../components/megalo/limits.h"

#include "../../formats/sha1.h"
#include "../../helpers/sha1.h"

#include "../errors.h"

bool GameVariantDataMultiplayer::read(cobb::bit_or_byte_reader& reader) noexcept {
   auto& error_report = GameEngineVariantLoadError::get();
   error_report.reset();
   //
   reader.synchronize();
   auto& stream = reader.bits;
   //
   stream.read(this->encodingVersion);
   stream.read(this->engineVersion);
   this->variantHeader.read(stream);
   this->flags.read(stream);
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
      t.resize(count);
      for (int i = 0; i < count; i++)
         t[i].read(stream);
      //
      count = stream.read_bits(cobb::bitcount(16));
      o.resize(count);
      for (int i = 0; i < count; i++)
         o[i].read(stream);
      //
      sd.strings.read(stream);
      //
      for (auto& traits : t)
         traits.postprocess_string_indices(sd.strings);
      for (auto& option : o)
         option.postprocess_string_indices(sd.strings);
   }
   this->stringTableIndexPointer.read(stream);
   this->localizedName.read(stream);
   this->localizedDesc.read(stream);
   this->localizedCategory.read(stream);
   this->engineIcon.read(stream);
   this->engineCategory.read(stream);
   this->mapPermissions.read(stream);
   this->playerRatingParams.read(stream);
   this->scoreToWin.read(stream);
   this->unkF7A6.read(stream);
   this->unkF7A7.read(stream);
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
         if (!conditions[i].read(stream)) {
            error_report.failure_index = i;
            return false;
         }
      }
      //
      count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_actions)); // 11 bits
      actions.resize(count);
      for (size_t i = 0; i < count; i++) {
         if (!actions[i].read(stream)) {
            error_report.failure_index = i;
            return false;
         }
      }
      //
      count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_triggers));
      triggers.resize(count);
      for (size_t i = 0; i < count; i++) {
         if (!triggers[i].read(stream)) {
            error_report.failure_index = i;
            return false;
         }
         triggers[i].postprocess_opcodes(conditions, actions);
      }
      /*//
      printf("\nFull script content:");
      for (size_t i = 0; i < triggers.size(); ++i) {
         auto& trigger = triggers[i];
         if (trigger.entryType == Megalo::entry_type::subroutine)
            continue;
         printf("\nTRIGGER #%d:\n", i);
         std::string out;
         trigger.to_string(triggers, out);
         printf(out.c_str());
      }
      printf("\n");
      //*/
      //
      count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_script_stats));
      this->scriptContent.stats.resize(count);
      for (size_t i = 0; i < count; i++) {
         this->scriptContent.stats[i].read(stream);
         this->scriptContent.stats[i].postprocess_string_indices(this->scriptData.strings);
      }
      //
      {  // Script variable declarations
         auto& v = this->scriptContent.variables;
         v.global.read(stream);
         v.player.read(stream);
         v.object.read(stream);
         v.team.read(stream);
      }
      {  // HUD widget declarations
         auto& widgets = this->scriptContent.widgets;
         widgets.resize(stream.read_bits(cobb::bitcount(Megalo::Limits::max_script_widgets)));
         for (auto& widget : widgets)
            widget.read(stream);
      }
      if (!this->scriptContent.entryPoints.read(stream))
         return false;
      this->scriptContent.usedMPObjectTypes.read(stream);
      //
      {  // Forge labels
         size_t count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_script_labels));
         this->scriptContent.forgeLabels.resize(count);
         for (auto& label : this->scriptContent.forgeLabels) {
            label.read(stream);
            label.postprocess_string_indices(this->scriptData.strings);
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
   error_report.state = GameEngineVariantLoadError::load_state::success;
   return true;
}
void GameVariantDataMultiplayer::write(cobb::bit_or_byte_writer& writer) const noexcept {
   writer.synchronize();
   auto& bits = writer.bits;
   //
   bits.write(this->encodingVersion);
   bits.write(this->engineVersion);
   this->variantHeader.write(bits);
   this->flags.write(bits);
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
   this->stringTableIndexPointer.write(bits);
   this->localizedName.write(bits);
   this->localizedDesc.write(bits);
   this->localizedCategory.write(bits);
   this->engineIcon.write(bits);
   this->engineCategory.write(bits);
   this->mapPermissions.write(bits);
   this->playerRatingParams.write(bits);
   this->scoreToWin.write(bits);
   this->unkF7A6.write(bits);
   this->unkF7A7.write(bits);
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
      bits.write(content.raw.conditions.size(), cobb::bitcount(Megalo::Limits::max_conditions)); // 10 bits
      for (auto& opcode : content.raw.conditions)
         opcode.write(bits);
      //
      bits.write(content.raw.actions.size(), cobb::bitcount(Megalo::Limits::max_actions)); // 11 bits
      for (auto& opcode : content.raw.actions)
         opcode.write(bits);
      //
      bits.write(content.triggers.size(), cobb::bitcount(Megalo::Limits::max_triggers));
      for (auto& trigger : content.triggers)
         trigger.write(bits);
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