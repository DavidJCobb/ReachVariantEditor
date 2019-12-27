#include "base.h"
#include "../helpers/bitreader.h"
#include "../helpers/bitwriter.h"

#include "megalo/actions.h"
#include "megalo/conditions.h"
#include "megalo/limits.h"
#include "megalo/parse_error_reporting.h"

#include "../formats/sha1.h"
#include "../helpers/sha1.h"

bool BlamHeader::read(cobb::bytereader& stream) noexcept {
   this->header.read(stream);
   stream.read(this->data.unk0C, cobb::endian::big); // endianness assumed but not known
   stream.read(this->data.unk0E, cobb::endian::big); // endianness assumed but not known
   stream.read(this->data.unk2E, cobb::endian::big); // endianness assumed but not known
   return true;
}
void BlamHeader::write(cobb::bytewriter& stream) const noexcept {
   this->header.write(stream);
   stream.enlarge_by(0x24);
   stream.write(this->data.unk0C, cobb::endian::big);
   stream.write(this->data.unk0E);
   stream.write(this->data.unk2E);
   this->header.write_postprocess(stream);
}

bool EOFBlock::read(cobb::bytereader& stream) noexcept {
   ReachFileBlock::read(stream);
   stream.read(this->length);
   stream.read(this->unk04);
   return true;
}
void EOFBlock::write(cobb::bytewriter& stream) const noexcept {
   uint32_t myPos = stream.get_bytespan();
   ReachFileBlock::write(stream);
   stream.write(myPos, cobb::endian::big); // NOTE: in some files this is little-endian, without the block having a different version or flags
   stream.write(uint8_t(0));
}

bool GameVariantHeader::read(cobb::bitreader& stream) noexcept {
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
   stream.read_u16string(this->title, 128); // big-endian
   this->title[127] = '\0';
   stream.read_u16string(this->description, 128); // big-endian
   this->description[127] = '\0';
   if (this->contentType == 6) {
      this->engineIcon.read(stream);
   }
   if (this->activity == 2)
      stream.skip(16); // TODO: hopper ID
   return true;
}
bool GameVariantHeader::read(cobb::bytereader& stream) noexcept {
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
   stream.read_u16string(this->title, 128);
   this->title[127] = '\0';
   stream.read_u16string(this->description, 128);
   this->description[127] = '\0';
   this->engineIcon.read(stream);
   stream.read(this->unk284);
   return true;
}
void GameVariantHeader::write(cobb::bitwriter& stream) const noexcept {
   this->contentType.write(stream);
   this->writeData.offset_of_file_length = stream.get_bitpos();
   this->fileLength.write(stream); // handled fully in the write_last_minute_fixup member function
   this->unk08.write(stream);
   this->unk10.write(stream);
   this->unk18.write(stream);
   this->unk20.write(stream);
   this->activity.write(stream);
   this->gameMode.write(stream);
   this->engine.write(stream);
   this->unk2C.write(stream);
   this->engineCategory.write(stream);
   this->createdBy.write(stream);
   this->modifiedBy.write(stream);
   stream.write_u16string(this->title,       128); // big-endian
   stream.write_u16string(this->description, 128); // big-endian
   if (this->contentType == 6) {
      this->engineIcon.write(stream);
   }
   if (this->activity == 2)
      assert(false && "Hopper ID writing not implemented!"); // TODO: hopper ID
}
void GameVariantHeader::write(cobb::bytewriter& stream) const noexcept {
   this->build.major.write(stream);
   this->build.minor.write(stream);
   this->contentType.write(stream);
   stream.pad(3);
   this->writeData.offset_of_file_length = stream.get_bytepos();
   this->fileLength.write(stream); // handled fully in the write_last_minute_fixup member function
   this->unk08.write(stream);
   this->unk10.write(stream);
   this->unk18.write(stream);
   this->unk20.write(stream);
   this->activity.write(stream);
   this->gameMode.write(stream);
   this->engine.write(stream);
   stream.pad(1);
   this->unk2C.write(stream);
   this->engineCategory.write(stream);
   stream.pad(4);
   this->createdBy.write(stream);
   this->modifiedBy.write(stream);
   stream.write(this->title);
   stream.write(this->description);
   this->engineIcon.write(stream);
   stream.write(this->unk284);
}
void GameVariantHeader::write_last_minute_fixup(cobb::bitwriter& stream) const noexcept {
   stream.write_to_bitpos(this->writeData.offset_of_file_length, cobb::bits_in<uint32_t>, stream.get_bytespan());
}
void GameVariantHeader::write_last_minute_fixup(cobb::bytewriter& stream) const noexcept {
   stream.write_to_offset(this->writeData.offset_of_file_length, stream.get_bytespan(), cobb::endian::little);
}
//
void GameVariantHeader::set_title(const char16_t* value) noexcept {
   memset(this->title, 0, sizeof(this->title));
   for (size_t i = 0; i < std::extent<decltype(this->title)>::value; i++) {
      char16_t c = value[i];
      if (!c)
         break;
      this->title[i] = c;
   }
}
void GameVariantHeader::set_description(const char16_t* value) noexcept {
   memset(this->description, 0, sizeof(this->description));
   for (size_t i = 0; i < std::extent<decltype(this->description)>::value; i++) {
      char16_t c = value[i];
      if (!c)
         break;
      this->description[i] = c;
   }
}

