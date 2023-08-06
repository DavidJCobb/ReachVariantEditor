#include "firefight.h"
#include "../io_process.h"
#include "../errors.h"
#include "../warnings.h"

bool GameVariantDataFirefight::read(cobb::reader& reader) noexcept {
   auto& error_report = GameEngineVariantLoadError::get();
   error_report.reset();
   GameEngineVariantLoadWarningLog::get().clear();
   //
   auto& stream = reader.bits;
   //
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
   this->options.read(stream);
   this->scenarioFlags.read(stream);
   this->mccExtensionVersion.read(stream);
   if (this->mccExtensionVersion > 2) {
      auto& log = GameEngineVariantLoadWarningLog::get();
      log.push_back(QString(
         "The Master Chief Collection repurposes a previously unused and hidden Firefight setting, and turns it into a Firefight settings "
         "version number. If the version number is too high, the MCC skips loading all Firefight settings. As of this writing, the current "
         "version is 2. This game variant uses version %1."
      ).arg((int)this->mccExtensionVersion));
   }
   this->waveLimit.read(stream);
   this->unkB.read(stream);
   this->bonusTarget.read(stream);
   this->eliteKillBonus.read(stream);
   this->startingLivesSpartan.read(stream);
   this->startingLivesElite.read(stream);
   this->unkD.read(stream);
   this->maxSpartanExtraLives.read(stream);
   this->generatorCount.read(stream);
   this->baseTraitsSpartan.read(stream);
   this->baseTraitsElite.read(stream);
   this->baseTraitsWave.read(stream);
   for (size_t i = 0; i < std::extent<decltype(customSkulls)>::value; ++i)
      this->customSkulls[i].read(stream);
   this->eliteRespawnOptions.read(stream);
   for (size_t i = 0; i < std::extent<decltype(rounds)>::value; ++i)
      this->rounds[i].read(stream);
   this->bonusWaveDuration.read(stream);
   this->bonusWaveSkulls.read(stream);
   this->bonusWave.read(stream);
   if (this->mccExtensionVersion >= 2) {
      this->mccExtensionFlags.read(stream);
   }
   //
   if (stream.get_overshoot_bits() > 0) {
      error_report.state  = GameEngineVariantLoadError::load_state::failure;
      error_report.reason = GameEngineVariantLoadError::load_failure_reason::block_ended_early;
      return false;
   }
   error_report.state = GameEngineVariantLoadError::load_state::success;
   //
   return true;
}
void GameVariantDataFirefight::write(GameVariantSaveProcess& save_process) noexcept {
   auto& writer = save_process.writer;
   writer.synchronize();
   auto& stream = writer.bits;
   //
   this->variantHeader.write(stream);
   this->isBuiltIn.write(stream);
   this->options.write(stream);
   this->scenarioFlags.write(stream);

   if (this->mccExtensionFlags != 0 && this->mccExtensionVersion < 2) {
      this->mccExtensionVersion = 2;
   }
   this->mccExtensionVersion.write(stream);

   this->waveLimit.write(stream);
   this->unkB.write(stream);
   this->bonusTarget.write(stream);
   this->eliteKillBonus.write(stream);
   this->startingLivesSpartan.write(stream);
   this->startingLivesElite.write(stream);
   this->unkD.write(stream);
   this->maxSpartanExtraLives.write(stream);
   this->generatorCount.write(stream);
   this->baseTraitsSpartan.write(stream);
   this->baseTraitsElite.write(stream);
   this->baseTraitsWave.write(stream);
   for (size_t i = 0; i < std::extent<decltype(customSkulls)>::value; ++i)
      this->customSkulls[i].write(stream);
   this->eliteRespawnOptions.write(stream);
   for (size_t i = 0; i < std::extent<decltype(rounds)>::value; ++i)
      this->rounds[i].write(stream);
   this->bonusWaveDuration.write(stream);
   this->bonusWaveSkulls.write(stream);
   this->bonusWave.write(stream);
   if (this->mccExtensionVersion >= 2) {
      this->mccExtensionFlags.write(stream);
   }
   //
   writer.synchronize();
}
void GameVariantDataFirefight::write_last_minute_fixup(GameVariantSaveProcess& save_process) const noexcept {
   auto& writer = save_process.writer;
   auto& bytes = writer.bytes;
   writer.synchronize();
   this->variantHeader.write_last_minute_fixup(writer.bits);
   writer.synchronize();
}
GameVariantData* GameVariantDataFirefight::clone() const noexcept {
   auto clone = new GameVariantDataFirefight();
   //
   // TODO
   //
   return clone;
}