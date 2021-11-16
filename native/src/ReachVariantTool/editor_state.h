#pragma once
#include <QObject>

class GameVariant;
class GameVariantDataFirefight;
class GameVariantDataMultiplayer;
class ReachCGRespawnOptions;
class ReachCustomGameOptions;
class ReachFirefightWaveTraits;
class ReachLoadoutPalette;
class ReachMegaloOption;
class ReachMegaloPlayerTraits;
class ReachPlayerTraits;
class ReachTeamData;

class ReachEditorState : public QObject {
   Q_OBJECT
   public:
      static ReachEditorState& get() {
         static ReachEditorState instance;
         return instance;
      }
      //
      ReachEditorState();
      ReachEditorState(const ReachEditorState& other) = delete; // no copy
      //
   private:
      std::wstring           currentFile;
      GameVariant*           currentVariant        = nullptr;
      GameVariant*           currentVariantClone   = nullptr; // checks for unsaved changes aren't yet implemented but when that becomes feasible we can turn this back on
      ReachPlayerTraits*     currentTraits         = nullptr;
      ReachLoadoutPalette*   currentLoadoutPalette = nullptr;
      ReachCGRespawnOptions* currentRespawnOptions = nullptr; // used so that Firefight and Megalo can share code for their respective sets of respawn options
      ReachFirefightWaveTraits* currentFFWaveTraits = nullptr;
      int8_t                 currentMPTeam = -1;
      //
      QString dirSavedVariants;
      QString dirBuiltInVariants;
      QString dirMatchmakingVariants;
      //
   signals:
      void variantAbandoned(GameVariant* variant); // the game variant is deleted after this is emitted
      void variantAcquired(GameVariant* variant);
      void variantFilePathChanged(const wchar_t* path);
      void variantRecompiled(GameVariant* variant);
      //
      void switchedFFWaveTraits(ReachFirefightWaveTraits* wave);
      void switchedLoadoutPalette(ReachLoadoutPalette* which);
      void switchedMultiplayerTeam(GameVariant*, int8_t index, ReachTeamData*);
      void switchedPlayerTraits(ReachPlayerTraits* traits);
      void switchedRespawnOptions(ReachCGRespawnOptions* options);
      //
      // These are fired by the UI so that different windows/panels/etc. can synch with each other:
      void scriptOptionModified(ReachMegaloOption*); // a single option has changed (does not include reordering, creation, or deletion)
      void scriptOptionsModified(); // options have been reordered, created, or deleted
      void scriptTraitsModified(ReachMegaloPlayerTraits*); // if the pointer is non-null, then those traits have had their name or description changed; else, traits have been reordered, created, or deleted
      void stringModified(uint32_t index); // a single string has changed (does not include reordering, creation, or deletion)
      void stringTableModified(); // strings have been reordered, created, or deleted
      void teamColorModified(ReachTeamData*); // exists to help with team editing UI, i.e. it's fired from outside, not from inside
      //
   public slots:
      void abandonVariant() noexcept;
      void setCurrentFFWaveTraits(ReachFirefightWaveTraits* traits) noexcept;
      void setCurrentLoadoutPalette(ReachLoadoutPalette* which) noexcept; /// sets us as editing a loadout palette
      void setCurrentMultiplayerTeam(int8_t index) noexcept; /// sets us as editing details for a specific team
      void setCurrentPlayerTraits(ReachPlayerTraits* which) noexcept; /// sets us as editing a set of player traits
      void setCurrentRespawnOptions(ReachCGRespawnOptions* which) noexcept;
      void setVariantFilePath(const wchar_t* path) noexcept; /// provided for you to call after "Save As"
      void takeVariant(GameVariant* other, const wchar_t* path) noexcept;
      //
      bool saveVariant(QWidget* parent, bool saveAs);
      //
      void openHelp(QWidget* parent, bool folder);
      
   public: // getters
      ReachCustomGameOptions* customGameOptions() noexcept;
      inline ReachFirefightWaveTraits* ffWaveTraits() noexcept { return this->currentFFWaveTraits; }
      GameVariantDataFirefight* firefightData() noexcept;
      inline ReachLoadoutPalette* loadoutPalette() noexcept { return this->currentLoadoutPalette; }
      GameVariantDataMultiplayer* multiplayerData() noexcept;
      ReachTeamData* multiplayerTeam() noexcept;
      inline int8_t multiplayerTeamIndex() noexcept { return this->currentMPTeam; }
      inline ReachPlayerTraits* playerTraits() noexcept { return this->currentTraits; }
      inline ReachCGRespawnOptions* respawnOptions() noexcept { return this->currentRespawnOptions; }
      inline GameVariant*   variant()         noexcept { return this->currentVariant; }
      inline const wchar_t* variantFilePath() noexcept { return this->currentFile.c_str(); }

      void getDefaultLoadDirectory(QString& out) const noexcept;
      void getDefaultSaveDirectory(QString& out) const noexcept;
};