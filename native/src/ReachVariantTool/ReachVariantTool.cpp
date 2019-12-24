#include "ReachVariantTool.h"
#include <cassert>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QTreeWidget>
#include "editor_state.h"
#include "game_variants/base.h"
#include "helpers/pointer_to_member.h"
#include "helpers/stream.h"

ReachVariantTool::ReachVariantTool(QWidget *parent) : QMainWindow(parent) {
   ui.setupUi(this);
   //
   QObject::connect(this->ui.actionOpen,   &QAction::triggered, this, &ReachVariantTool::openFile);
   QObject::connect(this->ui.actionSaveAs, &QAction::triggered, this, &ReachVariantTool::saveFile);
   //
   QObject::connect(this->ui.MainTreeview, &QTreeWidget::itemSelectionChanged, this, &ReachVariantTool::onSelectedPageChanged);
   //
   {
      //
      // This would be about a thousand times cleaner if we could use pointers-to-members-of-members, 
      // but the language doesn't support that even though it easily could. There are workarounds, but 
      // in C++17 they aren't constexpr and therefore can't be used as template parameters. (My plan 
      // was to create structs that act as decorators, to accomplish this stuff.) So, macros.
      //
      #define reach_main_window_setup_spinbox(w, field) \
         { \
            QSpinBox* widget = w; \
            QObject::connect(widget, QOverload<int>::of(&QSpinBox::valueChanged), [widget](int value) { \
               auto variant = ReachEditorState::get().currentVariant; \
               if (!variant) \
                  return; \
               variant->##field = value; \
            }); \
         };
      #define reach_main_window_setup_flag_checkbox(w, field, mask) \
         { \
            QCheckBox* widget = w; \
            QObject::connect(widget, &QCheckBox::stateChanged, [widget](int state) { \
               auto variant = ReachEditorState::get().currentVariant; \
               if (!variant) \
                  return; \
               if (widget->isChecked()) \
                  variant->##field |= mask ; \
               else \
                  variant->##field &= ~ mask ; \
            }); \
         };
      { // General
         reach_main_window_setup_flag_checkbox(this->ui.optionsGeneralTeamsEnabled,          multiplayer.options.misc.flags, 1);
         reach_main_window_setup_flag_checkbox(this->ui.optionsGeneralNewRoundResetsPlayers, multiplayer.options.misc.flags, 2);
         reach_main_window_setup_flag_checkbox(this->ui.optionsGeneralNewRoundResetsMap,     multiplayer.options.misc.flags, 4);
         reach_main_window_setup_flag_checkbox(this->ui.optionsGeneralFlag3,                 multiplayer.options.misc.flags, 8);
         reach_main_window_setup_spinbox(this->ui.optionsGeneralRoundTimeLimit,  multiplayer.options.misc.timeLimit);
         reach_main_window_setup_spinbox(this->ui.optionsGeneralRoundLimit,      multiplayer.options.misc.roundLimit);
         reach_main_window_setup_spinbox(this->ui.optionsGeneralRoundsToWin,     multiplayer.options.misc.roundsToWin);
         reach_main_window_setup_spinbox(this->ui.optionsGeneralSuddenDeathTime, multiplayer.options.misc.suddenDeathTime);
         reach_main_window_setup_spinbox(this->ui.optionsGeneralGracePeriod,     multiplayer.options.misc.gracePeriod);
      }
      {  // Respawn
         reach_main_window_setup_flag_checkbox(this->ui.optionsRespawnSyncWithTeam,   multiplayer.options.respawn.flags, 1);
         reach_main_window_setup_flag_checkbox(this->ui.optionsRespawnFlag1,          multiplayer.options.respawn.flags, 2);
         reach_main_window_setup_flag_checkbox(this->ui.optionsRespawnFlag2,          multiplayer.options.respawn.flags, 4);
         reach_main_window_setup_flag_checkbox(this->ui.optionsRespawnRespawnOnKills, multiplayer.options.respawn.flags, 8);
         reach_main_window_setup_spinbox(this->ui.optionsRespawnLivesPerRound,     multiplayer.options.respawn.livesPerRound);
         reach_main_window_setup_spinbox(this->ui.optionsRespawnTeamLivesPerRound, multiplayer.options.respawn.teamLivesPerRound);
         reach_main_window_setup_spinbox(this->ui.optionsRespawnRespawnTime,       multiplayer.options.respawn.respawnTime);
         reach_main_window_setup_spinbox(this->ui.optionsRespawnSuicidePenalty,    multiplayer.options.respawn.suicidePenalty);
         reach_main_window_setup_spinbox(this->ui.optionsRespawnBetrayalPenalty,   multiplayer.options.respawn.betrayalPenalty);
         reach_main_window_setup_spinbox(this->ui.optionsRespawnRespawnGrowth,     multiplayer.options.respawn.respawnGrowth);
         reach_main_window_setup_spinbox(this->ui.optionsRespawnLoadoutCamTime,    multiplayer.options.respawn.loadoutCamTime);
         reach_main_window_setup_spinbox(this->ui.optionsRespawnTraitsDuration,    multiplayer.options.respawn.traitsDuration);
      }
      //
      #undef reach_main_window_setup_spinbox
      #undef reach_main_window_setup_flag_checkbox
   }
}

