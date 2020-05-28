#include "editor_state.h"

ReachEditorState::ReachEditorState() {
   QObject::connect(this, &ReachEditorState::stringModified, [this](uint32_t index) {
      auto mp = this->multiplayerData();
      if (!mp)
         return;
      ReachString* str = mp->scriptData.strings.get_entry(index);
      if (!str)
         return;
      for (auto& option : mp->scriptData.options)
         if (option.uses_string(str))
            this->scriptOptionModified(&option);
      for (auto& traits : mp->scriptData.traits)
         if (traits.uses_string(str))
            this->scriptTraitsModified(&traits);
   });
}
void ReachEditorState::abandonVariant() noexcept {
   if (this->currentVariantClone) {
      delete this->currentVariantClone;
      this->currentVariantClone = nullptr;
   }
   if (!this->currentVariant)
      return;
   auto v = this->currentVariant;
   this->currentVariant = nullptr;
   this->currentTraits  = nullptr;
   this->currentLoadoutPalette = nullptr;
   this->currentFile    = L"";
   emit variantAbandoned(v);
   delete v;
}
void ReachEditorState::setCurrentMultiplayerTeam(int8_t index) noexcept {
   this->currentMPTeam = index;
   emit switchedMultiplayerTeam(this->currentVariant, index, this->multiplayerTeam());
}
void ReachEditorState::setCurrentLoadoutPalette(ReachLoadoutPalette* which) noexcept {
   this->currentLoadoutPalette = which;
   emit switchedLoadoutPalette(which);
}
void ReachEditorState::setCurrentPlayerTraits(ReachPlayerTraits* which) noexcept {
   this->currentTraits = which;
   emit switchedPlayerTraits(which);
}
void ReachEditorState::setVariantFilePath(const wchar_t* path) noexcept {
   this->currentFile = path;
   emit variantFilePathChanged(this->currentFile.c_str());
}
void ReachEditorState::takeVariant(GameVariant* other, const wchar_t* path) noexcept {
   this->abandonVariant();
   this->currentVariant = other;
   this->currentFile    = path;
   //
   //if (other)
   //   this->currentVariantClone = other->clone(); // no point until such time as we can implement comparisons quickly
   //
   emit variantAcquired(other);
   emit switchedMultiplayerTeam(this->currentVariant, this->currentMPTeam, this->multiplayerTeam());
}

GameVariantDataMultiplayer* ReachEditorState::multiplayerData() noexcept {
   auto v = this->currentVariant;
   if (!v)
      return nullptr;
   auto& m = v->multiplayer;
   if (m.data) {
      switch (m.data->get_type()) {
         case ReachGameEngine::multiplayer:
         case ReachGameEngine::forge:
            return dynamic_cast<GameVariantDataMultiplayer*>(m.data);
      }
   }
   return nullptr;
}
ReachTeamData* ReachEditorState::multiplayerTeam() noexcept {
   if (this->currentMPTeam == -1)
      return nullptr;
   auto mp = this->multiplayerData();
   if (mp)
      return &(mp->options.team.teams[this->currentMPTeam]);
   return nullptr;
}