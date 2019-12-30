#include "ReachVariantTool.h"
#include <cassert>
#include <filesystem>
#include <QColorDialog>
#include <QDateTime>
#include <QFileDialog>
#include <QGridLayout>
#include <QMessageBox>
#include <QSlider>
#include <QString>
#include <QTreeWidget>
#include <QWidget>
#include "editor_state.h"
#include "game_variants/base.h"
#include "game_variants/errors.h"
#include "helpers/ini.h"
#include "helpers/stream.h"
#include "services/ini.h"
#include "ui/generic/QXBLGamertagValidator.h"

#include "ProgramOptionsDialog.h"

namespace {
   ReachVariantTool* _window = nullptr;

   QColor _colorFromReach(uint32_t c) {
      return QColor((c >> 0x10) & 0xFF, (c >> 0x08) & 0xFF, c & 0xFF);
   }
   uint32_t _colorToReach(const QColor& c) {
      return ((c.red() & 0xFF) << 0x10) | ((c.green() & 0xFF) << 0x08) | (c.blue() & 0xFF);
   }
}

/*static*/ ReachVariantTool& ReachVariantTool::get() {
   assert(_window && "You shouldn't be calling ReachVariantTool::get before the main window is actually created!");
   return *_window;
}
ReachVariantTool::ReachVariantTool(QWidget *parent) : QMainWindow(parent) {
   ui.setupUi(this);
   _window = this;
   //
   ReachINI::get().register_for_changes([](cobb::ini::setting* setting, cobb::ini::setting_value_union oldValue, cobb::ini::setting_value_union newValue) {
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
   {  // set up engine categories
      auto widget = this->ui.engineCategory;
      widget->clear();
      widget->addItem(tr("Capture the Flag", "Engine Category"), QVariant(0));
      widget->addItem(tr("Slayer", "Engine Category"), QVariant(1));
      widget->addItem(tr("Oddball", "Engine Category"), QVariant(2));
      widget->addItem(tr("King of the Hill", "Engine Category"), QVariant(3));
      widget->addItem(tr("Juggernaut", "Engine Category"), QVariant(4));
      widget->addItem(tr("Territories", "Engine Category"), QVariant(5));
      widget->addItem(tr("Assault", "Engine Category"), QVariant(6));
      widget->addItem(tr("Infection", "Engine Category"), QVariant(7));
      widget->addItem(tr("VIP", "Engine Category"), QVariant(8));
      widget->addItem(tr("Invasion", "Engine Category"), QVariant(9));
      widget->addItem(tr("Stockpile", "Engine Category"), QVariant(10));
      widget->addItem(tr("Race", "Engine Category"), QVariant(12));
      widget->addItem(tr("Headhunter", "Engine Category"), QVariant(13));
      widget->addItem(tr("Insane", "Engine Category"), QVariant(16));
      widget->addItem(tr("Halomods.com", "Engine Category"), QVariant(24)); // for consistency with KSoft
      widget->addItem(tr("Community", "Engine Category"), QVariant(25));
   }
   //
   {  // Metadata
      QObject::connect(this->ui.headerName, &QLineEdit::textEdited, [this](const QString& text) {
         auto variant = ReachEditorState::get().currentVariant;
         if (!variant)
            return;
         auto u16s = text.toStdU16String();
         const char16_t* value = u16s.c_str();
         variant->contentHeader.data.set_title(value);
         variant->multiplayer.data->as_multiplayer()->variantHeader.set_title(value);
         //
         if (ReachINI::UIWindowTitle::bShowVariantTitle.current.b)
            this->refreshWindowTitle();
      });
      QObject::connect(this->ui.headerName, &QLineEdit::textEdited, this, &ReachVariantTool::updateDescriptionCharacterCount);
      //
      auto desc      = this->ui.headerDesc;
      auto descCount = this->ui.headerDescCharacterLimit;
      QObject::connect(this->ui.headerDesc, &QPlainTextEdit::textChanged, [desc, descCount]() {
         auto text    = desc->toPlainText();
         auto variant = ReachEditorState::get().currentVariant;
         if (!variant)
            return;
         auto u16s = text.toStdU16String();
         const char16_t* value = u16s.c_str();
         variant->contentHeader.data.set_description(value);
         variant->multiplayer.data->as_multiplayer()->variantHeader.set_description(value);
      });
      //
      QObject::connect(this->ui.engineIcon, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int value) {
         auto variant = ReachEditorState::get().currentVariant;
         if (!variant)
            return;
         auto data    = ReachEditorState::get().get_multiplayer_data();
         if (!data)
            return;
         variant->contentHeader.data.engineIcon = value;
         data->engineIcon = value;
         data->variantHeader.engineIcon = value;
      });
      QObject::connect(this->ui.engineCategory, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int value) {
         auto variant = ReachEditorState::get().currentVariant;
         if (!variant)
            return;
         auto data    = ReachEditorState::get().get_multiplayer_data();
         if (!data)
            return;
         variant->contentHeader.data.engineCategory = value;
         data->engineCategory = value;
         data->variantHeader.engineCategory = value;
      });
      //
      QObject::connect(this->ui.authorGamertag, &QLineEdit::textEdited, [](const QString& text) {
         auto variant = ReachEditorState::get().currentVariant;
         if (!variant)
            return;
         auto latin = text.toLatin1();
         variant->contentHeader.data.createdBy.set_author_name(latin.constData());
         variant->multiplayer.data->as_multiplayer()->variantHeader.createdBy.set_author_name(latin.constData());
      });
      QObject::connect(this->ui.eraseAuthorXUID, &QPushButton::clicked, [this]() {
         auto variant = ReachEditorState::get().currentVariant;
         if (!variant)
            return;
         auto& author_c = variant->contentHeader.data.createdBy;
         auto& author_m = variant->multiplayer.data->as_multiplayer()->variantHeader.createdBy;
         if (author_c.has_xuid() || author_m.has_xuid()) {
            author_c.erase_xuid();
            author_m.erase_xuid();
            QMessageBox::information(this, tr("Operation complete"), tr("The XUID and \"is online ID\" flag for this file's author have been wiped."));
            return;
         } else {
            author_c.erase_xuid();
            author_m.erase_xuid();
            QMessageBox::information(this, tr("Operation complete"), tr("The XUID and \"is online ID\" flag for this file's author were already blank."));
            return;
         }
      });
      QObject::connect(this->ui.createdOnDate, &QDateTimeEdit::dateTimeChanged, [](const QDateTime& time) {
         auto variant = ReachEditorState::get().currentVariant;
         if (!variant)
            return;
         uint64_t seconds = time.toSecsSinceEpoch();
         variant->contentHeader.data.createdBy.set_datetime(seconds);
         variant->multiplayer.data->as_multiplayer()->variantHeader.createdBy.set_datetime(seconds);
      });
      QObject::connect(this->ui.editorGamertag, &QLineEdit::textEdited, [](const QString& text) {
         auto variant = ReachEditorState::get().currentVariant;
         if (!variant)
            return;
         auto latin = text.toLatin1();
         variant->contentHeader.data.modifiedBy.set_author_name(latin.constData());
         variant->multiplayer.data->as_multiplayer()->variantHeader.modifiedBy.set_author_name(latin.constData());
      });
      QObject::connect(this->ui.eraseEditorXUID, &QPushButton::clicked, [this]() {
         auto variant = ReachEditorState::get().currentVariant;
         if (!variant)
            return;
         auto& author_c = variant->contentHeader.data.modifiedBy;
         auto& author_m = variant->multiplayer.data->as_multiplayer()->variantHeader.modifiedBy;
         if (author_c.has_xuid() || author_m.has_xuid()) {
            author_c.erase_xuid();
            author_m.erase_xuid();
            QMessageBox::information(this, tr("Operation complete"), tr("The XUID and \"is online ID\" flag for this file's editor have been wiped."));
            return;
         } else {
            author_c.erase_xuid();
            author_m.erase_xuid();
            QMessageBox::information(this, tr("Operation complete"), tr("The XUID and \"is online ID\" flag for this file's editor were already blank."));
            return;
         }
      });
      QObject::connect(this->ui.editedOnDate, &QDateTimeEdit::dateTimeChanged, [](const QDateTime& time) {
         auto variant = ReachEditorState::get().currentVariant;
         if (!variant)
            return;
         uint64_t seconds = time.toSecsSinceEpoch();
         variant->contentHeader.data.modifiedBy.set_datetime(seconds);
         variant->multiplayer.data->as_multiplayer()->variantHeader.modifiedBy.set_datetime(seconds);
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
               auto data = ReachEditorState::get().get_multiplayer_data(); \
               if (!data) \
                  return; \
               data->##field = value; \
            }); \
         };
      #define reach_main_window_setup_spinbox(w, field) \
         { \
            QSpinBox* widget = w; \
            QObject::connect(widget, QOverload<int>::of(&QSpinBox::valueChanged), [](int value) { \
               auto data = ReachEditorState::get().get_multiplayer_data(); \
               if (!data) \
                  return; \
               data->##field = value; \
            }); \
         };
      #define reach_main_window_setup_spinbox_dbl(w, field) \
         { \
            QDoubleSpinBox* widget = w; \
            QObject::connect(widget, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [](double value) { \
               auto data = ReachEditorState::get().get_multiplayer_data(); \
               if (!data) \
                  return; \
               data->##field = value; \
            }); \
         };
      #define reach_main_window_setup_flag_checkbox(w, field, mask) \
         { \
            QCheckBox* widget = w; \
            QObject::connect(widget, &QCheckBox::stateChanged, [widget](int state) { \
               auto data = ReachEditorState::get().get_multiplayer_data(); \
               if (!data) \
                  return; \
               if (widget->isChecked()) \
                  data->##field |= mask ; \
               else \
                  data->##field &= ~ mask ; \
            }); \
         };
      #define reach_main_window_setup_bool_checkbox(w, field) \
         { \
            QCheckBox* widget = w; \
            QObject::connect(widget, &QCheckBox::stateChanged, [widget](int state) { \
               auto data = ReachEditorState::get().get_multiplayer_data(); \
               if (!data) \
                  return; \
               data->##field = widget->isChecked(); \
            }); \
         };
      #pragma endregion
      { // General
         reach_main_window_setup_flag_checkbox(this->ui.optionsGeneralTeamsEnabled,          options.misc.flags, 1);
         reach_main_window_setup_flag_checkbox(this->ui.optionsGeneralNewRoundResetsPlayers, options.misc.flags, 2);
         reach_main_window_setup_flag_checkbox(this->ui.optionsGeneralNewRoundResetsMap,     options.misc.flags, 4);
         reach_main_window_setup_flag_checkbox(this->ui.optionsGeneralFlag3,                 options.misc.flags, 8);
         reach_main_window_setup_spinbox(this->ui.optionsGeneralRoundTimeLimit,  options.misc.timeLimit);
         reach_main_window_setup_spinbox(this->ui.optionsGeneralRoundLimit,      options.misc.roundLimit);
         reach_main_window_setup_spinbox(this->ui.optionsGeneralRoundsToWin,     options.misc.roundsToWin);
         reach_main_window_setup_spinbox(this->ui.optionsGeneralSuddenDeathTime, options.misc.suddenDeathTime);
         reach_main_window_setup_spinbox(this->ui.optionsGeneralGracePeriod,     options.misc.gracePeriod);
      }
      {  // Respawn
         reach_main_window_setup_flag_checkbox(this->ui.optionsRespawnSyncWithTeam,   options.respawn.flags, 1);
         reach_main_window_setup_flag_checkbox(this->ui.optionsRespawnFlag1,          options.respawn.flags, 2);
         reach_main_window_setup_flag_checkbox(this->ui.optionsRespawnFlag2,          options.respawn.flags, 4);
         reach_main_window_setup_flag_checkbox(this->ui.optionsRespawnRespawnOnKills, options.respawn.flags, 8);
         reach_main_window_setup_spinbox(this->ui.optionsRespawnLivesPerRound,     options.respawn.livesPerRound);
         reach_main_window_setup_spinbox(this->ui.optionsRespawnTeamLivesPerRound, options.respawn.teamLivesPerRound);
         reach_main_window_setup_spinbox(this->ui.optionsRespawnRespawnTime,       options.respawn.respawnTime);
         reach_main_window_setup_spinbox(this->ui.optionsRespawnSuicidePenalty,    options.respawn.suicidePenalty);
         reach_main_window_setup_spinbox(this->ui.optionsRespawnBetrayalPenalty,   options.respawn.betrayalPenalty);
         reach_main_window_setup_spinbox(this->ui.optionsRespawnRespawnGrowth,     options.respawn.respawnGrowth);
         reach_main_window_setup_spinbox(this->ui.optionsRespawnLoadoutCamTime,    options.respawn.loadoutCamTime);
         reach_main_window_setup_spinbox(this->ui.optionsRespawnTraitsDuration,    options.respawn.traitsDuration);
      }
      {  // Social
         reach_main_window_setup_bool_checkbox(this->ui.optionsSocialObservers, options.social.observers);
         reach_main_window_setup_spinbox(this->ui.optionsSocialTeamChanging, options.social.teamChanges);
         reach_main_window_setup_flag_checkbox(this->ui.optionsSocialFriendlyFire,    options.social.flags, 0x01);
         reach_main_window_setup_flag_checkbox(this->ui.optionsSocialBetrayalBooting, options.social.flags, 0x02);
         reach_main_window_setup_flag_checkbox(this->ui.optionsSocialProximityVoice,  options.social.flags, 0x04);
         reach_main_window_setup_flag_checkbox(this->ui.optionsSocialGlobalVoice,     options.social.flags, 0x08);
         reach_main_window_setup_flag_checkbox(this->ui.optionsSocialDeadPlayerVoice, options.social.flags, 0x10);
      }
      {  // Map and Game
         reach_main_window_setup_flag_checkbox(this->ui.optionsMapGrenadesEnabled, options.map.flags, 0x01);
         reach_main_window_setup_flag_checkbox(this->ui.optionsMapShortcutsEnabled, options.map.flags, 0x02);
         reach_main_window_setup_flag_checkbox(this->ui.optionsMapAbilitiesEnabled, options.map.flags, 0x04);
         reach_main_window_setup_flag_checkbox(this->ui.optionsMapPowerupsEnabled, options.map.flags, 0x08);
         reach_main_window_setup_flag_checkbox(this->ui.optionsMapTurretsEnabled, options.map.flags, 0x10);
         reach_main_window_setup_flag_checkbox(this->ui.optionsMapIndestructibleVehicles, options.map.flags, 0x20);
         {  // Weapon Set
            QComboBox* widget = this->ui.optionsMapWeaponSet;
            //
            // -2 = Map Default
            // -1 = No Weapons
            //  0 = Human
            //  1 = Covenant
            //
            QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int index) {
               auto data = ReachEditorState::get().get_multiplayer_data();
               if (!data)
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
               data->options.map.weaponSet = value;
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
               auto data = ReachEditorState::get().get_multiplayer_data();
               if (!data)
                  return;
               int8_t value;
               switch (index) {
                  case 0: // Map Default
                     value = -2; // Map Default
                     break;
                  default:
                     value = index - 1;
               }
               data->options.map.vehicleSet = value;
            });
         }
         reach_main_window_setup_spinbox(this->ui.optionsMapRedPowerupDuration, options.map.powerups.red.duration);
         reach_main_window_setup_spinbox(this->ui.optionsMapBluePowerupDuration, options.map.powerups.blue.duration);
         reach_main_window_setup_spinbox(this->ui.optionsMapYellowPowerupDuration, options.map.powerups.yellow.duration);
      }
      {  // Team
         reach_main_window_setup_spinbox(this->ui.optionsTeamScoringMethod, options.team.scoring);
         reach_main_window_setup_combobox(this->ui.optionsTeamPlayerSpecies, options.team.species);
         reach_main_window_setup_spinbox(this->ui.optionsTeamDesignatorSwitchType, options.team.designatorSwitchType);
      }
      {  // Loadout Base Options
         reach_main_window_setup_flag_checkbox(this->ui.optionsLoadoutFlag0, options.loadouts.flags, 0x01);
         reach_main_window_setup_flag_checkbox(this->ui.optionsLoadoutFlag1, options.loadouts.flags, 0x02);
      }
      {  // Specific Team Settings
         {  // Add each team to the navigation
            auto tree = this->ui.MainTreeview;
            QTreeWidgetItem* branch;
            {
               auto list = tree->findItems(tr("Team Settings", "MainTreeview"), Qt::MatchRecursive, 0);
               if (!list.size())
                  return;
               branch = list[0];
            }
            for (QTreeWidgetItem* child : branch->takeChildren())
               delete child;
            for (size_t i = 0; i < 8; i++) {
               auto item = new QTreeWidgetItem(branch, QTreeWidgetItem::UserType + i);
               item->setText(0, QString("Team %1").arg(i + 1));
            }
         }
         //
         #pragma region Preprocessor macros to set up Specific Team widgets
         #define reach_team_pane_setup_flag_checkbox(w, field, mask) \
            { \
               QCheckBox* widget = w; \
               QObject::connect(widget, &QCheckBox::stateChanged, [widget](int state) { \
                  auto team = ReachVariantTool::get()._getCurrentTeam(); \
                  if (!team) \
                     return; \
                  if (widget->isChecked()) \
                     team->##field |= mask ; \
                  else \
                     team->##field &= ~ mask ; \
               }); \
            };
         #define reach_team_pane_setup_combobox(w, field) \
            { \
               auto widget = w; \
               QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int value) { \
                  auto team = ReachVariantTool::get()._getCurrentTeam(); \
                  if (!team) \
                     return; \
                  team->##field = value; \
               }); \
            };
         #define reach_team_pane_setup_spinbox(w, field) \
            { \
               auto widget = w; \
               QObject::connect(widget, QOverload<int>::of(&QSpinBox::valueChanged), [](int value) { \
                  auto team = ReachVariantTool::get()._getCurrentTeam(); \
                  if (!team) \
                     return; \
                  team->##field = value; \
               }); \
            };
         #pragma endregion
         reach_team_pane_setup_flag_checkbox(this->ui.specificTeamFlagEnabled,        flags, 1);
         reach_team_pane_setup_flag_checkbox(this->ui.specificTeamFlagColorPrimary,   flags, 2);
         reach_team_pane_setup_flag_checkbox(this->ui.specificTeamFlagColorSecondary, flags, 4);
         reach_team_pane_setup_flag_checkbox(this->ui.specificTeamFlagColorText,      flags, 8);
         reach_team_pane_setup_combobox(this->ui.specificTeamSpecies, spartanOrElite);
         reach_team_pane_setup_spinbox(this->ui.specificTeamFireteamCount, fireteamCount);
         reach_team_pane_setup_spinbox(this->ui.specificTeamInitialDesignator, initialDesignator);
         QObject::connect(this->ui.specificTeamButtonColorPrimary, &QPushButton::clicked, [this]() {
            auto team = this->_getCurrentTeam();
            if (!team)
               return;
            auto color = QColorDialog::getColor(_colorFromReach(team->colorPrimary), this);
            if (!color.isValid())
               return;
            team->colorPrimary = _colorToReach(color);
            this->refreshTeamColorWidgets();
         });
         QObject::connect(this->ui.specificTeamButtonColorSecondary, &QPushButton::clicked, [this]() {
            auto team = this->_getCurrentTeam();
            if (!team)
               return;
            auto color = QColorDialog::getColor(_colorFromReach(team->colorSecondary), this);
            if (!color.isValid())
               return;
            team->colorSecondary = _colorToReach(color);
            this->refreshTeamColorWidgets();
         });
         QObject::connect(this->ui.specificTeamButtonColorText, &QPushButton::clicked, [this]() {
            auto team = this->_getCurrentTeam();
            if (!team)
               return;
            auto color = QColorDialog::getColor(_colorFromReach(team->colorText), this);
            if (!color.isValid())
               return;
            team->colorText = _colorToReach(color);
            this->refreshTeamColorWidgets();
         });
         #undef reach_team_pane_setup_flag_checkbox
         #undef reach_team_pane_setup_combobox
         #undef reach_team_pane_setup_spinbox
      }
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
            reach_traits_pane_setup_combobox(this->ui.playerTraitCannotDieFromDamage, defense.cannotDieFromDamage);
         }
         {  // Offense
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
            reach_traits_pane_setup_combobox(this->ui.playerTraitAbilityUsage,    offense.abilityUsage);
            reach_traits_pane_setup_combobox(this->ui.playerTraitAbilitiesDropOnDeath,  offense.abilitiesDropOnDeath);
            reach_traits_pane_setup_combobox(this->ui.playerTraitInfiniteAbility, offense.infiniteAbility);
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
         #undef reach_traits_pane_setup_combobox
         #undef reach_traits_pane_setup_spinbox
      }
      {  // Title Update Config
         reach_main_window_setup_flag_checkbox(this->ui.titleUpdateBleedthrough, titleUpdateData.flags, 0x01);
         reach_main_window_setup_flag_checkbox(this->ui.titleUpdateArmorLockCantShed, titleUpdateData.flags, 0x02);
         reach_main_window_setup_flag_checkbox(this->ui.titleUpdateArmorLockCanBeStuck, titleUpdateData.flags, 0x04);
         reach_main_window_setup_flag_checkbox(this->ui.titleUpdateEnableActiveCamoModifiers, titleUpdateData.flags, 0x08);
         reach_main_window_setup_flag_checkbox(this->ui.titleUpdateLimitSwordBlockToSword, titleUpdateData.flags, 0x10);
         reach_main_window_setup_flag_checkbox(this->ui.titleUpdateAutomaticMagnum, titleUpdateData.flags, 0x20);
         reach_main_window_setup_flag_checkbox(this->ui.titleUpdateFlag6, titleUpdateData.flags, 0x40);
         reach_main_window_setup_flag_checkbox(this->ui.titleUpdateFlag7, titleUpdateData.flags, 0x80);
         {  // Precision Bloom
            QDoubleSpinBox* widget = this->ui.titleUpdatePrecisionBloom;
            QObject::connect(widget, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [](int value) {
               auto data = ReachEditorState::get().get_multiplayer_data();
               if (!data)
                  return;
               data->titleUpdateData.precisionBloom = value * 5.0F / 100.0F; // normalize from percentage of vanilla to internal format
            });
         }
         reach_main_window_setup_spinbox_dbl(this->ui.titleUpdateArmorLockDamageDrain, titleUpdateData.armorLockDamageDrain);
         reach_main_window_setup_spinbox_dbl(this->ui.titleUpdateArmorLockDamageDrainLimit, titleUpdateData.armorLockDamageDrainLimit);
         reach_main_window_setup_spinbox_dbl(this->ui.titleUpdateActiveCamoEnergyBonus, titleUpdateData.activeCamoEnergyBonus);
         reach_main_window_setup_spinbox_dbl(this->ui.titleUpdateActiveCamoEnergy, titleUpdateData.activeCamoEnergy);
         reach_main_window_setup_spinbox_dbl(this->ui.titleUpdateMagnumDamage, titleUpdateData.magnumDamage);
         reach_main_window_setup_spinbox_dbl(this->ui.titleUpdateMagnumFireDelay, titleUpdateData.magnumFireDelay);
      }
      {  // Forge
         {
            QCheckBox* widget = this->ui.forgeEnable;
            QObject::connect(widget, &QCheckBox::stateChanged, [widget](int state) {
               auto data = ReachEditorState::get().get_multiplayer_data();
               if (!data)
                  return;
               data->isForge = widget->isChecked();
            });
         }
         reach_main_window_setup_flag_checkbox(this->ui.forgeFlagOpenVoice, forgeData.flags, 0x01);
         reach_main_window_setup_flag_checkbox(this->ui.forgeFlag1, forgeData.flags, 0x02);
         reach_main_window_setup_spinbox(this->ui.forgeRespawnTime, forgeData.respawnTime);
         reach_main_window_setup_combobox(this->ui.forgeEditModeAccess, forgeData.editModeType);
      }
      //
      #undef reach_main_window_setup_combobox
      #undef reach_main_window_setup_spinbox
      #undef reach_main_window_setup_spinbox_dbl
      #undef reach_main_window_setup_flag_checkbox
      #undef reach_main_window_setup_bool_checkbox
   }
}

