#include "base.h"
#include "../helpers/bitstream.h"
#include "../helpers/bitwriter.h"

#include "megalo/actions.h"
#include "megalo/conditions.h"
#include "megalo/limits.h"
#include "megalo/parse_error_reporting.h"

#include "../formats/sha1.h"

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
   uint32_t offset_before_hashable;
   uint32_t offset_after_hashable;
   //
   if (!this->header.read(stream)) {
      printf("Failed to read MPVR block header.\n");
      return false;
   }
   stream.read(this->hashSHA1);
   stream.skip(4 * 8); // skip four unused bytes
   stream.skip(4 * 8); // == size of variant data in big-endian, i.e. offset_after_hashable - offset_before_hashable
   offset_before_hashable = stream.get_bytespan();
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
   printf("Stream bit pos: %d\n", stream.offset);
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
      printf("\n");
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
   if (this->encodingVersion >= 0x6B) // TU1 encoding version (stock is 0x6A)
      this->titleUpdateData.read(stream);
   offset_after_hashable = stream.get_bytespan();
   this->remainingData.read(stream, this->header.end());
   //
   {
      auto     hasher   = InProgressSHA1();
      uint32_t size     = offset_after_hashable - offset_before_hashable;
      printf("Checking SHA-1 hash... Data size is %08X (%08X - %08X).\n", size, offset_before_hashable, offset_after_hashable);
      uint32_t bufsize  = size + sizeof(reachSHA1Salt) + 4;
      auto     buffer   = (const uint8_t*)stream.buffer.data();
      auto     buffer32 = (const uint32_t*)buffer;
      uint8_t* working  = (uint8_t*)malloc(bufsize);
      memcpy(working, reachSHA1Salt, sizeof(reachSHA1Salt));
      *(uint32_t*)(working + sizeof(reachSHA1Salt)) = cobb::to_big_endian(uint32_t(size));
      memcpy(working + bufsize - size, buffer + offset_before_hashable, size);
      hasher.transform(working, sizeof(reachSHA1Salt) + 4 + size);
      //
      printf("File's existing hash:\n");
      for (int i = 0; i < 5; i++)
         printf("   %08X\n", buffer32[0x2FC / 4 + i]);
      printf("\nOur hash:\n");
      for (int i = 0; i < 5; i++)
         printf("   %08X\n", hasher.hash[i]);
      printf("Check done.\n");
   }
   //
   #if _DEBUG
      __debugbreak();
   #endif
   return true;
}
void ReachBlockMPVR::write(cobb::bitwriter& stream) const noexcept {
   uint32_t offset_before_hashable;
   uint32_t offset_after_hashable;
   //
   this->header.write(stream);
   stream.write(this->hashSHA1);
   stream.pad_bytes(4); // four unused bytes
   stream.pad_bytes(4); // TODO: go back after writing all data and set this to big-endian: (offset_after_hashable - offset_before_hashable)
   offset_before_hashable = stream.get_bytespan();
   this->type.write(stream);
   stream.write(this->encodingVersion, cobb::endian::big);
   stream.write(this->engineVersion,   cobb::endian::big);
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
      //
      s.observers.write(stream);
      s.teamChanges.write(stream);
      s.flags.write(stream);
      //
      a.flags.write(stream);
      a.baseTraits.write(stream);
      a.weaponSet.write(stream);
      a.vehicleSet.write(stream);
      a.powerups.red.traits.write(stream);
      a.powerups.blue.traits.write(stream);
      a.powerups.yellow.traits.write(stream);
      a.powerups.red.duration.write(stream);
      a.powerups.blue.duration.write(stream);
      a.powerups.yellow.duration.write(stream);
      //
      t.scoring.write(stream);
      t.species.write(stream);
      t.designatorSwitchType.write(stream);
      for (int i = 0; i < std::extent<decltype(t.teams)>::value; i++)
         t.teams[i].write(stream);
      //
      l.flags.write(stream);
      for (size_t i = 0; i < l.palettes.size(); i++)
         l.palettes[i].write(stream);
   }
   {
      auto& sd = this->scriptData;
      auto& t = sd.traits;
      auto& o = sd.options;
      stream.write(t.size(), cobb::bitcount(Megalo::Limits::max_script_traits));
      for (auto& traits : t)
         traits.write(stream);
      stream.write(o.size(), cobb::bitcount(Megalo::Limits::max_script_options));
      for (auto& option : o)
         option.write(stream);
      sd.strings.write(stream);
   }
   cobb_test_display_bitwriter_offset("after first piece of script data");
   this->stringTableIndexPointer.write(stream);
   this->localizedName.write(stream);
   this->localizedDesc.write(stream);
   this->localizedCategory.write(stream);
   this->engineIcon.write(stream);
   this->engineCategory.write(stream);
   this->mapPermissions.write(stream);
   this->playerRatingParams.write(stream);
   this->scoreToWin.write(stream);
   this->unkF7A6.write(stream);
   this->unkF7A7.write(stream);
   {
      auto& ot = this->optionToggles;
      auto& e = ot.engine;
      auto& m = ot.megalo;
      //
      e.disabled.write(stream);
      e.hidden.write(stream);
      //
      m.disabled.write(stream);
      m.hidden.write(stream);
   }
   cobb_test_display_bitwriter_offset("after engine option toggles");
   {  // Megalo
      auto& content = this->scriptContent;
      //
      stream.write(content.raw.conditions.size(), cobb::bitcount(Megalo::Limits::max_conditions)); // 10 bits
      for (auto& opcode : content.raw.conditions)
         opcode.write(stream);
      cobb_test_display_bitwriter_offset("after conditions");
      //
      stream.write(content.raw.actions.size(), cobb::bitcount(Megalo::Limits::max_actions)); // 11 bits
      for (auto& opcode : content.raw.actions)
         opcode.write(stream);
      cobb_test_display_bitwriter_offset("after actions");
      //
      stream.write(content.triggers.size(), cobb::bitcount(Megalo::Limits::max_triggers));
      for (auto& trigger : content.triggers)
         trigger.write(stream);
      cobb_test_display_bitwriter_offset("after triggers");
      //
      stream.write(content.stats.size(), cobb::bitcount(Megalo::Limits::max_script_stats));
      for (auto& stat : content.stats)
         stat.write(stream);
      cobb_test_display_bitwriter_offset("after stats");
      //
      {  // Script variable declarations
         auto& v = content.variables;
         v.global.write(stream);
         v.player.write(stream);
         v.object.write(stream);
         v.team.write(stream);
      }
      cobb_test_display_bitwriter_offset("after variable declarations");
      //
      stream.write(content.widgets.size(), cobb::bitcount(Megalo::Limits::max_script_widgets));
      for (auto& widget : content.widgets)
         widget.write(stream);
      cobb_test_display_bitwriter_offset("after widgets");
      //
      content.entryPoints.write(stream);
      content.usedMPObjectTypes.write(stream);
      //
      stream.write(content.forgeLabels.size(), cobb::bitcount(Megalo::Limits::max_script_labels));
      for (auto& label : content.forgeLabels)
         label.write(stream);
      cobb_test_display_bitwriter_offset("after forge labels");
   }
   if (this->encodingVersion >= 0x6B) // TU1 encoding version (stock is 0x6A)
      this->titleUpdateData.write(stream);
   offset_after_hashable = stream.get_bytespan();
   stream.pad_to_bytepos(this->header.write_end());
   //
   {  // SHA-1 hash
      uint32_t stream_offset = stream.get_bytepos();
      stream.go_to_bytepos(this->header.writeState.pos + 0x28);
      const uint8_t* data = stream.data() + 0x28;
      //
      auto hasher = InProgressSHA1();
      hasher.transform(reachSHA1Salt, sizeof(reachSHA1Salt));
      hasher.transform(data, 0x5000);
      //
      stream.write(hasher.hash[0]); // TODO: not sure about endianness...
      stream.write(hasher.hash[1]);
      stream.write(hasher.hash[2]);
      stream.write(hasher.hash[3]);
      stream.write(hasher.hash[4]);
      #if !_DEBUG
         static_assert(false, "Once we determine the right hashing approach through testing, make this consistent with it.");
      #endif
      stream.go_to_bytepos(stream_offset);
   }
}