void ReachVariantTool::openFile() {
   auto& editor = ReachEditorState::get();
   //
   // TODO: warn on unsaved changes
   //
   QString fileName = QFileDialog::getOpenFileName(this, tr("Open Game Variant"), "", tr("Game Variant (*.bin);;All Files (*)"));
   if (fileName.isEmpty())
      return;
   std::wstring s = fileName.toStdWString();
   auto file    = cobb::mapped_file(s.c_str());
   s.clear();
   auto variant = new GameVariant();
   if (!variant->read(file)) {
      QMessageBox::information(this, tr("Unable to open file"), tr("Failed to read the game variant data."));
      return;
   }
   editor.take_game_variant(variant);
   this->refreshWidgetsFromVariant();
}
void ReachVariantTool::saveFile() {
   auto& editor = ReachEditorState::get();
   if (!editor.currentVariant) {
      QMessageBox::information(this, tr("No game variant is open"), tr("We do not currently support creating game variants from scratch. Open a variant and then you can save an edited copy of it."));
      return;
   }
   //
   QString fileName = QFileDialog::getSaveFileName(this, tr("Save Game Variant"), "", tr("Game Variant (*.bin);;All Files (*)"));
   if (fileName.isEmpty())
      return;
   QFile file(fileName);
   if (!file.open(QIODevice::WriteOnly)) {
      QMessageBox::information(this, tr("Unable to open file for writing"), file.errorString());
      return;
   }
   QDataStream out(&file);
   out.setVersion(QDataStream::Qt_4_5);
   //
   cobb::bit_or_byte_writer writer;
   editor.currentVariant->write(writer);
   out.writeRawData((const char*)writer.bytes.data(), writer.bytes.get_bytespan());
}
void ReachVariantTool::onSelectedPageChanged() {
   auto widget     = this->ui.MainTreeview;
   auto selections = widget->selectedItems();
   if (!selections.size())
      return;
   auto sel   = selections[0];
   auto text  = sel->text(0);
   auto stack = this->ui.MainContentView;
   if (text == tr("Metadata", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageGameVariantHeader);
      return;
   }
   if (text == tr("General Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsGeneral);
      return;
   }
   if (text == tr("Respawn Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsRespawn);
      return;
   }
   //
   // TODO: add other panes
   //
}
void ReachVariantTool::refreshWidgetsFromVariant() {
   auto& editor = ReachEditorState::get();
   auto variant = editor.currentVariant;
   if (!variant)
      return;
   this->isUpdatingFromVariant = true;
   {  // Metadata
      this->ui.headerName->setText(QString::fromWCharArray(variant->multiplayer.variantHeader.title));
      this->ui.headerDesc->setPlainText(QString::fromWCharArray(variant->multiplayer.variantHeader.description));
      this->ui.authorGamertag->setText(QString::fromLatin1(variant->multiplayer.variantHeader.createdBy.author));
      this->ui.editorGamertag->setText(QString::fromLatin1(variant->multiplayer.variantHeader.modifiedBy.author));
   }
   //
   // This would be about a thousand times cleaner if we could use pointers-to-members-of-members, 
   // but the language doesn't support that even though it easily could. There are workarounds, but 
   // in C++17 they aren't constexpr and therefore can't be used as template parameters. (My plan 
   // was to create structs that act as decorators, to accomplish this stuff.) So, macros.
   //
   #define reach_main_window_update_spinbox(w, field) \
      { \
         auto widget = w; \
         const QSignalBlocker blocker(widget); \
         widget->setValue( variant->##field ); \
      };
   #define reach_main_window_update_flag_checkbox(w, field, mask) \
      { \
         auto widget = w; \
         const QSignalBlocker blocker(widget); \
         widget->setChecked(( variant->##field & mask ) != 0); \
      };
   {  // General
      reach_main_window_update_flag_checkbox(this->ui.optionsGeneralTeamsEnabled,          multiplayer.options.misc.flags, 1);
      reach_main_window_update_flag_checkbox(this->ui.optionsGeneralNewRoundResetsPlayers, multiplayer.options.misc.flags, 2);
      reach_main_window_update_flag_checkbox(this->ui.optionsGeneralNewRoundResetsMap,     multiplayer.options.misc.flags, 4);
      reach_main_window_update_flag_checkbox(this->ui.optionsGeneralFlag3,                 multiplayer.options.misc.flags, 8);
      reach_main_window_update_spinbox(this->ui.optionsGeneralRoundTimeLimit,  multiplayer.options.misc.timeLimit);
      reach_main_window_update_spinbox(this->ui.optionsGeneralRoundLimit,      multiplayer.options.misc.roundLimit);
      reach_main_window_update_spinbox(this->ui.optionsGeneralRoundsToWin,     multiplayer.options.misc.roundsToWin);
      reach_main_window_update_spinbox(this->ui.optionsGeneralSuddenDeathTime, multiplayer.options.misc.suddenDeathTime);
      reach_main_window_update_spinbox(this->ui.optionsGeneralGracePeriod,     multiplayer.options.misc.gracePeriod);
   }
   {  // Respawn
      reach_main_window_update_flag_checkbox(this->ui.optionsRespawnSyncWithTeam,   multiplayer.options.respawn.flags, 1);
      reach_main_window_update_flag_checkbox(this->ui.optionsRespawnFlag1,          multiplayer.options.respawn.flags, 2);
      reach_main_window_update_flag_checkbox(this->ui.optionsRespawnFlag2,          multiplayer.options.respawn.flags, 4);
      reach_main_window_update_flag_checkbox(this->ui.optionsRespawnRespawnOnKills, multiplayer.options.respawn.flags, 8);
      reach_main_window_update_spinbox(this->ui.optionsRespawnLivesPerRound,     multiplayer.options.respawn.livesPerRound);
      reach_main_window_update_spinbox(this->ui.optionsRespawnTeamLivesPerRound, multiplayer.options.respawn.teamLivesPerRound);
      reach_main_window_update_spinbox(this->ui.optionsRespawnRespawnTime,       multiplayer.options.respawn.respawnTime);
      reach_main_window_update_spinbox(this->ui.optionsRespawnSuicidePenalty,    multiplayer.options.respawn.suicidePenalty);
      reach_main_window_update_spinbox(this->ui.optionsRespawnBetrayalPenalty,   multiplayer.options.respawn.betrayalPenalty);
      reach_main_window_update_spinbox(this->ui.optionsRespawnRespawnGrowth,     multiplayer.options.respawn.respawnGrowth);
      reach_main_window_update_spinbox(this->ui.optionsRespawnLoadoutCamTime,    multiplayer.options.respawn.loadoutCamTime);
      reach_main_window_update_spinbox(this->ui.optionsRespawnTraitsDuration,    multiplayer.options.respawn.traitsDuration);
   }
   //
   #undef reach_main_window_update_spinbox
   #undef reach_main_window_update_flag_checkbox
   //
   this->isUpdatingFromVariant = false;
}