void ReachVariantTool::updateDescriptionCharacterCount() {
   auto text    = this->ui.headerDesc->toPlainText();
   auto length  = text.size();
   auto readout = this->ui.headerDescCharacterLimit;
   readout->setText(tr("%1 / %2").arg(length).arg(127));
   if (length >= 128)
      readout->setStyleSheet("QLabel { color: red; }");
   else
      readout->setStyleSheet("");
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
   if (!file) {
      QString text = tr("Failed to open the file. %1");
      //
      LPVOID message;
      uint32_t size = FormatMessage(
         FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
         nullptr,
         file.get_error(),
         LANG_USER_DEFAULT,
         (LPTSTR)&message,
         0,
         nullptr
      );
      text = text.arg((LPCTSTR)message);
      LocalFree(message);
      //
      QMessageBox::information(this, tr("Unable to open file"), text);
      return;
   }
   auto variant = new GameVariant();
   if (!variant->read(file)) {
      auto& error_report = GameEngineVariantLoadError::get();
      if (error_report.state == GameEngineVariantLoadError::load_state::failure) {
         QMessageBox::information(this, tr("Unable to open file"), error_report.to_qstring());
      } else {
         QMessageBox::information(this, tr("Unable to open file"), tr("Failed to read the game variant data. The code that failed didn't signal a reason; that would be a programming mistake on my part, so let me know."));
      }
      return;
   }
   editor.take_game_variant(variant, s.c_str());
   this->refreshWidgetsFromVariant();
   this->refreshScriptedPlayerTraitList();
   this->setupWidgetsForScriptedOptions();
   {
      auto i = this->ui.MainTreeview->currentIndex();
      this->ui.MainTreeview->setCurrentItem(nullptr);
      this->ui.MainTreeview->setCurrentIndex(i); // force update of team, trait, etc., pages
   }
   this->refreshWindowTitle();
   this->updateDescriptionCharacterCount();
   this->ui.optionTogglesScripted->updateModelFromGameVariant();
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
      this->refreshWindowTitle();
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
   auto sel     = selections[0];
   auto text    = sel->text(0);
   auto stack   = this->ui.MainContentView;
   auto variant = ReachEditorState::get().currentVariant;
   auto mp_data = variant ? variant->multiplayer.data->as_multiplayer() : nullptr;
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
   if (text == tr("Social Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsSocial);
      return;
   }
   if (text == tr("Map and Game Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsMap);
      return;
   }
   if (text == tr("Team Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsTeam);
      return;
   }
   if (text == tr("Script-Specific Options", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsScripted);
      return;
   }
   if (text == tr("Loadout Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsLoadout);
      return;
   }
   if (variant && mp_data) { // traits; loadout palettes; scripted traits; teams
      if (auto p = sel->parent()) {
         const auto text = p->text(0);
         if (text == tr("Script-Specific Options", "MainTreeview")) {
            auto t = sel->type();
            if (t >= QTreeWidgetItem::UserType) {
               size_t index = t - QTreeWidgetItem::UserType;
               this->switchToPlayerTraits(&mp_data->scriptData.traits[index]);
               return;
            }
         }
      }
      if (auto p = sel->parent()) {
         const auto text = p->text(0);
         if (text == tr("Team Settings", "MainTreeview")) {
            auto t = sel->type();
            if (t >= QTreeWidgetItem::UserType) {
               size_t index = t - QTreeWidgetItem::UserType;
               this->switchToTeam(index);
               return;
            }
         }
      }
      //
      if (text == tr("Respawn Traits", "MainTreeview")) {
         this->switchToPlayerTraits(&mp_data->options.respawn.traits);
         return;
      }
      if (text == tr("Base Player Traits", "MainTreeview")) {
         this->switchToPlayerTraits(&mp_data->options.map.baseTraits);
         return;
      }
      if (text == tr("Red Powerup Traits", "MainTreeview")) {
         this->switchToPlayerTraits(&mp_data->options.map.powerups.red.traits);
         return;
      }
      if (text == tr("Blue Powerup Traits", "MainTreeview")) {
         this->switchToPlayerTraits(&mp_data->options.map.powerups.blue.traits);
         return;
      }
      if (text == tr("Custom Powerup Traits", "MainTreeview")) {
         this->switchToPlayerTraits(&mp_data->options.map.powerups.yellow.traits);
         return;
      }
      if (text == tr("Editor Traits", "MainTreeview")) {
         this->switchToPlayerTraits(&mp_data->forgeData.editorTraits);
         return;
      }
      //
      const QString paletteNames[] = {
         tr("Spartan Tier 1", "MainTreeview"),
         tr("Spartan Tier 2", "MainTreeview"),
         tr("Spartan Tier 3", "MainTreeview"),
         tr("Elite Tier 1", "MainTreeview"),
         tr("Elite Tier 2", "MainTreeview"),
         tr("Elite Tier 3", "MainTreeview"),
      };
      for (uint8_t i = 0; i < std::extent<decltype(paletteNames)>::value; i++) {
         if (text == paletteNames[i]) {
            this->switchToLoadoutPalette(&mp_data->options.loadouts.palettes[i]);
            return;
         }
      }
   }
   if (text == tr("Option Visibility", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionToggles);
      return;
   }
   if (text == tr("Title Update Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageTitleUpdateConfig);
      return;
   }
   if (text == tr("Forge Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageForge);
      return;
   }
}
void ReachVariantTool::refreshWidgetsFromVariant() {
   auto& editor = ReachEditorState::get();
   auto variant = editor.currentVariant;
   if (!variant)
      return;
   auto mp_data = variant ? variant->multiplayer.data->as_multiplayer() : nullptr;
   if (!mp_data)
      return;
   {  // Metadata
      const QSignalBlocker blocker0(this->ui.headerName);
      const QSignalBlocker blocker1(this->ui.headerDesc);
      const QSignalBlocker blocker2(this->ui.authorGamertag);
      const QSignalBlocker blocker3(this->ui.editorGamertag);
      const QSignalBlocker blocker4(this->ui.createdOnDate);
      const QSignalBlocker blocker5(this->ui.editedOnDate);
      this->ui.headerName->setText(QString::fromUtf16(mp_data->variantHeader.title));
      this->ui.headerDesc->setPlainText(QString::fromUtf16(mp_data->variantHeader.description));
      this->ui.authorGamertag->setText(QString::fromLatin1(mp_data->variantHeader.createdBy.author));
      this->ui.editorGamertag->setText(QString::fromLatin1(mp_data->variantHeader.modifiedBy.author));
      //
      {
         const QSignalBlocker blocker0(this->ui.engineIcon);
         const QSignalBlocker blocker1(this->ui.engineCategory);
         this->ui.engineIcon->setCurrentIndex(mp_data->engineIcon);
         int index = this->ui.engineCategory->findData((int)mp_data->engineCategory);
         if (index != -1)
            this->ui.engineCategory->setCurrentIndex(index);
      }
      //
      QDateTime temp;
      temp.setSecsSinceEpoch(mp_data->variantHeader.createdBy.timestamp);
      this->ui.createdOnDate->setDateTime(temp);
      temp.setSecsSinceEpoch(mp_data->variantHeader.modifiedBy.timestamp);
      this->ui.editedOnDate->setDateTime(temp);
   }
   //
   // This would probably be a whole lot cleaner if we could use pointers-to-members-of-members, 
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
         widget->setCurrentIndex( mp_data->##field ); \
      };
   #define reach_main_window_update_spinbox(w, field) \
      { \
         auto widget = w; \
         const QSignalBlocker blocker(widget); \
         widget->setValue( mp_data->##field ); \
      };
   #define reach_main_window_update_flag_checkbox(w, field, mask) \
      { \
         auto widget = w; \
         const QSignalBlocker blocker(widget); \
         widget->setChecked(( mp_data->##field & mask ) != 0); \
      };
   #define reach_main_window_update_bool_checkbox(w, field) \
      { \
         auto widget = w; \
         const QSignalBlocker blocker(widget); \
         widget->setChecked( mp_data->##field ); \
      };
   #pragma endregion
   {  // General
      reach_main_window_update_flag_checkbox(this->ui.optionsGeneralTeamsEnabled,          options.misc.flags, 1);
      reach_main_window_update_flag_checkbox(this->ui.optionsGeneralNewRoundResetsPlayers, options.misc.flags, 2);
      reach_main_window_update_flag_checkbox(this->ui.optionsGeneralNewRoundResetsMap,     options.misc.flags, 4);
      reach_main_window_update_flag_checkbox(this->ui.optionsGeneralFlag3,                 options.misc.flags, 8);
      reach_main_window_update_spinbox(this->ui.optionsGeneralRoundTimeLimit,  options.misc.timeLimit);
      reach_main_window_update_spinbox(this->ui.optionsGeneralRoundLimit,      options.misc.roundLimit);
      reach_main_window_update_spinbox(this->ui.optionsGeneralRoundsToWin,     options.misc.roundsToWin);
      reach_main_window_update_spinbox(this->ui.optionsGeneralSuddenDeathTime, options.misc.suddenDeathTime);
      reach_main_window_update_spinbox(this->ui.optionsGeneralGracePeriod,     options.misc.gracePeriod);
   }
   {  // Respawn
      reach_main_window_update_flag_checkbox(this->ui.optionsRespawnSyncWithTeam,   options.respawn.flags, 1);
      reach_main_window_update_flag_checkbox(this->ui.optionsRespawnFlag1,          options.respawn.flags, 2);
      reach_main_window_update_flag_checkbox(this->ui.optionsRespawnFlag2,          options.respawn.flags, 4);
      reach_main_window_update_flag_checkbox(this->ui.optionsRespawnRespawnOnKills, options.respawn.flags, 8);
      reach_main_window_update_spinbox(this->ui.optionsRespawnLivesPerRound,     options.respawn.livesPerRound);
      reach_main_window_update_spinbox(this->ui.optionsRespawnTeamLivesPerRound, options.respawn.teamLivesPerRound);
      reach_main_window_update_spinbox(this->ui.optionsRespawnRespawnTime,       options.respawn.respawnTime);
      reach_main_window_update_spinbox(this->ui.optionsRespawnSuicidePenalty,    options.respawn.suicidePenalty);
      reach_main_window_update_spinbox(this->ui.optionsRespawnBetrayalPenalty,   options.respawn.betrayalPenalty);
      reach_main_window_update_spinbox(this->ui.optionsRespawnRespawnGrowth,     options.respawn.respawnGrowth);
      reach_main_window_update_spinbox(this->ui.optionsRespawnLoadoutCamTime,    options.respawn.loadoutCamTime);
      reach_main_window_update_spinbox(this->ui.optionsRespawnTraitsDuration,    options.respawn.traitsDuration);
   }
   {  // Social
      reach_main_window_update_bool_checkbox(this->ui.optionsSocialObservers, options.social.observers);
      reach_main_window_update_spinbox(this->ui.optionsSocialTeamChanging, options.social.teamChanges);
      reach_main_window_update_flag_checkbox(this->ui.optionsSocialFriendlyFire,    options.social.flags, 0x01);
      reach_main_window_update_flag_checkbox(this->ui.optionsSocialBetrayalBooting, options.social.flags, 0x02);
      reach_main_window_update_flag_checkbox(this->ui.optionsSocialProximityVoice,  options.social.flags, 0x04);
      reach_main_window_update_flag_checkbox(this->ui.optionsSocialGlobalVoice,     options.social.flags, 0x08);
      reach_main_window_update_flag_checkbox(this->ui.optionsSocialDeadPlayerVoice, options.social.flags, 0x10);
   }
   {  // Map and Game
      reach_main_window_update_flag_checkbox(this->ui.optionsMapGrenadesEnabled, options.map.flags, 0x01);
      reach_main_window_update_flag_checkbox(this->ui.optionsMapShortcutsEnabled, options.map.flags, 0x02);
      reach_main_window_update_flag_checkbox(this->ui.optionsMapAbilitiesEnabled, options.map.flags, 0x04);
      reach_main_window_update_flag_checkbox(this->ui.optionsMapPowerupsEnabled, options.map.flags, 0x08);
      reach_main_window_update_flag_checkbox(this->ui.optionsMapTurretsEnabled, options.map.flags, 0x10);
      reach_main_window_update_flag_checkbox(this->ui.optionsMapIndestructibleVehicles, options.map.flags, 0x20);
      {  // Weapon Set
         QComboBox* widget = this->ui.optionsMapWeaponSet;
         const QSignalBlocker blocker(widget);
         auto value = mp_data->options.map.weaponSet;
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
         auto value = mp_data->options.map.vehicleSet;
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
      reach_main_window_update_spinbox(this->ui.optionsMapRedPowerupDuration, options.map.powerups.red.duration);
      reach_main_window_update_spinbox(this->ui.optionsMapBluePowerupDuration, options.map.powerups.blue.duration);
      reach_main_window_update_spinbox(this->ui.optionsMapYellowPowerupDuration, options.map.powerups.yellow.duration);
   }
   {  // Team
      reach_main_window_update_spinbox(this->ui.optionsTeamScoringMethod, options.team.scoring);
      reach_main_window_update_combobox(this->ui.optionsTeamPlayerSpecies, options.team.species);
      reach_main_window_update_spinbox(this->ui.optionsTeamDesignatorSwitchType, options.team.designatorSwitchType);
   }
   {  // Loadout Base Options
      reach_main_window_update_flag_checkbox(this->ui.optionsLoadoutFlag0, options.loadouts.flags, 0x01);
      reach_main_window_update_flag_checkbox(this->ui.optionsLoadoutFlag1, options.loadouts.flags, 0x02);
   }
   {  // Title Update Config
      reach_main_window_update_flag_checkbox(this->ui.titleUpdateBleedthrough, titleUpdateData.flags, 0x01);
      reach_main_window_update_flag_checkbox(this->ui.titleUpdateArmorLockCantShed, titleUpdateData.flags, 0x02);
      reach_main_window_update_flag_checkbox(this->ui.titleUpdateArmorLockCanBeStuck, titleUpdateData.flags, 0x04);
      reach_main_window_update_flag_checkbox(this->ui.titleUpdateEnableActiveCamoModifiers, titleUpdateData.flags, 0x08);
      reach_main_window_update_flag_checkbox(this->ui.titleUpdateLimitSwordBlockToSword, titleUpdateData.flags, 0x10);
      reach_main_window_update_flag_checkbox(this->ui.titleUpdateAutomaticMagnum, titleUpdateData.flags, 0x20);
      reach_main_window_update_flag_checkbox(this->ui.titleUpdateFlag6, titleUpdateData.flags, 0x40);
      reach_main_window_update_flag_checkbox(this->ui.titleUpdateFlag7, titleUpdateData.flags, 0x80);
      {  // Precision Bloom
         auto widget = this->ui.titleUpdatePrecisionBloom;
         const QSignalBlocker blocker(widget);
         widget->setValue(mp_data->titleUpdateData.precisionBloom * 100.0F / 5.0F); // normalize to percentage of vanilla
      }
      reach_main_window_update_spinbox(this->ui.titleUpdateArmorLockDamageDrain, titleUpdateData.armorLockDamageDrain);
      reach_main_window_update_spinbox(this->ui.titleUpdateArmorLockDamageDrainLimit, titleUpdateData.armorLockDamageDrainLimit);
      reach_main_window_update_spinbox(this->ui.titleUpdateActiveCamoEnergyBonus, titleUpdateData.activeCamoEnergyBonus);
      reach_main_window_update_spinbox(this->ui.titleUpdateActiveCamoEnergy, titleUpdateData.activeCamoEnergy);
      reach_main_window_update_spinbox(this->ui.titleUpdateMagnumDamage, titleUpdateData.magnumDamage);
      reach_main_window_update_spinbox(this->ui.titleUpdateMagnumFireDelay, titleUpdateData.magnumFireDelay);
   }
   {  // Forge
      const QSignalBlocker blocker(this->ui.forgeEnable);
      this->ui.forgeEnable->setChecked(mp_data->isForge);
      //
      reach_main_window_update_flag_checkbox(this->ui.forgeFlagOpenVoice, forgeData.flags, 0x01);
      reach_main_window_update_flag_checkbox(this->ui.forgeFlag1,         forgeData.flags, 0x02);
      reach_main_window_update_spinbox(this->ui.forgeRespawnTime, forgeData.respawnTime);
      reach_main_window_update_combobox(this->ui.forgeEditModeAccess, forgeData.editModeType);
   }
   //
   #undef reach_main_window_update_combobox
   #undef reach_main_window_update_spinbox
   #undef reach_main_window_update_flag_checkbox
   #undef reach_main_window_update_bool_checkbox
}
void ReachVariantTool::switchToLoadoutPalette(ReachLoadoutPalette* palette) {
   ReachEditorState::get().start_editing_loadouts(palette);
   this->refreshWidgetsForLoadoutPalette();
   this->ui.MainContentView->setCurrentWidget(this->ui.PageLoadoutPalette);
}
void ReachVariantTool::switchToPlayerTraits(ReachPlayerTraits* traits) {
   ReachEditorState::get().start_editing_traits(traits);
   this->refreshWidgetsForPlayerTraits();
   this->ui.MainContentView->setCurrentWidget(this->ui.PagePlayerTraits);
}
void ReachVariantTool::refreshWidgetsForLoadoutPalette() {
   this->ui.loadout1Content->pullFromGameVariant();
   this->ui.loadout2Content->pullFromGameVariant();
   this->ui.loadout3Content->pullFromGameVariant();
   this->ui.loadout4Content->pullFromGameVariant();
   this->ui.loadout5Content->pullFromGameVariant();
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
      reach_traits_pane_update_combobox(this->ui.playerTraitCannotDieFromDamage, defense.cannotDieFromDamage);
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
      reach_traits_pane_update_combobox(this->ui.playerTraitAbilityUsage, offense.abilityUsage);
      reach_traits_pane_update_combobox(this->ui.playerTraitAbilitiesDropOnDeath, offense.abilitiesDropOnDeath);
      reach_traits_pane_update_combobox(this->ui.playerTraitInfiniteAbility, offense.infiniteAbility);
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
void ReachVariantTool::refreshScriptedPlayerTraitList() {
   auto tree   = this->ui.MainTreeview;
   QTreeWidgetItem* branch;
   {
      auto list = tree->findItems(tr("Script-Specific Options", "MainTreeview"), Qt::MatchRecursive, 0);
      if (!list.size())
         return;
      branch = list[0];
   }
   for (QTreeWidgetItem* child : branch->takeChildren())
      delete child;
   auto data = ReachEditorState::get().get_multiplayer_data();
   if (!data)
      return;
   auto& t = data->scriptData.traits;
   for (size_t i = 0; i < t.size(); i++) {
      auto item = new QTreeWidgetItem(branch, QTreeWidgetItem::UserType + i);
      auto name = t[i].name;
      QString text;
      if (name) {
         text = QString::fromUtf8(name->english().c_str());
      } else {
         text = QString("Unnamed Traits %1").arg(i);
      }
      item->setText(0, text);
   }
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
      QString variantTitle;
      //
      auto mp = editor.get_multiplayer_data();
      if (mp) {
         //
         // Prefer getting the title from the mpvr block when possible, because it's always 
         // big-endian there. The endianness is inconsistent in chdr (old modded variants 
         // packaged for 360 may use little-endian) and the block header doesn't have any 
         // data that could clue us into when it's little-endian (i.e. the version and flags 
         // are the same as in big-endian MCC-era blocks).
         //
         variantTitle = QString::fromUtf16(mp->variantHeader.title);
      } else {
         variantTitle = QString::fromUtf16(editor.currentVariant->contentHeader.data.title);
      }
      this->setWindowTitle(
         QString("%1 <%2> - ReachVariantTool").arg(variantTitle).arg(file)
      );
   } else {
      this->setWindowTitle(
         QString("%1 - ReachVariantTool").arg(file)
      );
   }
}
//
namespace {
   void _onMegaloComboboxChange(QComboBox* widget, int index) {
      auto data = ReachEditorState::get().get_multiplayer_data();
      if (!data)
         return;
      auto v = widget->property("MegaloOptionIndex");
      if (!v.isValid())
         return;
      int32_t i = v.toInt();
      if (i < 0)
         return;
      auto& list   = data->scriptData.options;
      if (i >= list.size())
         return;
      auto& option = data->scriptData.options[i];
      option.currentValueIndex = index;
   }
   void _onMegaloSliderChange(QSlider* widget, int value) {
      auto data = ReachEditorState::get().get_multiplayer_data();
      if (!data)
         return;
      auto v = widget->property("MegaloOptionIndex");
      if (!v.isValid())
         return;
      int32_t i = v.toInt();
      if (i < 0)
         return;
      auto& list = data->scriptData.options;
      if (i >= list.size())
         return;
      auto& option = data->scriptData.options[i];
      option.rangeCurrent = value;
   }
}
void ReachVariantTool::setupWidgetsForScriptedOptions() {
   auto page   = this->ui.PageOptionsScripted;
   auto layout = dynamic_cast<QGridLayout*>(page->layout());
   if (!layout) {
      layout = new QGridLayout;
      page->setLayout(layout);
   }
   {  // Clear layout
      QLayoutItem* child;
      while ((child = layout->takeAt(0)) != nullptr)
         delete child;
   }
   auto data = ReachEditorState::get().get_multiplayer_data();
   if (!data)
      return;
   const auto& options = data->scriptData.options;
   for (uint32_t i = 0; i < options.size(); i++) {
      auto& option = options[i];
      auto  desc   = option.desc;
      //
      auto label = new QLabel(this);
      label->setProperty("MegaloOptionIndex", i);
      if (option.name) {
         label->setText(QString::fromUtf8(option.name->english().c_str()));
      } else {
         label->setText(QString("Unnamed Option #%1").arg(i + 1));
      }
      layout->addWidget(label, i, 0);
      if (desc)
         label->setToolTip(QString::fromUtf8(desc->english().c_str()));
      //
      if (option.isRange) {
         auto slider = new QSlider(Qt::Horizontal, this);
         slider->setProperty("MegaloOptionIndex", i);
         slider->setMinimum(option.rangeMin.value);
         slider->setMaximum(option.rangeMax.value);
         slider->setValue(option.rangeCurrent);
         layout->addWidget(slider, i, 1);
         //
         // TODO: display default value?
         //
         QObject::connect(slider, QOverload<int>::of(&QSlider::valueChanged), [slider](int v) { _onMegaloSliderChange(slider, v); });
         if (desc)
            slider->setToolTip(QString::fromUtf8(desc->english().c_str()));
         label->setBuddy(slider);
      } else {
         auto combo = new QComboBox(this);
         combo->setProperty("MegaloOptionIndex", i);
         for (uint32_t j = 0; j < option.values.size(); j++) {
            auto& value = option.values[j];
            if (value.name) {
               combo->addItem(QString::fromUtf8(value.name->english().c_str()), (int16_t)value.value);
            } else {
               combo->addItem(QString(value.value), (int16_t)value.value);
            }
         }
         combo->setCurrentIndex(option.currentValueIndex);
         layout->addWidget(combo, i, 1);
         //
         // TODO: display default value?
         //
         QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), [combo](int v) { _onMegaloComboboxChange(combo, v); });
         if (desc)
            combo->setToolTip(QString::fromUtf8(desc->english().c_str()));
         label->setBuddy(combo);
      }
   }
   auto spacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
   layout->addItem(spacer, options.size(), 0, 1, 2);
}

void ReachVariantTool::refreshTeamColorWidgets() {
   auto team = this->_getCurrentTeam();
   if (!team)
      return;
   //
   const QString style("QPushButton { background-color : %1; }");
   //
   this->ui.specificTeamButtonColorPrimary->setStyleSheet(style.arg(_colorFromReach(team->colorPrimary).name()));
   this->ui.specificTeamButtonColorSecondary->setStyleSheet(style.arg(_colorFromReach(team->colorSecondary).name()));
   this->ui.specificTeamButtonColorText->setStyleSheet(style.arg(_colorFromReach(team->colorText).name()));
}
void ReachVariantTool::switchToTeam(int8_t teamIndex) {
   this->currentTeam = teamIndex;
   if (teamIndex < 0)
      return;
   this->ui.MainContentView->setCurrentWidget(this->ui.PageSpecificTeamConfig);
   //
   auto team = this->_getCurrentTeam();
   //
   #pragma region Preprocessor macros to update Specific Team widgets
   #define reach_team_pane_update_flag_checkbox(w, field, mask) \
      { \
         QCheckBox* widget = w; \
         const QSignalBlocker blocker(widget); \
         widget->setChecked((team->##field & mask) != 0); \
      };
   #define reach_team_pane_update_combobox(w, field) \
      { \
         auto widget = w; \
         const QSignalBlocker blocker(widget); \
         widget->setCurrentIndex(team->##field ); \
      };
   #define reach_team_pane_update_spinbox(w, field) \
      { \
         auto widget = w; \
         const QSignalBlocker blocker(widget); \
         widget->setValue(team->##field ); \
      };
   #pragma endregion
   {  // Show localized team name
      auto name = team->get_name();
      QString text;
      if (name)
         text = QString::fromUtf8(name->english().c_str());
      this->ui.specificTeamNameLabel->setText(text);
   }
   //
   reach_team_pane_update_flag_checkbox(this->ui.specificTeamFlagEnabled,        flags, 1);
   reach_team_pane_update_flag_checkbox(this->ui.specificTeamFlagColorPrimary,   flags, 2);
   reach_team_pane_update_flag_checkbox(this->ui.specificTeamFlagColorSecondary, flags, 4);
   reach_team_pane_update_flag_checkbox(this->ui.specificTeamFlagColorText,      flags, 8);
   reach_team_pane_update_combobox(this->ui.specificTeamSpecies, spartanOrElite);
   reach_team_pane_update_spinbox(this->ui.specificTeamFireteamCount, fireteamCount);
   reach_team_pane_update_spinbox(this->ui.specificTeamInitialDesignator, initialDesignator);
   //
   #undef reach_team_pane_update_flag_checkbox
   #undef reach_team_pane_update_combobox
   #undef reach_team_pane_update_spinbox
   //
   this->refreshTeamColorWidgets();
}
ReachTeamData* ReachVariantTool::_getCurrentTeam() const noexcept {
   if (this->currentTeam < 0)
      return nullptr;
   auto data = ReachEditorState::get().get_multiplayer_data();
   if (!data)
      return nullptr;
   return &data->options.team.teams[this->currentTeam];
}