void GameVariant::test_mpvr_hash(cobb::mapped_file& file) noexcept {
   printf("Testing our hashing algorithm on this game variant...\n");
   //
   const uint8_t*  buffer   = (const uint8_t*)file.data();
   const uint32_t* buffer32 = (const uint32_t*)buffer;
   //
   uint32_t signature = cobb::from_big_endian(buffer32[0x2F0 / 4]);
   if (signature != 'mpvr') {
      printf("Failed to find the mpvr block; signature found was %08X when we expected %08X.\n", signature, 'mpvr');
      return;
   }
   //
   printf("File's existing hash:\n");
   for (int i = 0; i < 5; i++)
      printf("   %08X\n", buffer32[0x2FC / 4 + i]);
   printf("\n");
   //
   auto hasher = InProgressSHA1();
   {
      //
      // DATA TO BE HASHED:
      //
      //  - The hash salt
      //
      //  - A uint32_t indicating the length of the game variant data (always 0x5000?), in 
      //    big-endian
      //
      //  - The game variant data
      //
      //  = The above is all stitched together into one buffer and then hashed. We only specif-
      //    ically need to worry about endianness when writing the length in; we write the salt 
      //    in as bytes and our SHA-1 code reads everything as bytes and stitches them into big-
      //    endian dwords as needed.
      //
      // TODO:
      //
      // I'm pretty sure at this point that they're not serializing the max data size (0x5000, 
      // the number of bytes that a file always contains for holding game variant data) but 
      // rather the actual data size (i.e. how many of those 0x5000 bytes are actually being 
      // used by the game variant). This means two things:
      //
      //  - We can't test our hashing code on an existing file without bundling it with the 
      //    code to actually read that file, i.e. we need to know the stream position in bytes 
      //    at the end of the game variant data and before the filler.
      //
      //     - Test this.
      //
      //     - If that doesn't work, modify the second memcpy we're using and don't add 4. 
      //       I think that four of the padding bytes after the SHA-1 hash in the file aren't 
      //       part of the content that gets hashed but it's hard to be sure.
      //
      //  - Same for saving: we need to compute the hash before we handle the filler.
      //
      uint8_t* working = (uint8_t*)malloc(sizeof(reachSHA1Salt) + 4 + 0x5000);
      memcpy(working, reachSHA1Salt, sizeof(reachSHA1Salt));
      *(uint32_t*)(working + sizeof(reachSHA1Salt)) = cobb::to_big_endian(uint32_t(0x5000));
      memcpy(working + sizeof(reachSHA1Salt) + 4, buffer + 0x2FC + 0x14 + 0x4, 0x5000); // there are four padding bytes after the hash that are not part of the actual "data" that gets hashed
      hasher.transform(working, sizeof(reachSHA1Salt) + 4 + 0x5000);
   }
   printf("Our hash:\n");
   for (int i = 0; i < 5; i++)
      printf("   %08X\n", hasher.hash[i]);
   printf("Test done.\n");
}