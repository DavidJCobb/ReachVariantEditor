#include "multiplayer.h"
#include "../../helpers/stream.h"
#include "../../helpers/bitwriter.h"

#include "../components/megalo/actions.h"
#include "../components/megalo/conditions.h"
#include "../components/megalo/limits.h"

#include "../../formats/sha1.h"
#include "../../helpers/sha1.h"

#include "../io_process.h"
#include "../errors.h"
#include "../warnings.h"

ReachMPSizeData::ReachMPSizeData() {
   {  // Header information
      auto& bitcount = this->bits.header;
      bitcount = 0;
      // ReachBlockMPVR members:
      bitcount += 0x0C * 8; // size of a block header
      bitcount += 0x14 * 8; // size of SHA-1 hash
      bitcount += 0x08 * 8; // size of four padding bytes and four-byte length of hashable content
      bitcount += decltype(ReachBlockMPVR::type)::bitcount;
      // GameVariantDataMultiplayer members:
      bitcount += cobb::bits_in<decltype(GameVariantDataMultiplayer::encodingVersion)>;
      bitcount += cobb::bits_in<decltype(GameVariantDataMultiplayer::engineVersion)>;
      bitcount += MegaloStringIndexOptional::bitcount; // mp->genericName
      bitcount += decltype(GameVariantDataMultiplayer::isBuiltIn)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::engineIcon)::max_bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::engineCategory)::max_bitcount;
      bitcount += cobb::bitcount(Megalo::Limits::max_script_traits);  // count
      bitcount += cobb::bitcount(Megalo::Limits::max_script_options); // count
      bitcount += cobb::bitcount(Megalo::Limits::max_script_stats);   // count
      bitcount += cobb::bitcount(Megalo::Limits::max_script_labels);  // count
      bitcount += cobb::bitcount(Megalo::Limits::max_script_widgets); // count
      bitcount += ReachGameVariantUsedMPObjectTypeList::flag_count;
      bitcount += decltype(GameVariantDataMultiplayer::variantHeader)::bitcount();
   }
   {  // Custom Game options
      auto& bitcount = this->bits.cg_options;
      bitcount = 0;
      //
      bitcount += decltype(GameVariantDataMultiplayer::options.misc.flags)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.misc.timeLimit)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.misc.roundLimit)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.misc.roundsToWin)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.misc.suddenDeathTime)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.misc.gracePeriod)::bitcount;
      //
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.flags)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.livesPerRound)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.teamLivesPerRound)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.respawnTime)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.suicidePenalty)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.betrayalPenalty)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.respawnGrowth)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.loadoutCamTime)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.traitsDuration)::bitcount;
      bitcount += ReachPlayerTraits::bitcount(); // respawn traits
      //
      bitcount += decltype(GameVariantDataMultiplayer::options.social.observers)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.social.teamChanges)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.social.flags)::bitcount;
      //
      bitcount += decltype(GameVariantDataMultiplayer::options.map.flags)::bitcount;
      bitcount += ReachPlayerTraits::bitcount(); // base player traits
      bitcount += decltype(GameVariantDataMultiplayer::options.map.weaponSet)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.map.vehicleSet)::bitcount;
      bitcount += ReachPlayerTraits::bitcount() * 3; // powerup traits
      bitcount += decltype(GameVariantDataMultiplayer::options.map.powerups.red.duration)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.map.powerups.blue.duration)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.map.powerups.yellow.duration)::bitcount;
      //
      bitcount += decltype(GameVariantDataMultiplayer::options.team.scoring)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.team.species)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.team.designatorSwitchType)::bitcount;
      //
      bitcount += decltype(GameVariantDataMultiplayer::options.loadouts.flags)::bitcount;
      //
      bitcount += decltype(GameVariantDataMultiplayer::scoreToWin)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::fireteamsEnabled)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::symmetric)::bitcount;
      //
      bitcount += ReachLoadoutPalette::bitcount() * std::tuple_size<decltype(GameVariantDataMultiplayer::options.loadouts.palettes)>::value;
   }
   this->bits.option_toggles = (ReachGameVariantEngineOptionToggles::dword_count + ReachGameVariantMegaloOptionToggles::dword_count) * 32 * 2;
   this->bits.rating_params  = ReachPlayerRatingParams::bitcount();
   this->bits.title_update_1 = ReachGameVariantTU1Options::bitcount();
}
void ReachMPSizeData::update_from(GameVariant& variant) {
   auto mp = variant.get_multiplayer_data();
   if (mp)
      this->update_from(*mp);
}
void ReachMPSizeData::update_from(GameVariantDataMultiplayer& mp) {
   #pragma region header strings
      this->bits.header_strings  = mp.localizedName.get_size_to_save();
      this->bits.header_strings += mp.localizedDesc.get_size_to_save();
      this->bits.header_strings += mp.localizedCategory.get_size_to_save();
   #pragma endregion
   #pragma region team configuration
   {
      auto& bitcount = this->bits.team_config;
      auto& list     = mp.options.team.teams;
      bitcount = 0;
      for (int i = 0; i < std::extent<decltype(mp.options.team.teams)>::value; i++) // decltype(any_reference) is broken
         bitcount += list[i].bitcount();
   }
   #pragma endregion
   this->bits.script_traits = ReachMegaloPlayerTraits::bitcount() * mp.scriptData.traits.size(); // list count is included in "header information"
   #pragma region script options
   {
      auto& bitcount = this->bits.script_options;
      auto& list     = mp.scriptData.options;
      bitcount = 0; // list count is included in "header information"
      for (auto& item : list)
         bitcount += item.bitcount();
   }
   #pragma endregion
   this->bits.script_stats   = ReachMegaloGameStat::bitcount() * mp.scriptContent.stats.size(); // list count is included in "header information"
   this->bits.script_widgets = Megalo::HUDWidgetDeclaration::bitcount() * mp.scriptContent.widgets.size(); // list count is included in "header information"
   #pragma region Forge labels
   {
      auto& bitcount = this->bits.forge_labels;
      auto& list     = mp.scriptContent.forgeLabels;
      bitcount = 0; // list count is included in "header information"
      for (auto& item : list)
         bitcount += item.bitcount();
   }
   #pragma endregion
   this->bits.script_strings = mp.scriptData.strings.get_size_to_save();
   this->bits.map_perms      = mp.mapPermissions.bitcount();
   //
   this->update_script_from(mp);
}
void ReachMPSizeData::update_script_from(GameVariantDataMultiplayer& mp) {
   auto& bitcount = this->bits.script_content;
   //
   bitcount  = cobb::bitcount(Megalo::Limits::max_conditions); // count
   bitcount += cobb::bitcount(Megalo::Limits::max_actions);    // count
   bitcount += cobb::bitcount(Megalo::Limits::max_triggers);   // count
   //
   cobb::bit_or_byte_writer writer;
   auto& list = mp.scriptContent.triggers;
   auto& bits = writer.bits;
   //
   this->counts.triggers   = 0;
   this->counts.conditions = 0;
   this->counts.actions    = 0;
   for (auto& trigger : list) {
      ++this->counts.triggers;
      //
      trigger.write(bits);
      for (auto opcode : trigger.opcodes) {
         opcode->write(bits);
         //
         auto action = dynamic_cast<Megalo::Action*>(opcode);
         if (action)
            ++this->counts.actions;
         else
            ++this->counts.conditions;
      }
   }
   mp.scriptContent.entryPoints.write(bits);
   {  // Script variable declarations
      auto& vars = mp.scriptContent.variables;
      vars.global.write(bits);
      vars.player.write(bits);
      vars.object.write(bits);
      vars.team.write(bits);
   }
   bitcount += writer.bits.get_bitpos();
}
uint32_t ReachMPSizeData::total_bits() const noexcept {
   uint32_t bitcount = 0;
   bitcount += this->bits.header;
   bitcount += this->bits.header_strings;
   bitcount += this->bits.cg_options;
   bitcount += this->bits.team_config;
   bitcount += this->bits.script_traits;
   bitcount += this->bits.script_options;
   bitcount += this->bits.script_strings;
   bitcount += this->bits.option_toggles;
   bitcount += this->bits.rating_params;
   bitcount += this->bits.map_perms;
   bitcount += this->bits.script_content;
   bitcount += this->bits.script_stats;
   bitcount += this->bits.script_widgets;
   bitcount += this->bits.forge_labels;
   bitcount += this->bits.title_update_1;
   return bitcount;
}

