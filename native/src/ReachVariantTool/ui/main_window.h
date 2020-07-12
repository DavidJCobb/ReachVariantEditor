#pragma once
#include <cstdint>

#include <QtWidgets/QMainWindow>
#include "ui_main_window.h"

class ReachCGRespawnOptions;
class ReachFirefightWaveTraits;
class ReachLoadoutPalette;
class ReachMegaloPlayerTraits;
class ReachPlayerTraits;
class ReachTeamData;

class ReachVariantTool : public QMainWindow {
   Q_OBJECT
   //
   public:
      ReachVariantTool(QWidget* parent = Q_NULLPTR); // needs to be public for Qt? but do not call; use the static getter
      //
      static ReachVariantTool& get(); // done differently because the usual "static singleton getter" approach apparently causes Qt to crash on exit if applied to the main window
      //
   public:
      void getDefaultLoadDirectory(QString& out) const noexcept;
      void getDefaultSaveDirectory(QString& out) const noexcept;
      //
   private slots:
      //
   private:
      void _saveFileImpl(bool saveAs);
      //
      void openFile(); // open-file dialog
      void openFile(QString fileName = "");
      //
      void saveFile()   { this->_saveFileImpl(false); }
      void saveFileAs() { this->_saveFileImpl(true); }
      void onSelectedPageChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
      //
      void regenerateNavigation();
      //
      void switchToFFWaveTraits(ReachFirefightWaveTraits*);
      void switchToLoadoutPalette(ReachLoadoutPalette*);
      void switchToPlayerTraits(ReachPlayerTraits*);
      void switchToRespawnOptions(ReachCGRespawnOptions*);
      //
      void refreshWindowTitle();
      //
      void dragEnterEvent(QDragEnterEvent* event); // override
      void dropEvent(QDropEvent* event); // override
      //
      QTreeWidgetItem* getNavItemForScriptTraits(ReachMegaloPlayerTraits*, int32_t index = -1);
      //
   private:
      Ui::ReachVariantToolClass ui;
      //
      QString dirSavedVariants;
      QString dirBuiltInVariants;
      QString dirMatchmakingVariants;
      QString lastFileDirectory;
};