void ReachTeamData::read(cobb::bitreader& stream) noexcept {
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

bool ReachBlockMPVR::read(cobb::bit_or_byte_reader& reader) {
   Megalo::ParseState::reset();
   uint32_t offset_before_hashable;
   uint32_t offset_after_hashable;
   //
   if (!this->header.read(reader.bytes)) {
      printf("Failed to read MPVR block header.\n");
      return false;
   }
   reader.synchronize();
   auto& stream = reader.bits;
   //
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
   printf("Stream bit pos: %d\n", stream.get_bitpos());
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
         if (trigger.entryType == Megalo::entry_type::subroutine)
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
      auto     hasher   = cobb::sha1();
      uint32_t size     = offset_after_hashable - offset_before_hashable;
      printf("Checking SHA-1 hash... Data size is %08X (%08X - %08X).\n", size, offset_before_hashable, offset_after_hashable);
      uint32_t bufsize  = size + sizeof(reachSHA1Salt) + 4;
      auto     buffer   = (const uint8_t*)stream.data();
      auto     buffer32 = (const uint32_t*)buffer;
      uint8_t* working  = (uint8_t*)malloc(bufsize);
      memcpy(working, reachSHA1Salt, sizeof(reachSHA1Salt));
      *(uint32_t*)(working + sizeof(reachSHA1Salt)) = cobb::to_big_endian(uint32_t(size));
      memcpy(working + bufsize - size, buffer + offset_before_hashable, size);
      hasher.transform(working, bufsize);
      free(working);
      working = nullptr;
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
   return true;
}
void ReachBlockMPVR::write(cobb::bit_or_byte_writer& writer) const noexcept {
   auto& bytes = writer.bytes;
   auto& bits  = writer.bits;
   auto& wd    = this->writeData;

   uint32_t offset_of_hash;
   uint32_t offset_before_hashable;
   uint32_t offset_after_hashable;
   uint32_t offset_of_hashable_length;
   this->header.write(bytes);
   wd.offset_of_hash = bytes.get_bytespan();
   bytes.write(this->hashSHA1);
   bytes.pad(4); // four unused bytes
   wd.offset_of_hashable_length = bytes.get_bytespan();
   bytes.pad(4); // handled in (write_last_minute_fixup)
   wd.offset_before_hashable = bytes.get_bytespan();
   //
   writer.synchronize();
   //
   this->type.write(bits);
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
   wd.offset_after_hashable = bits.get_bytespan();
   //
   writer.synchronize();
   //
   bytes.pad_to_bytepos(this->header.write_end());
   this->header.write_postprocess(bytes);
   //
   writer.synchronize();
}
void ReachBlockMPVR::write_last_minute_fixup(cobb::bit_or_byte_writer& writer) const noexcept {
   auto& wd    = this->writeData;
   auto& bytes = writer.bytes;
   writer.synchronize();
   //
   this->variantHeader.write_last_minute_fixup(writer.bits);
   {  // SHA-1 hash
      auto hasher = cobb::sha1();
      uint32_t size = wd.offset_after_hashable - wd.offset_before_hashable;
      bytes.write_to_offset(wd.offset_of_hashable_length, size, cobb::endian::little);
      uint32_t bufsize  = size + sizeof(reachSHA1Salt) + 4;
      auto     buffer   = (const uint8_t*)bytes.data();
      auto     buffer32 = (const uint32_t*)buffer;
      uint8_t* working  = (uint8_t*)malloc(bufsize);
      memcpy(working, reachSHA1Salt, sizeof(reachSHA1Salt));
      *(uint32_t*)(working + sizeof(reachSHA1Salt)) = cobb::to_big_endian(size);
      memcpy(working + bufsize - size, buffer + 0x318, size);
      hasher.transform(working, bufsize);
      free(working);
      //
      bytes.write_to_offset(wd.offset_of_hash + 0x00, hasher.hash[0], cobb::endian::big);
      bytes.write_to_offset(wd.offset_of_hash + 0x04, hasher.hash[1], cobb::endian::big);
      bytes.write_to_offset(wd.offset_of_hash + 0x08, hasher.hash[2], cobb::endian::big);
      bytes.write_to_offset(wd.offset_of_hash + 0x0C, hasher.hash[3], cobb::endian::big);
      bytes.write_to_offset(wd.offset_of_hash + 0x10, hasher.hash[4], cobb::endian::big);
   }
   //
   writer.synchronize();
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
   // Bungie only hashes the portion of MPVR that the game variant actually uses. This bytecount is a 
   // big-endian uint32_t located 0x4 bytes after the hash (i.e. offset 0x314 in the file). We need to 
   // read that, and then only run the hashing algorithm on that many bytes after that length value.
   //
   uint32_t size = cobb::from_big_endian(buffer32[0x314 / 4]);
   printf("File's existing hash (%04X bytes of data):\n", size);
   for (int i = 0; i < 5; i++)
      printf("   %08X\n", buffer32[0x2FC / 4 + i]);
   printf("\n");
   //
   auto hasher = cobb::sha1();
   {
      uint32_t bufsize  = size + sizeof(reachSHA1Salt) + 4;
      auto     buffer   = (const uint8_t*)file.data();
      auto     buffer32 = (const uint32_t*)buffer;
      uint8_t* working  = (uint8_t*)malloc(bufsize);
      memcpy(working, reachSHA1Salt, sizeof(reachSHA1Salt));
      *(uint32_t*)(working + sizeof(reachSHA1Salt)) = cobb::to_big_endian(size);
      memcpy(working + bufsize - size, buffer + 0x318, size);
      hasher.transform(working, bufsize);
      free(working);
   }
   printf("Our hash:\n");
   for (int i = 0; i < 5; i++)
      printf("   %08X\n", hasher.hash[i]);
   printf("Test done.\n");
}