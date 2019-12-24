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
   {  // Metadata
      QObject::connect(this->ui.headerName, &QLineEdit::textEdited, [](const QString& text) {
         auto variant = ReachEditorState::get().currentVariant;
         if (!variant)
            return;
         std::wstring value = text.toStdWString();
         variant->contentHeader.data.set_title(value.c_str());
         variant->multiplayer.variantHeader.set_title(value.c_str());
      });
      //
      auto desc = this->ui.headerDesc;
      QObject::connect(this->ui.headerDesc, &QPlainTextEdit::textChanged, [desc]() {
         auto text    = desc->toPlainText();
         auto variant = ReachEditorState::get().currentVariant;
         if (!variant)
            return;
         std::wstring value = text.toStdWString();
         variant->contentHeader.data.set_description(value.c_str());
         variant->multiplayer.variantHeader.set_description(value.c_str());
      });
      //
      QObject::connect(this->ui.authorGamertag, &QLineEdit::textEdited, [](const QString& text) {
         auto variant = ReachEditorState::get().currentVariant;
         if (!variant)
            return;
         auto latin = text.toLatin1();
         variant->contentHeader.data.createdBy.set_author_name(latin.constData());
         variant->multiplayer.variantHeader.createdBy.set_author_name(latin.constData());
      });
      QObject::connect(this->ui.editorGamertag, &QLineEdit::textEdited, [](const QString& text) {
         auto variant = ReachEditorState::get().currentVariant;
         if (!variant)
            return;
         auto latin = text.toLatin1();
         variant->contentHeader.data.modifiedBy.set_author_name(latin.constData());
         variant->multiplayer.variantHeader.modifiedBy.set_author_name(latin.constData());
      });
   }
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
      #define reach_main_window_setup_bool_checkbox(w, field) \
         { \
            QCheckBox* widget = w; \
            QObject::connect(widget, &QCheckBox::stateChanged, [widget](int state) { \
               auto variant = ReachEditorState::get().currentVariant; \
               if (!variant) \
                  return; \
               variant->##field = widget->isChecked(); \
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
      {  // Social
         reach_main_window_setup_bool_checkbox(this->ui.optionsSocialObservers, multiplayer.options.social.observers);
         reach_main_window_setup_spinbox(this->ui.optionsSocialTeamChanging, multiplayer.options.social.teamChanges);
         reach_main_window_setup_flag_checkbox(this->ui.optionsSocialFriendlyFire,    multiplayer.options.social.flags, 0x01);
         reach_main_window_setup_flag_checkbox(this->ui.optionsSocialBetrayalBooting, multiplayer.options.social.flags, 0x02);
         reach_main_window_setup_flag_checkbox(this->ui.optionsSocialProximityVoice,  multiplayer.options.social.flags, 0x04);
         reach_main_window_setup_flag_checkbox(this->ui.optionsSocialGlobalVoice,     multiplayer.options.social.flags, 0x08);
         reach_main_window_setup_flag_checkbox(this->ui.optionsSocialDeadPlayerVoice, multiplayer.options.social.flags, 0x10);
      }
      {
         reach_main_window_setup_flag_checkbox(this->ui.optionsMapGrenadesEnabled, multiplayer.options.map.flags, 0x01);
         reach_main_window_setup_flag_checkbox(this->ui.optionsMapShortcutsEnabled, multiplayer.options.map.flags, 0x02);
         reach_main_window_setup_flag_checkbox(this->ui.optionsMapAbilitiesEnabled, multiplayer.options.map.flags, 0x04);
         reach_main_window_setup_flag_checkbox(this->ui.optionsMapPowerupsEnabled, multiplayer.options.map.flags, 0x08);
         reach_main_window_setup_flag_checkbox(this->ui.optionsMapTurretsEnabled, multiplayer.options.map.flags, 0x10);
         reach_main_window_setup_flag_checkbox(this->ui.optionsMapIndestructibleVehicles, multiplayer.options.map.flags, 0x20);
         {  // Weapon Set
            QComboBox* widget = this->ui.optionsMapWeaponSet;
            //
            // -2 = Map Default
            // -1 = No Weapons
            //  0 = Human
            //  1 = Covenant
            //
            QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int index) {
               auto variant = ReachEditorState::get().currentVariant;
               if (!variant)
                  return;
               int8_t value;
               switch (index) {
                  case 0: // Map Default
                     value = -2; // Map Default
                     break;
                  case 16:
                     value = -1; // No Weapons
                     break;
                  default:
                     value = index - 1;
               }
               variant->multiplayer.options.map.weaponSet = value;
            });
         }
         {  // Vehicle Set
            QComboBox* widget = this->ui.optionsMapVehicleSet;
            //
            // -2 = Map Default
            //  0 = Mongooses
            //  1 = Warthogs
            // 12 = No Vehicles
            //
            QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int index) {
               auto variant = ReachEditorState::get().currentVariant;
               if (!variant)
                  return;
               int8_t value;
               switch (index) {
                  case 0: // Map Default
                     value = -2; // Map Default
                     break;
                  default:
                     value = index - 1;
               }
               variant->multiplayer.options.map.vehicleSet = value;
            });
         }
         reach_main_window_setup_spinbox(this->ui.optionsMapRedPowerupDuration, multiplayer.options.map.powerups.red.duration);
         reach_main_window_setup_spinbox(this->ui.optionsMapBluePowerupDuration, multiplayer.options.map.powerups.blue.duration);
         reach_main_window_setup_spinbox(this->ui.optionsMapYellowPowerupDuration, multiplayer.options.map.powerups.yellow.duration);
      }
      //
      #undef reach_main_window_setup_spinbox
      #undef reach_main_window_setup_flag_checkbox
      #undef reach_main_window_setup_bool_checkbox
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
   // TODO: Respawn Traits
   //
   if (text == tr("Social Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsSocial);
      return;
   }
   if (text == tr("Map and Game Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsMap);
      return;
   }
   //
   // TODO: Base Player Traits
   // TODO: Red Powerup Traits
   // TODO: Blue Powerup Traits
   // TODO: Custom Powerup Traits
   //
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
      const QSignalBlocker blocker0(this->ui.headerName);
      const QSignalBlocker blocker1(this->ui.headerDesc);
      const QSignalBlocker blocker2(this->ui.authorGamertag);
      const QSignalBlocker blocker3(this->ui.editorGamertag);
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
   #define reach_main_window_update_bool_checkbox(w, field) \
      { \
         auto widget = w; \
         const QSignalBlocker blocker(widget); \
         widget->setChecked( variant->##field ); \
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
   {  // Social
      reach_main_window_update_bool_checkbox(this->ui.optionsSocialObservers, multiplayer.options.social.observers);
      reach_main_window_update_spinbox(this->ui.optionsSocialTeamChanging, multiplayer.options.social.teamChanges);
      reach_main_window_update_flag_checkbox(this->ui.optionsSocialFriendlyFire,    multiplayer.options.social.flags, 0x01);
      reach_main_window_update_flag_checkbox(this->ui.optionsSocialBetrayalBooting, multiplayer.options.social.flags, 0x02);
      reach_main_window_update_flag_checkbox(this->ui.optionsSocialProximityVoice,  multiplayer.options.social.flags, 0x04);
      reach_main_window_update_flag_checkbox(this->ui.optionsSocialGlobalVoice,     multiplayer.options.social.flags, 0x08);
      reach_main_window_update_flag_checkbox(this->ui.optionsSocialDeadPlayerVoice, multiplayer.options.social.flags, 0x10);
   }
   {
      reach_main_window_update_flag_checkbox(this->ui.optionsMapGrenadesEnabled, multiplayer.options.map.flags, 0x01);
      reach_main_window_update_flag_checkbox(this->ui.optionsMapShortcutsEnabled, multiplayer.options.map.flags, 0x02);
      reach_main_window_update_flag_checkbox(this->ui.optionsMapAbilitiesEnabled, multiplayer.options.map.flags, 0x04);
      reach_main_window_update_flag_checkbox(this->ui.optionsMapPowerupsEnabled, multiplayer.options.map.flags, 0x08);
      reach_main_window_update_flag_checkbox(this->ui.optionsMapTurretsEnabled, multiplayer.options.map.flags, 0x10);
      reach_main_window_update_flag_checkbox(this->ui.optionsMapIndestructibleVehicles, multiplayer.options.map.flags, 0x20);
      {  // Weapon Set
         QComboBox* widget = this->ui.optionsMapWeaponSet;
         const QSignalBlocker blocker(widget);
         auto value = variant->multiplayer.options.map.weaponSet;
         int  index;
         switch (value) {
            case -2:
               index = 0;
               break;
            case -1:
               index = 16;
               break;
            default:
               index = value + 1;
         }
         widget->setCurrentIndex(index);
      }
      {  // Vehicle Set
         QComboBox* widget = this->ui.optionsMapVehicleSet;
         const QSignalBlocker blocker(widget);
         auto value = variant->multiplayer.options.map.vehicleSet;
         int  index;
         switch (value) {
            case -2:
               index = 0;
               break;
            default:
               index = value + 1;
         }
         widget->setCurrentIndex(index);
      }
      reach_main_window_update_spinbox(this->ui.optionsMapRedPowerupDuration, multiplayer.options.map.powerups.red.duration);
      reach_main_window_update_spinbox(this->ui.optionsMapBluePowerupDuration, multiplayer.options.map.powerups.blue.duration);
      reach_main_window_update_spinbox(this->ui.optionsMapYellowPowerupDuration, multiplayer.options.map.powerups.yellow.duration);
   }
   //
   #undef reach_main_window_update_spinbox
   #undef reach_main_window_update_flag_checkbox
   #undef reach_main_window_update_bool_checkbox
   //
   this->isUpdatingFromVariant = false;
}