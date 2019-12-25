#include "ReachVariantTool.h"
#include <cassert>
#include <filesystem>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QTreeWidget>
#include "editor_state.h"
#include "game_variants/base.h"
#include "helpers/stream.h"
#include "services/ini.h"
#include "ui/generic/QXBLGamertagValidator.h"

#include "ProgramOptionsDialog.h"

namespace {
   ReachVariantTool* _window = nullptr;
}

/*static*/ ReachVariantTool& ReachVariantTool::get() {
   assert(_window && "You shouldn't be calling ReachVariantTool::get before the main window is actually created!");
   return *_window;
}
ReachVariantTool::ReachVariantTool(QWidget *parent) : QMainWindow(parent) {
   ui.setupUi(this);
   _window = this;
   //
   ReachINI::RegisterForChanges([](ReachINI::setting* setting, ReachINI::setting_value_union oldValue, ReachINI::setting_value_union newValue) {
      if (setting == &ReachINI::UIWindowTitle::bShowFullPath || setting == &ReachINI::UIWindowTitle::bShowVariantTitle) {
         ReachVariantTool::get().refreshWindowTitle();
         return;
      }
   });
   //
   QObject::connect(this->ui.actionOpen,    &QAction::triggered, this, &ReachVariantTool::openFile);
   QObject::connect(this->ui.actionSave,    &QAction::triggered, this, &ReachVariantTool::saveFile);
   QObject::connect(this->ui.actionSaveAs,  &QAction::triggered, this, &ReachVariantTool::saveFileAs);
   QObject::connect(this->ui.actionOptions, &QAction::triggered, &ProgramOptionsDialog::get(), &ProgramOptionsDialog::open);
   #if _DEBUG
      QObject::connect(this->ui.actionDebugbreak, &QAction::triggered, []() {
         auto variant = ReachEditorState::get().currentVariant;
         __debugbreak();
      });
   #else
      this->ui.actionDebugbreak->setEnabled(false);
      this->ui.actionDebugbreak->setVisible(false);
   #endif
   //
   this->ui.MainContentView->setCurrentIndex(0); // Qt Designer makes the last page you were looking at in the editor the default page; let's just switch to the first page here
   this->ui.PagePlayerTraitsTabview->setCurrentIndex(0);
   QObject::connect(this->ui.MainTreeview, &QTreeWidget::itemSelectionChanged, this, &ReachVariantTool::onSelectedPageChanged);
   //
   {  // Metadata
      //
      // TODO: Put a validator on gamertags; limit them to ASCII glyphs considered valid by XBL
      //
      QObject::connect(this->ui.headerName, &QLineEdit::textEdited, [](const QString& text) {
         auto variant = ReachEditorState::get().currentVariant;
         if (!variant)
            return;
         const char16_t* value = (const char16_t*)text.utf16();
         variant->contentHeader.data.set_title(value);
         variant->multiplayer.variantHeader.set_title(value);
      });
      //
      auto desc = this->ui.headerDesc;
      QObject::connect(this->ui.headerDesc, &QPlainTextEdit::textChanged, [desc]() {
         auto text    = desc->toPlainText();
         auto variant = ReachEditorState::get().currentVariant;
         if (!variant)
            return;
         const char16_t* value = (const char16_t*)text.utf16();
         variant->contentHeader.data.set_description(value);
         variant->multiplayer.variantHeader.set_description(value);
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
      //
      this->ui.authorGamertag->setValidator(QXBLGamertagValidator::getReachInstance());
      this->ui.editorGamertag->setValidator(QXBLGamertagValidator::getReachInstance());
   }
   {
      //
      // This would be about a thousand times cleaner if we could use pointers-to-members-of-members, 
      // but the language doesn't support that even though it easily could. There are workarounds, but 
      // in C++17 they aren't constexpr and therefore can't be used as template parameters. (My plan 
      // was to create structs that act as decorators, to accomplish this stuff.) So, macros.
      //
      #pragma region Preprocessor macros to set up widgets
         // reach_main_window_setup_combobox -- use only when an enum maps one-to-one with a combobox's indices
      #define reach_main_window_setup_combobox(w, field) \
         { \
            QComboBox* widget = w; \
            QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int value) { \
               auto variant = ReachEditorState::get().currentVariant; \
               if (!variant) \
                  return; \
               variant->##field = value; \
            }); \
         };
      #define reach_main_window_setup_spinbox(w, field) \
         { \
            QSpinBox* widget = w; \
            QObject::connect(widget, QOverload<int>::of(&QSpinBox::valueChanged), [](int value) { \
               auto variant = ReachEditorState::get().currentVariant; \
               if (!variant) \
                  return; \
               variant->##field = value; \
            }); \
         };
      #define reach_main_window_setup_spinbox_dbl(w, field) \
         { \
            QDoubleSpinBox* widget = w; \
            QObject::connect(widget, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [](double value) { \
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
      #pragma endregion
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
      {  // Team
         reach_main_window_setup_spinbox(this->ui.optionsTeamScoringMethod, multiplayer.options.team.scoring);
         reach_main_window_setup_combobox(this->ui.optionsTeamPlayerSpecies, multiplayer.options.team.species);
         reach_main_window_setup_spinbox(this->ui.optionsTeamDesignatorSwitchType, multiplayer.options.team.designatorSwitchType);
      }
      {  // Loadout Base Options
         reach_main_window_setup_flag_checkbox(this->ui.optionsLoadoutFlag0, multiplayer.options.loadouts.flags, 0x01);
         reach_main_window_setup_flag_checkbox(this->ui.optionsLoadoutFlag1, multiplayer.options.loadouts.flags, 0x02);
      }
      //
      // TODO: other pages
      //
      {  // Player Traits
         #pragma region Preprocessor macros to set up Player Traits widgets
         #define reach_traits_pane_setup_combobox(w, field) \
            { \
               auto widget = w; \
               QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int value) { \
                  auto traits = ReachEditorState::get().currentTraits; \
                  if (!traits) \
                     return; \
                  traits->##field = value; \
               }); \
            };
         #define reach_traits_pane_setup_spinbox(w, field) \
            { \
               auto widget = w; \
               QObject::connect(widget, QOverload<int>::of(&QSpinBox::valueChanged), [](int value) { \
                  auto traits = ReachEditorState::get().currentTraits; \
                  if (!traits) \
                     return; \
                  traits->##field = value; \
               }); \
            };
         #pragma endregion
         {  // Defense
            reach_traits_pane_setup_combobox(this->ui.playerTraitDamageResist,     defense.damageResist);
            reach_traits_pane_setup_combobox(this->ui.playerTraitHealthMult,       defense.healthMult);
            reach_traits_pane_setup_combobox(this->ui.playerTraitHealthRegenRate,  defense.healthRate);
            reach_traits_pane_setup_combobox(this->ui.playerTraitShieldMult,       defense.shieldMult);
            reach_traits_pane_setup_combobox(this->ui.playerTraitShieldRegenRate,  defense.shieldRate);
            reach_traits_pane_setup_combobox(this->ui.playerTraitShieldRegenDelay, defense.shieldDelay);
            reach_traits_pane_setup_combobox(this->ui.playerTraitHeadshotImmunity, defense.headshotImmune);
            reach_traits_pane_setup_combobox(this->ui.playerTraitAssassinationImmunity, defense.assassinImmune);
            reach_traits_pane_setup_combobox(this->ui.playerTraitShieldVampirism,  defense.vampirism);
            reach_traits_pane_setup_combobox(this->ui.playerTraitDefenseUnknown09, defense.unk09);
         }
         {  // Offense
            //
            // TODO: WE CAN'T USE THE MACRO FOR WEAPONS OR THE ARMOR ABILITY. REMEMBER, WE SORTED THOSE 
            // NEAR-ALPHABETICALLY AND ATTACHED A "QVariant" DATA ITEM TO EACH SO WE COULD SELECT BY 
            // VALUE THAT WAY
            //
            // REMEMBER TO FIX THE "UPDATE" CODE TOO, NOT JUST THIS CODE HERE
            //
            reach_traits_pane_setup_combobox(this->ui.playerTraitDamageMult,      offense.damageMult);
            reach_traits_pane_setup_combobox(this->ui.playerTraitMeleeMult,       offense.meleeMult);
            {
               auto widget = this->ui.playerTraitWeaponPrimary;
               QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [widget](int value) {
                  auto traits = ReachEditorState::get().currentTraits;
                  if (!traits)
                     return;
                  traits->offense.weaponPrimary = (reach::weapon)widget->currentData().toInt();
               });
            }
            {
               auto widget = this->ui.playerTraitWeaponSecondary;
               QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [widget](int value) {
                  auto traits = ReachEditorState::get().currentTraits;
                  if (!traits)
                     return;
                  traits->offense.weaponSecondary = (reach::weapon)widget->currentData().toInt();
               });
            }
            {
               auto widget = this->ui.playerTraitArmorAbility;
               QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [widget](int value) {
                  auto traits = ReachEditorState::get().currentTraits;
                  if (!traits)
                     return;
                  traits->offense.ability = (reach::ability)widget->currentData().toInt();
               });
            }
            reach_traits_pane_setup_combobox(this->ui.playerTraitGrenadeCount,    offense.grenadeCount);
            reach_traits_pane_setup_combobox(this->ui.playerTraitGrenadeRegen,    offense.grenadeRegen);
            reach_traits_pane_setup_combobox(this->ui.playerTraitWeaponPickup,    offense.weaponPickup);
            reach_traits_pane_setup_combobox(this->ui.playerTraitInfiniteAmmo,    offense.infiniteAmmo);
            //
            // TODO: Armor Ability settings
            //
         }
         {  // Movement
            reach_traits_pane_setup_combobox(this->ui.playerTraitMovementSpeed, movement.speed);
            reach_traits_pane_setup_spinbox(this->ui.playerTraitJumpHeight, movement.jumpHeight);
            reach_traits_pane_setup_combobox(this->ui.playerTraitGravity, movement.gravity);
            reach_traits_pane_setup_combobox(this->ui.playerTraitMovementUnknown, movement.unknown);
            reach_traits_pane_setup_combobox(this->ui.playerTraitVehicleUse, movement.vehicleUsage);
         }
         {  // Appearance
            reach_traits_pane_setup_combobox(this->ui.playerTraitActiveCamo,      appearance.activeCamo);
            reach_traits_pane_setup_combobox(this->ui.playerTraitVisibleWaypoint, appearance.waypoint);
            reach_traits_pane_setup_combobox(this->ui.playerTraitVisibleName,     appearance.visibleName);
            reach_traits_pane_setup_combobox(this->ui.playerTraitAura,            appearance.aura);
            reach_traits_pane_setup_combobox(this->ui.playerTraitForcedColor,     appearance.forcedColor);
         }
         {  // Sensors
            reach_traits_pane_setup_combobox(this->ui.playerTraitRadarState, sensors.radarState);
            reach_traits_pane_setup_combobox(this->ui.playerTraitRadarRange, sensors.radarRange);
            reach_traits_pane_setup_combobox(this->ui.playerTraitDirectionalDamageIndicator, sensors.directionalDamageIndicator);
         }
      }
      {  // Title Update Config
         reach_main_window_setup_flag_checkbox(this->ui.titleUpdateBleedthrough, multiplayer.titleUpdateData.flags, 0x01);
         reach_main_window_setup_flag_checkbox(this->ui.titleUpdateArmorLockCantShed, multiplayer.titleUpdateData.flags, 0x02);
         reach_main_window_setup_flag_checkbox(this->ui.titleUpdateArmorLockCanBeStuck, multiplayer.titleUpdateData.flags, 0x04);
         reach_main_window_setup_flag_checkbox(this->ui.titleUpdateEnableActiveCamoModifiers, multiplayer.titleUpdateData.flags, 0x08);
         reach_main_window_setup_flag_checkbox(this->ui.titleUpdateLimitSwordBlockToSword, multiplayer.titleUpdateData.flags, 0x10);
         reach_main_window_setup_flag_checkbox(this->ui.titleUpdateAutomaticMagnum, multiplayer.titleUpdateData.flags, 0x20);
         reach_main_window_setup_flag_checkbox(this->ui.titleUpdateFlag6, multiplayer.titleUpdateData.flags, 0x40);
         reach_main_window_setup_flag_checkbox(this->ui.titleUpdateFlag7, multiplayer.titleUpdateData.flags, 0x80);
         {  // Precision Bloom
            QDoubleSpinBox* widget = this->ui.titleUpdatePrecisionBloom;
            QObject::connect(widget, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [](int value) {
               auto variant = ReachEditorState::get().currentVariant;
               if (!variant)
                  return;
               variant->multiplayer.titleUpdateData.precisionBloom = value * 5.0F / 100.0F; // normalize from percentage of vanilla to internal format
            });
         }
         reach_main_window_setup_spinbox_dbl(this->ui.titleUpdateArmorLockDamageDrain, multiplayer.titleUpdateData.armorLockDamageDrain);
         reach_main_window_setup_spinbox_dbl(this->ui.titleUpdateArmorLockDamageDrainLimit, multiplayer.titleUpdateData.armorLockDamageDrainLimit);
         reach_main_window_setup_spinbox_dbl(this->ui.titleUpdateActiveCamoEnergyBonus, multiplayer.titleUpdateData.activeCamoEnergyBonus);
         reach_main_window_setup_spinbox_dbl(this->ui.titleUpdateActiveCamoEnergy, multiplayer.titleUpdateData.activeCamoEnergy);
         reach_main_window_setup_spinbox_dbl(this->ui.titleUpdateMagnumDamage, multiplayer.titleUpdateData.magnumDamage);
         reach_main_window_setup_spinbox_dbl(this->ui.titleUpdateMagnumFireDelay, multiplayer.titleUpdateData.magnumFireDelay);
      }
      //
      #undef reach_main_window_setup_combobox
      #undef reach_main_window_setup_spinbox
      #undef reach_main_window_setup_spinbox_dbl
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
   auto variant = new GameVariant();
   if (!variant->read(file)) {
      QMessageBox::information(this, tr("Unable to open file"), tr("Failed to read the game variant data."));
      return;
   }
   editor.take_game_variant(variant, s.c_str());
   this->refreshWidgetsFromVariant();
   this->refreshWindowTitle();
}
void ReachVariantTool::_saveFileImpl(bool saveAs) {
   auto& editor = ReachEditorState::get();
   if (!editor.currentVariant) {
      QMessageBox::information(this, tr("No game variant is open"), tr("We do not currently support creating game variants from scratch. Open a variant and then you can save an edited copy of it."));
      return;
   }
   QString fileName;
   if (saveAs) {
      fileName = QFileDialog::getSaveFileName(
         this,
         tr("Save Game Variant"), // window title
         QString::fromWCharArray(editor.currentFile.c_str()), // working directory and optionally default-selected file
         tr("Game Variant (*.bin);;All Files (*)") // filetype filters
      );
      if (fileName.isEmpty())
         return;
   } else {
      fileName = QString::fromWCharArray(editor.currentFile.c_str());
   }
   QFile file(fileName);
   if (!file.open(QIODevice::WriteOnly)) {
      QMessageBox::information(this, tr("Unable to open file for writing"), file.errorString());
      return;
   }
   if (saveAs) {
      std::wstring temp = fileName.toStdWString();
      editor.set_variant_file_path(temp.c_str());
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
   if (text == tr("Respawn Traits", "MainTreeview")) {
      auto variant = ReachEditorState::get().currentVariant;
      if (!variant)
         return;
      this->switchToPlayerTraits(&variant->multiplayer.options.respawn.traits);
      return;
   }
   if (text == tr("Social Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsSocial);
      return;
   }
   if (text == tr("Map and Game Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsMap);
      return;
   }
   if (text == tr("Base Player Traits", "MainTreeview")) {
      auto variant = ReachEditorState::get().currentVariant;
      if (!variant)
         return;
      this->switchToPlayerTraits(&variant->multiplayer.options.map.baseTraits);
      return;
   }
   if (text == tr("Red Powerup Traits", "MainTreeview")) {
      auto variant = ReachEditorState::get().currentVariant;
      if (!variant)
         return;
      this->switchToPlayerTraits(&variant->multiplayer.options.map.powerups.red.traits);
      return;
   }
   if (text == tr("Blue Powerup Traits", "MainTreeview")) {
      auto variant = ReachEditorState::get().currentVariant;
      if (!variant)
         return;
      this->switchToPlayerTraits(&variant->multiplayer.options.map.powerups.blue.traits);
      return;
   }
   if (text == tr("Custom Powerup Traits", "MainTreeview")) {
      auto variant = ReachEditorState::get().currentVariant;
      if (!variant)
         return;
      this->switchToPlayerTraits(&variant->multiplayer.options.map.powerups.yellow.traits);
      return;
   }
   if (text == tr("Team Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsTeam);
      return;
   }
   //
   // TODO: panes for each team
   //
   if (text == tr("Loadout Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsLoadout);
      return;
   }
   //
   // TODO: panes for each loadout palette
   //
   //
   // TODO: add other panes
   //
   if (text == tr("Option Visibility", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionToggles);
      return;
   }
   if (text == tr("Title Update Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageTitleUpdateConfig);
      return;
   }
}
void ReachVariantTool::refreshWidgetsFromVariant() {
   auto& editor = ReachEditorState::get();
   auto variant = editor.currentVariant;
   if (!variant)
      return;
   {  // Metadata
      const QSignalBlocker blocker0(this->ui.headerName);
      const QSignalBlocker blocker1(this->ui.headerDesc);
      const QSignalBlocker blocker2(this->ui.authorGamertag);
      const QSignalBlocker blocker3(this->ui.editorGamertag);
      this->ui.headerName->setText(QString::fromUtf16(variant->multiplayer.variantHeader.title));
      this->ui.headerDesc->setPlainText(QString::fromUtf16(variant->multiplayer.variantHeader.description));
      this->ui.authorGamertag->setText(QString::fromLatin1(variant->multiplayer.variantHeader.createdBy.author));
      this->ui.editorGamertag->setText(QString::fromLatin1(variant->multiplayer.variantHeader.modifiedBy.author));
   }
   //
   // This would be about a thousand times cleaner if we could use pointers-to-members-of-members, 
   // but the language doesn't support that even though it easily could. There are workarounds, but 
   // in C++17 they aren't constexpr and therefore can't be used as template parameters. (My plan 
   // was to create structs that act as decorators, to accomplish this stuff.) So, macros.
   //
   #pragma region Preprocessor macros to update widgets
      // reach_main_window_update_combobox -- use only when an enum maps one-to-one with a combobox's indices
   #define reach_main_window_update_combobox(w, field) \
      { \
         auto widget = w; \
         const QSignalBlocker blocker(widget); \
         widget->setCurrentIndex( variant->##field ); \
      };
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
   #pragma endregion
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
   {  // Map and Game
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
   {  // Team
      reach_main_window_update_spinbox(this->ui.optionsTeamScoringMethod, multiplayer.options.team.scoring);
      reach_main_window_update_combobox(this->ui.optionsTeamPlayerSpecies, multiplayer.options.team.species);
      reach_main_window_update_spinbox(this->ui.optionsTeamDesignatorSwitchType, multiplayer.options.team.designatorSwitchType);
   }
   {  // Loadout Base Options
      reach_main_window_update_flag_checkbox(this->ui.optionsLoadoutFlag0, multiplayer.options.loadouts.flags, 0x01);
      reach_main_window_update_flag_checkbox(this->ui.optionsLoadoutFlag1, multiplayer.options.loadouts.flags, 0x02);
   }
   //
   // TODO: add other panes
   //
   {  // Title Update Config
      reach_main_window_update_flag_checkbox(this->ui.titleUpdateBleedthrough, multiplayer.titleUpdateData.flags, 0x01);
      reach_main_window_update_flag_checkbox(this->ui.titleUpdateArmorLockCantShed, multiplayer.titleUpdateData.flags, 0x02);
      reach_main_window_update_flag_checkbox(this->ui.titleUpdateArmorLockCanBeStuck, multiplayer.titleUpdateData.flags, 0x04);
      reach_main_window_update_flag_checkbox(this->ui.titleUpdateEnableActiveCamoModifiers, multiplayer.titleUpdateData.flags, 0x08);
      reach_main_window_update_flag_checkbox(this->ui.titleUpdateLimitSwordBlockToSword, multiplayer.titleUpdateData.flags, 0x10);
      reach_main_window_update_flag_checkbox(this->ui.titleUpdateAutomaticMagnum, multiplayer.titleUpdateData.flags, 0x20);
      reach_main_window_update_flag_checkbox(this->ui.titleUpdateFlag6, multiplayer.titleUpdateData.flags, 0x40);
      reach_main_window_update_flag_checkbox(this->ui.titleUpdateFlag7, multiplayer.titleUpdateData.flags, 0x80);
      {  // Precision Bloom
         auto widget = this->ui.titleUpdatePrecisionBloom;
         const QSignalBlocker blocker(widget);
         widget->setValue(variant->multiplayer.titleUpdateData.precisionBloom * 100.0F / 5.0F); // normalize to percentage of vanilla
      }
      reach_main_window_update_spinbox(this->ui.titleUpdateArmorLockDamageDrain, multiplayer.titleUpdateData.armorLockDamageDrain);
      reach_main_window_update_spinbox(this->ui.titleUpdateArmorLockDamageDrainLimit, multiplayer.titleUpdateData.armorLockDamageDrainLimit);
      reach_main_window_update_spinbox(this->ui.titleUpdateActiveCamoEnergyBonus, multiplayer.titleUpdateData.activeCamoEnergyBonus);
      reach_main_window_update_spinbox(this->ui.titleUpdateActiveCamoEnergy, multiplayer.titleUpdateData.activeCamoEnergy);
      reach_main_window_update_spinbox(this->ui.titleUpdateMagnumDamage, multiplayer.titleUpdateData.magnumDamage);
      reach_main_window_update_spinbox(this->ui.titleUpdateMagnumFireDelay, multiplayer.titleUpdateData.magnumFireDelay);
   }
   //
   #undef reach_main_window_update_combobox
   #undef reach_main_window_update_spinbox
   #undef reach_main_window_update_flag_checkbox
   #undef reach_main_window_update_bool_checkbox
}
void ReachVariantTool::switchToPlayerTraits(ReachPlayerTraits* traits) {
   ReachEditorState::get().start_editing_traits(traits);
   this->refreshWidgetsForPlayerTraits();
   this->ui.MainContentView->setCurrentWidget(this->ui.PagePlayerTraits);
}
void ReachVariantTool::refreshWidgetsForPlayerTraits() {
   auto& editor = ReachEditorState::get();
   auto  traits = editor.currentTraits;
   if (!traits)
      return;
   #define reach_traits_pane_update_combobox(w, field) \
      { \
         auto widget = w; \
         const QSignalBlocker blocker(widget); \
         widget->setCurrentIndex((int)traits->##field ); \
      }
   #define reach_traits_pane_update_spinbox(w, field) \
      { \
         auto widget = w; \
         const QSignalBlocker blocker(widget); \
         widget->setValue((int)traits->##field ); \
      }
   {  // Defense
      reach_traits_pane_update_combobox(this->ui.playerTraitDamageResist,     defense.damageResist);
      reach_traits_pane_update_combobox(this->ui.playerTraitHealthMult,       defense.healthMult);
      reach_traits_pane_update_combobox(this->ui.playerTraitHealthRegenRate,  defense.healthRate);
      reach_traits_pane_update_combobox(this->ui.playerTraitShieldMult,       defense.shieldMult);
      reach_traits_pane_update_combobox(this->ui.playerTraitShieldRegenRate,  defense.shieldRate);
      reach_traits_pane_update_combobox(this->ui.playerTraitShieldRegenDelay, defense.shieldDelay);
      reach_traits_pane_update_combobox(this->ui.playerTraitHeadshotImmunity, defense.headshotImmune);
      reach_traits_pane_update_combobox(this->ui.playerTraitAssassinationImmunity, defense.assassinImmune);
      reach_traits_pane_update_combobox(this->ui.playerTraitShieldVampirism,  defense.vampirism);
      reach_traits_pane_update_combobox(this->ui.playerTraitDefenseUnknown09, defense.unk09);
   }
   {  // Offense
      reach_traits_pane_update_combobox(this->ui.playerTraitDamageMult,      offense.damageMult);
      reach_traits_pane_update_combobox(this->ui.playerTraitMeleeMult,       offense.meleeMult);
      {
         auto widget = this->ui.playerTraitWeaponPrimary;
         const QSignalBlocker blocker(widget);
         widget->setByData((int)traits->offense.weaponPrimary);
      }
      {
         auto widget = this->ui.playerTraitWeaponSecondary;
         const QSignalBlocker blocker(widget);
         widget->setByData((int)traits->offense.weaponSecondary);
      }
      {
         auto widget = this->ui.playerTraitArmorAbility;
         const QSignalBlocker blocker(widget);
         widget->setByData((int)traits->offense.ability);
      }
      reach_traits_pane_update_combobox(this->ui.playerTraitGrenadeCount,    offense.grenadeCount);
      reach_traits_pane_update_combobox(this->ui.playerTraitGrenadeRegen,    offense.grenadeRegen);
      reach_traits_pane_update_combobox(this->ui.playerTraitWeaponPickup,    offense.weaponPickup);
      reach_traits_pane_update_combobox(this->ui.playerTraitInfiniteAmmo,    offense.infiniteAmmo);
      //
      // TODO: Armor Ability settings
      //
   }
   {  // Movement
      reach_traits_pane_update_combobox(this->ui.playerTraitMovementSpeed, movement.speed);
      reach_traits_pane_update_spinbox(this->ui.playerTraitJumpHeight, movement.jumpHeight);
      reach_traits_pane_update_combobox(this->ui.playerTraitGravity, movement.gravity);
      reach_traits_pane_update_combobox(this->ui.playerTraitMovementUnknown, movement.unknown);
      reach_traits_pane_update_combobox(this->ui.playerTraitVehicleUse, movement.vehicleUsage);
   }
   {  // Appearance
      reach_traits_pane_update_combobox(this->ui.playerTraitActiveCamo,      appearance.activeCamo);
      reach_traits_pane_update_combobox(this->ui.playerTraitVisibleWaypoint, appearance.waypoint);
      reach_traits_pane_update_combobox(this->ui.playerTraitVisibleName,     appearance.visibleName);
      reach_traits_pane_update_combobox(this->ui.playerTraitAura,            appearance.aura);
      reach_traits_pane_update_combobox(this->ui.playerTraitForcedColor,     appearance.forcedColor);
   }
   {  // Sensors
      reach_traits_pane_update_combobox(this->ui.playerTraitRadarState, sensors.radarState);
      reach_traits_pane_update_combobox(this->ui.playerTraitRadarRange, sensors.radarRange);
      reach_traits_pane_update_combobox(this->ui.playerTraitDirectionalDamageIndicator, sensors.directionalDamageIndicator);
   }
   #undef reach_traits_pane_update_combobox
   #undef reach_traits_pane_update_spinbox
}
void ReachVariantTool::refreshWindowTitle() {
   auto& editor = ReachEditorState::get();
   if (!editor.currentVariant) {
      this->setWindowTitle("ReachVariantTool");
      return;
   }
   std::wstring file = editor.currentFile;
   if (ReachINI::UIWindowTitle::bShowFullPath.current.b == false) {
      file = std::filesystem::path(file).filename().wstring();
   }
   if (ReachINI::UIWindowTitle::bShowVariantTitle.current.b == true) {
      QString variantTitle = QString::fromUtf16(editor.currentVariant->contentHeader.data.title);
      this->setWindowTitle(
         QString("%1 <%2> - ReachVariantTool").arg(variantTitle).arg(file)
      );
   } else {
      this->setWindowTitle(
         QString("%1 - ReachVariantTool").arg(file)
      );
   }
}