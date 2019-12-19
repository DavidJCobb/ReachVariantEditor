#include "base.h"
#include "../helpers/bitstream.h"
#include "../helpers/bitwriter.h"

#include "megalo/actions.h"
#include "megalo/conditions.h"
#include "megalo/limits.h"
#include "megalo/parse_error_reporting.h"

#define cobb_test_display_bitwriter_offset(text) printf("== Writer bytepos " text ": %08X\n", stream.get_bytepos());

bool BlamHeader::read(cobb::bitstream& stream) noexcept {
   this->header.read(stream);
   stream.read(this->data.unk0C);
   stream.read(this->data.unk0E);
   stream.read(this->data.unk2E);
   return true;
}
void BlamHeader::write(cobb::bitwriter& stream) const noexcept {
   this->header.write(stream);
   stream.enlarge_by(0x24);
   stream.write(this->data.unk0C, cobb::endian::big);
   stream.write(this->data.unk0E);
   stream.write(this->data.unk2E);
}

bool GameVariantHeader::read(cobb::bitstream& stream) noexcept {
   this->build.major = 0; // not in mpvr
   this->build.minor = 0; // not in mpvr
   this->contentType.read(stream);
   this->fileLength.read(stream);
   this->unk08.read(stream);
   this->unk10.read(stream);
   this->unk18.read(stream);
   this->unk20.read(stream);
   this->activity.read(stream);
   this->gameMode.read(stream);
   this->engine.read(stream);
   this->unk2C.read(stream);
   this->engineCategory.read(stream);
   this->createdBy.read(stream);
   this->modifiedBy.read(stream);
   stream.read_wstring(this->title,       128); // big-endian
   stream.read_wstring(this->description, 128); // big-endian
   if (this->contentType == 6) {
      this->engineIcon.read(stream);
   }
   if (this->activity == 2)
      stream.skip(16); // TODO: hopper ID
   return true;
}
bool GameVariantHeader::read(cobb::bytestream& stream) noexcept {
   this->build.major.read(stream);
   this->build.minor.read(stream);
   this->contentType.read(stream);
   stream.pad(3);
   this->fileLength.read(stream);
   this->unk08.read(stream);
   this->unk10.read(stream);
   this->unk18.read(stream);
   this->unk20.read(stream);
   this->activity.read(stream);
   this->gameMode.read(stream);
   this->engine.read(stream);
   stream.pad(1);
   this->unk2C.read(stream);
   this->engineCategory.read(stream);
   stream.pad(4);
   this->createdBy.read(stream);
   this->modifiedBy.read(stream);
   stream.read_wstring(this->title, 128);
   stream.read_wstring(this->description, 128);
   this->engineIcon.read(stream);
   stream.read(this->unk284);
   return true;
}
void GameVariantHeader::write_bits(cobb::bitwriter& stream) const noexcept {
   this->contentType.write(stream);
   #if _DEBUG
      this->fileLength.write(stream);
   #else
      static_assert(false, "TODO: Write out a dummy file length and then, after the file is fully written, come back and overwrite that length with a real value.");
   #endif
   this->unk08.write(stream);
   this->unk10.write(stream);
   this->unk18.write(stream);
   this->unk20.write(stream);
   cobb_test_display_bitwriter_offset("after MPVR int64s");
   this->activity.write(stream);
   this->gameMode.write(stream);
   this->engine.write(stream);
   this->unk2C.write(stream);
   this->engineCategory.write(stream);
   cobb_test_display_bitwriter_offset("before MPVR created-by");
   this->createdBy.write_bits(stream);
   cobb_test_display_bitwriter_offset("before MPVR modified-by");
   this->modifiedBy.write_bits(stream);
   cobb_test_display_bitwriter_offset("before MPVR title");
   stream.write_wstring(this->title,       128); // big-endian
   cobb_test_display_bitwriter_offset("before MPVR desc");
   stream.write_wstring(this->description, 128); // big-endian
   if (this->contentType == 6) {
      this->engineIcon.write(stream);
   }
   if (this->activity == 2)
      assert(false && "Hopper ID writing not implemented!"); // TODO: hopper ID
}
void GameVariantHeader::write_bytes(cobb::bitwriter& stream) const noexcept {
   this->build.major.write_bytes(stream);
   this->build.minor.write_bytes(stream);
   this->contentType.write_bytes(stream);
   stream.pad_bytes(3);
   #if _DEBUG
      this->fileLength.write_bytes(stream);
   #else
      static_assert(false, "TODO: Write out a dummy file length and then, after the file is fully written, come back and overwrite that length with a real value.");
   #endif
   this->unk08.write_bytes(stream);
   this->unk10.write_bytes(stream);
   this->unk18.write_bytes(stream);
   this->unk20.write_bytes(stream);
   this->activity.write_bytes(stream);
   this->gameMode.write_bytes(stream);
   this->engine.write_bytes(stream);
   stream.pad_bytes(1);
   this->unk2C.write_bytes(stream);
   this->engineCategory.write_bytes(stream);
   stream.pad_bytes(4);
   this->createdBy.write_bytes(stream);
   this->modifiedBy.write_bytes(stream);
   stream.write(this->title);
   stream.write(this->description);
   this->engineIcon.write_bytes(stream);
   stream.write(this->unk284);
}

