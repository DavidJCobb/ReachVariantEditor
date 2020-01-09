#pragma once
#include <QObject>
#include "game_variants/base.h"
#include "game_variants/components/loadouts.h"
#include "game_variants/components/player_traits.h"
#include "game_variants/components/teams.h"
#include "game_variants/types/multiplayer.h"

class ReachMegaloOption;

class ReachEditorState : public QObject {
   Q_OBJECT
   public:
      static ReachEditorState& get() {
         static ReachEditorState instance;
         return instance;
      }
      //
      ReachEditorState() {}
      ReachEditorState(const ReachEditorState& other) = delete; // no copy
      //
   private:
      std::wstring         currentFile;
      GameVariant*         currentVariant = nullptr;
      GameVariant*         currentVariantClone = nullptr; /// checks for unsaved changes aren't yet implemented but when that becomes feasible we can turn this back on
      ReachPlayerTraits*   currentTraits  = nullptr;
      ReachLoadoutPalette* currentLoadoutPalette = nullptr;
      int8_t               currentMPTeam = -1;
      //
   signals:
      void variantAbandoned(GameVariant* variant); // the game variant is deleted after this is emitted
      void variantAcquired(GameVariant* variant);
      void variantFilePathChanged(const wchar_t* path);
      //
      void switchedLoadoutPalette(ReachLoadoutPalette* which);
      void switchedMultiplayerTeam(GameVariant*, int8_t index, ReachTeamData*);
      void switchedPlayerTraits(ReachPlayerTraits* traits);
      //
      // These are fired by the UI so that different windows/panels/etc. can synch with each other:
      void scriptOptionModified(ReachMegaloOption*); // a single option has changed (does not include reordering, creation, or deletion)
      void scriptOptionsModified(); // options have been reordered, created, or deleted
      void stringModified(uint32_t index); // a single string has changed (does not include reordering, creation, or deletion)
      void stringTableModified(); // strings have been reordered, created, or deleted
      void teamColorModified(ReachTeamData*); // exists to help with team editing UI, i.e. it's fired from outside, not from inside
      //
   public slots:
      void abandonVariant() noexcept;
      void setCurrentLoadoutPalette(ReachLoadoutPalette* which) noexcept; /// sets us as editing a loadout palette
      void setCurrentMultiplayerTeam(int8_t index) noexcept; /// sets us as editing details for a specific team
      void setCurrentPlayerTraits(ReachPlayerTraits* which) noexcept; /// sets us as editing a set of player traits
      void setVariantFilePath(const wchar_t* path) noexcept; /// provided for you to call after "Save As"
      void takeVariant(GameVariant* other, const wchar_t* path) noexcept;
      //
   public: // getters
      inline ReachLoadoutPalette* loadoutPalette() noexcept { return this->currentLoadoutPalette; }
      GameVariantDataMultiplayer* multiplayerData() noexcept;
      ReachTeamData* multiplayerTeam() noexcept;
      inline int8_t multiplayerTeamIndex() noexcept { return this->currentMPTeam; }
      inline ReachPlayerTraits* playerTraits() noexcept { return this->currentTraits; }
      inline GameVariant*   variant()         noexcept { return this->currentVariant; }
      inline const wchar_t* variantFilePath() noexcept { return this->currentFile.c_str(); }
};