bool GameVariantDataMultiplayer::receive_editor_data(RVTEditorBlock::subrecord* subrecord) noexcept {
   if (!subrecord)
      return false;
   switch (subrecord->signature) {
      case RVTEditorBlock::signature_megalo_string_table:
         this->scriptData.strings.read_fallback_data(subrecord->data);
         delete subrecord;
         return true;
      case RVTEditorBlock::signature_megalo_script:
         this->scriptContent.triggers.clear(); // owns its contents; deletes them automatically
         {
            auto& error_report = GameEngineVariantLoadError::get();
            cobb::reader reader(subrecord->data.data(), subrecord->data.size());
            this->_read_script_code(reader.bits);
            if (!this->scriptContent.entryPoints.read(reader.bits))
               return false;
         }
         delete subrecord;
         return true;
   }
   return false;
}
bool GameVariantDataMultiplayer::_read_script_code(cobb::ibitreader& stream) noexcept {
   this->scriptContent.triggers.clear(); // owns its contents; deletes them automatically
   //
   auto& error_report = GameEngineVariantLoadError::get();
   //
   int   count;
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
   return true;
}

namespace {
   bool _error_check_count(int count, int maximum, GameEngineVariantLoadError::load_failure_detail detail) {
      if (count > maximum) {
         auto& error_report = GameEngineVariantLoadError::get();
         //
         error_report.state  = GameEngineVariantLoadError::load_state::failure;
         error_report.reason = GameEngineVariantLoadError::load_failure_reason::invalid_mpvr_data;
         error_report.detail = detail;
         error_report.extra[0] = count;
         error_report.extra[1] = maximum;
         return false;
      }
      return true;
   }
}
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
   if (this->isBuiltIn) {
      //
      // If this flag is true, then MCC ignores the title and description of the game variant, instead using 
      // MCC-side localized strings whose keys are derived from the English text of the string pointed to by 
      // GameVariantDataMultiplayer::genericName i.e. "$hr_gvar_[genericName]" e.g. "$hr_gvar_Halo_Chess+".
      //
      this->isBuiltIn = false;
      printf("Loaded a game variant with isBuiltIn set to (true). There is no value in keeping that, so forcing it to false.");
   }
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
      if (!_error_check_count(count, Megalo::Limits::max_script_traits, GameEngineVariantLoadError::load_failure_detail::too_many_script_traits))
         return false;
      for (int i = 0; i < count; i++) {
         auto& traits = t[i];
         traits.read(stream);
      }
      //
      count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_script_options));
      if (!_error_check_count(count, Megalo::Limits::max_script_options, GameEngineVariantLoadError::load_failure_detail::too_many_script_options))
         return false;
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
      this->_read_script_code(stream);
      //
      int count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_script_stats));
      if (!_error_check_count(count, Megalo::Limits::max_script_stats, GameEngineVariantLoadError::load_failure_detail::too_many_script_stats))
         return false;
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
         if (!_error_check_count(count, Megalo::Limits::max_script_widgets, GameEngineVariantLoadError::load_failure_detail::too_many_script_widgets))
            return false;
         for (size_t i = 0; i < count; i++) {
            this->scriptContent.widgets[i].read(stream);
         }
      }
      if (!this->scriptContent.entryPoints.read(stream))
         return false;
      this->scriptContent.usedMPObjectTypes.read(stream);
      //
      {  // Forge labels
         size_t count = stream.read_bits(cobb::bitcount(Megalo::Limits::max_script_labels));
         if (!_error_check_count(count, Megalo::Limits::max_script_labels, GameEngineVariantLoadError::load_failure_detail::too_many_forge_labels))
            return false;
         for (size_t i = 0; i < count; i++) {
            this->scriptContent.forgeLabels[i].read(stream, *this);
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
void GameVariantDataMultiplayer::write(GameVariantSaveProcess& save_process) noexcept {
   auto& writer = save_process.writer;
   writer.synchronize();
   auto& bits   = writer.bits;
   //
   this->encodingVersion = encoding_version_tu1; // upgrade, so that TU1 settings are always saved
   //
   auto predicted_size = this->get_size_data();
   bool strings_in_ex  = false;
   bool script_in_ex   = false;
   {
      auto total = predicted_size.total_bits();
      if (total > predicted_size.bits.maximum) {
         strings_in_ex = true;
      }
   }
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
      //
      if (!strings_in_ex) {
         bool success = sd.strings.write(bits);
         strings_in_ex = !success;
      }
      if (strings_in_ex) {
         save_process.set_flag(GameVariantSaveProcess::flag::uses_xrvt_strings);
         //
         auto prior = bits.get_bitpos();
         sd.strings.write_placeholder(bits);
         auto after = bits.get_bitpos();
         predicted_size.bits.script_strings = after - prior;
         //
         auto subrecord = save_process.create_subrecord(RVTEditorBlock::signature_megalo_string_table);
         subrecord->version = 0;
         sd.strings.write_fallback_data(subrecord->data);
         subrecord->size = subrecord->data.size();
         //
         if (predicted_size.total_bits() > predicted_size.bits.maximum) {
            script_in_ex = true;
         }
      }
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
         if (script_in_ex) {
            save_process.set_flag(GameVariantSaveProcess::flag::uses_xrvt_scripts);
            //
            // The script cannot fit inside of the MPVR block. Write an empty script as a placeholder, and 
            // store the true script content in an xRVT subrecord.
            //
            auto prior = bits.get_bitpos();
            bits.write(0, cobb::bitcount(Megalo::Limits::max_conditions)); // 10 bits
            bits.write(0, cobb::bitcount(Megalo::Limits::max_actions)); // 11 bits
            bits.write(0, cobb::bitcount(Megalo::Limits::max_triggers));
            auto after = bits.get_bitpos();
            predicted_size.bits.script_content = after - prior;
            //
            auto subrecord = save_process.create_subrecord(RVTEditorBlock::signature_megalo_script);
            subrecord->version = 0;
            //
            cobb::bitwriter temp;
            temp.write(allConditions.size(), cobb::bitcount(Megalo::Limits::max_conditions)); // 10 bits
            for (auto opcode : allConditions)
               opcode->write(temp);
            //
            temp.write(allActions.size(), cobb::bitcount(Megalo::Limits::max_actions)); // 11 bits
            for (auto opcode : allActions)
               opcode->write(temp);
            //
            temp.write(content.triggers.size(), cobb::bitcount(Megalo::Limits::max_triggers));
            for (auto& trigger : content.triggers)
               trigger.write(temp);
            //
            content.entryPoints.write(temp);
            //
            auto& buffer = subrecord->data;
            auto  size   = temp.get_bytespan();
            buffer.allocate(size);
            memcpy(buffer.data(), temp.data(), size);
            subrecord->size = size;
         } else {
            //
            // Normal save code for a script that can fit inside of the MPVR block.
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
      if (script_in_ex)
         content.entryPoints.write_placeholder(bits);
      else
         content.entryPoints.write(bits);
      //
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
void GameVariantDataMultiplayer::write_last_minute_fixup(GameVariantSaveProcess& save_process) const noexcept {
   auto& writer = save_process.writer;
   auto& bytes  = writer.bytes;
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
         if (!fails)
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

ReachMegaloOption* GameVariantDataMultiplayer::create_script_option() {
   auto& list = this->scriptData.options;
   if (list.size() >= list.max_count)
      return nullptr;
   auto option = list.emplace_back();
   option->is_defined = true;
   auto value = option->add_value(); // enum-options must have at least one value
   {  // If possible, default the new option's name and description to an empty string; ditto for its value
      auto str = this->scriptData.strings.get_empty_entry();
      if (str) {
         option->name = str;
         option->desc = str;
         value->name = str;
         value->desc = str;
      }
   }
   auto index = option->index;
   if (index >= 0) { // should always be true
      this->optionToggles.megalo.disabled.bits.reset(index);
      this->optionToggles.megalo.hidden.bits.reset(index);
   }
   return option;
}
void GameVariantDataMultiplayer::delete_script_option(ReachMegaloOption* option) {
   if (!option || option->get_refcount())
      return;
   auto& list  = this->scriptData.options;
   auto  index = list.index_of(option);
   if (index < 0)
      return;
   list.erase(index);
   //
   auto& disabled = this->optionToggles.megalo.disabled.bits;
   auto& hidden   = this->optionToggles.megalo.hidden.bits;
   disabled.remove(index);
   hidden.remove(index);
}
bool GameVariantDataMultiplayer::swap_script_options(int8_t index_a, int8_t index_b) {
   if (index_a < 0 || index_b < 0)
      return false;
   auto& list = this->scriptData.options;
   auto  size = list.size();
   if (index_a >= size || index_b >= size)
      return false;
   list.swap_items(index_a, index_b);
   this->optionToggles.megalo.disabled.bits.swap_bits(index_a, index_b);
   this->optionToggles.megalo.hidden.bits.swap_bits(index_a, index_b);
   return true;
}