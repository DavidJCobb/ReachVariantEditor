#include "base.h"
#include "../helpers/bitstream.h"

bool BlamHeader::read(cobb::bitstream& stream) noexcept {
   this->header.read(stream);
   stream.read(this->data.unk0C);
   stream.read(this->data.unk0E);
   stream.read(this->data.unk2E);
   return true;
}

bool GameVariantHeader::read(cobb::bitstream& stream) noexcept {
   this->build.major = 0; // not in mpvr
   this->build.minor = 0; // not in mpvr
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

bool ReachBlockMPVR::read(cobb::bitstream& stream) noexcept {
   this->header.read(stream);
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
      std::vector<MegaloCondition> conditions;
      int count;
      //
      count = stream.read_bits(cobb::bitcount(reach::megalo::max_conditions)); // 10 bits
      conditions.resize(count);
      for (int i = 0; i < count; i++) {
         printf("Reading condition %d of %d...\n", i, count);
         if (!conditions[i].read(stream))
            break;
      }
      #if _DEBUG
         __debugbreak();
      #endif

      // all conditions (10-bit count)
      // all actions    (11-bit count)
      // all triggers
         // execution mode
         // trigger type
         // switch (execution mode)
            // case object filter:
               // object filter index
            // case candy spawner:
               // game object type (1 bit)
               // object filter index
         // first condition index (condition index bitlength; no presence bit or offset)
         // condition count       (condition count bitlength)
         // first action index (action index bitlength; no presence bit or offset)
         // action count       (action count bitlength)
      // game statistics
      // global variable declarations
      // player variable declarations
      // object variable declarations
      // team   variable declarations
      // HUD widget definitions
      // trigger entry points
      // object type references (bitset)
      // object filters (i.e. Forge labels?)

      // NOTES:
      //  - GameObjectFilters are Halo 4 only

      //
      // TODO: retain data and organize it (i.e. go from struct-of-arrays to array-of-structs, 
      // with triggers actually containing their conditions and actions).
      //
   }
   if (this->encodingVersion >= 0x6B) // TU1 encoding version (stock is 0x6A)
      this->titleUpdateData.read(stream);

   return true;
}