void ReachTeamData::read(cobb::bitstream& stream) noexcept {
   this->flags.read(stream);
   this->name.read(stream);
   this->initialDesignator.read(stream);
   this->spartanOrElite.read(stream);
   this->colorPrimary.read(stream);
   this->colorSecondary.read(stream);
   this->colorText.read(stream);
   this->fireteamCount.read(stream);
}
void ReachTeamData::write(cobb::bitwriter& stream) const noexcept {
   this->flags.write(stream);
   this->name.write(stream);
   this->initialDesignator.write(stream);
   this->spartanOrElite.write(stream);
   this->colorPrimary.write(stream);
   this->colorSecondary.write(stream);
   this->colorText.write(stream);
   this->fireteamCount.write(stream);
}

bool ReachBlockMPVR::read(cobb::bitstream& stream) {
   Megalo::ParseState::reset();
   //
   if (!this->header.read(stream)) {
      printf("Failed to read MPVR block header.\n");
      return false;
   }
   stream.read(this->hashSHA1);
   stream.skip(8 * 8); // skip eight bytes
   this->type.read(stream);
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
      count = stream.read_bits(cobb::bitcount(16));
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
   printf("Stream bit pos: %d\n", stream.offset);
   {
      auto& ot = this->optionToggles;
      auto& e = ot.engine;
      auto& m = ot.megalo;
      //
      #if _DEBUG
         uint32_t offset = stream.offset;
      #endif
      e.disabled.read(stream);
      e.hidden.read(stream);
      #if _DEBUG
         uint32_t distance = stream.offset - offset;
         if (distance != 2560) {
            //
            // this doesn't trip, so as of this writing we ARE handling this correctly
            //
            // we ARE reading megalo data from the right place; the problem is some hard-to-find 
            // mistake we're making when reading it
            //
            printf("WARNING: Possible incorrect handling for ReachGameVariantEngineOptionToggles.\n"
               "Data type holds 1272 bits; minimum size needed to hold this using uint32_ts as the "
               "game does is 1280 bits. After loading two of these, we should be 1280 * 2 = 2560 bits "
               "ahead of the start; however, we are only %d bits ahead.",
               distance
            );
         }
      #endif
      //
      m.disabled.read(stream);
      m.hidden.read(stream);
   }
   {  // Megalo
      uint32_t stream_bitpos = stream.offset;
      {
         int  count;
         int  i;
         bool success = true;
         auto& conditions = this->scriptContent.raw.conditions;
         auto& actions    = this->scriptContent.raw.actions;
         auto& triggers   = this->scriptContent.triggers;
         //
         count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_conditions)); // 10 bits
         conditions.resize(count);
         for (i = 0; i < count; i++) {
            if (!(success = conditions[i].read(stream))) {
               Megalo::ParseState::get().opcode_index = i;
               break;
            }
         }
         Megalo::ParseState::print();
         if (!success) {
            return false;
         }
         //
         count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_actions)); // 11 bits
         actions.resize(count);
         for (i = 0; i < count; i++) {
            if (!(success = actions[i].read(stream))) {
               Megalo::ParseState::get().opcode_index = i;
               break;
            }
         }
         Megalo::ParseState::print();
         if (!success) {
            return false;
         }
         //
         count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_triggers));
         triggers.resize(count);
         for (i = 0; i < count; i++) {
            printf("Reading trigger %d of %d...\n", i, count);
            triggers[i].read(stream);
            triggers[i].postprocess_opcodes(conditions, actions);
         }
         printf("\nFull script content:");
         for (size_t i = 0; i < triggers.size(); ++i) {
            auto& trigger = triggers[i];
            if (trigger.entry == Megalo::trigger_type::subroutine)
               continue;
            printf("\nTRIGGER #%d:\n", i);
            std::string out;
            trigger.to_string(triggers, out);
            printf(out.c_str());
         }
         //
         count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_script_stats));
         this->scriptContent.stats.resize(count);
         for (int i = 0; i < count; i++) {
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
   }
   if (this->encodingVersion >= 0x6B) // TU1 encoding version (stock is 0x6A)
      this->titleUpdateData.read(stream);
   this->remainingData.read(stream, this->header.end());
   //
   #if _DEBUG
      __debugbreak();
   #endif
   return true;
}
void ReachBlockMPVR::write(cobb::bitwriter& stream) const noexcept {
   this->header.write(stream);
   stream.write(this->hashSHA1);
   stream.pad_bytes(8);
   this->type.write(stream);
   stream.write(this->encodingVersion);
   stream.write(this->engineVersion);
   cobb_test_display_bitwriter_offset("after encoding+engine versions");
   this->variantHeader.write_bits(stream);
   this->flags.write(stream);
   cobb_test_display_bitwriter_offset("before options");
   {
      auto& o = this->options;
      auto& m = o.misc;
      auto& r = o.respawn;
      auto& s = o.social;
      auto& a = o.map;
      auto& t = o.team;
      auto& l = o.loadouts;
      //
      m.flags.write(stream);
      m.timeLimit.write(stream);
      m.roundLimit.write(stream);
      m.roundsToWin.write(stream);
      m.suddenDeathTime.write(stream);
      m.gracePeriod.write(stream);
      cobb_test_display_bitwriter_offset("after misc options");
      //
      r.flags.write(stream);
      r.livesPerRound.write(stream);
      r.teamLivesPerRound.write(stream);
      r.respawnTime.write(stream);
      r.suicidePenalty.write(stream);
      r.betrayalPenalty.write(stream);
      r.respawnGrowth.write(stream);
      r.loadoutCamTime.write(stream);
      r.traitsDuration.write(stream);
      r.traits.write(stream);
      cobb_test_display_bitwriter_offset("after respawn options");
      //
      s.observers.write(stream);
      s.teamChanges.write(stream);
      s.flags.write(stream);
      cobb_test_display_bitwriter_offset("after social options");
      //
      a.flags.write(stream);
      a.baseTraits.write(stream);
      a.weaponSet.write(stream);
      a.vehicleSet.write(stream);
      cobb_test_display_bitwriter_offset("in map options, between basic options and powerup traits");
      a.powerups.red.traits.write(stream);
      a.powerups.blue.traits.write(stream);
      a.powerups.yellow.traits.write(stream);
      cobb_test_display_bitwriter_offset("in map options, between powerup traits and durations");
      a.powerups.red.duration.write(stream);
      a.powerups.blue.duration.write(stream);
      a.powerups.yellow.duration.write(stream);
      cobb_test_display_bitwriter_offset("after map options");
      //
      t.scoring.write(stream);
      t.species.write(stream);
      t.designatorSwitchType.write(stream);
      for (int i = 0; i < std::extent<decltype(t.teams)>::value; i++)
         t.teams[i].write(stream);
      cobb_test_display_bitwriter_offset("after team options");
      //
      l.flags.write(stream);
      for (size_t i = 0; i < l.palettes.size(); i++)
         l.palettes[i].write(stream);
      cobb_test_display_bitwriter_offset("after loadout options");
   }
   #if !_DEBUG
      static_assert(false, "FINISH ME");
   #endif

}