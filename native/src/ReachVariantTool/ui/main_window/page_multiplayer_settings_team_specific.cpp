#include "page_multiplayer_settings_team_specific.h"
#include <QColorDialog>
#include "../localized_string_editor.h"

namespace {
   QColor _colorFromReach(uint32_t c) {
      return QColor((c >> 0x10) & 0xFF, (c >> 0x08) & 0xFF, c & 0xFF);
   }
   uint32_t _colorToReach(const QColor& c) {
      return ((c.red() & 0xFF) << 0x10) | ((c.green() & 0xFF) << 0x08) | (c.blue() & 0xFF);
   }
}

PageMPSettingsTeamSpecific::PageMPSettingsTeamSpecific(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::switchedMultiplayerTeam, this, &PageMPSettingsTeamSpecific::updateFromVariant);
   QObject::connect(&editor, &ReachEditorState::teamColorModified,       this, &PageMPSettingsTeamSpecific::updateTeamColor);
   //
   QObject::connect(this->ui.buttonName, &QPushButton::clicked, [this]() {
      auto& editor = ReachEditorState::get();
      auto  team   = editor.multiplayerTeam();
      if (!team)
         return;
      ReachString* string = team->get_name();
      if (!string)
         return;
      if (LocalizedStringEditorModal::editString(this, ReachStringFlags::SingleLanguageString | ReachStringFlags::IsNotInStandardTable, string)) {
         this->ui.fieldName->setText(QString::fromUtf8(string->english().c_str()));
      }
   });
   //
   #include "widget_macros_setup_start.h"
   reach_team_pane_setup_flag_checkbox(this->ui.fieldEnabled,              flags, 1);
   reach_team_pane_setup_flag_checkbox(this->ui.fieldEnableColorPrimary,   flags, 2);
   reach_team_pane_setup_flag_checkbox(this->ui.fieldEnableColorSecondary, flags, 4);
   reach_team_pane_setup_flag_checkbox(this->ui.fieldEnableColorText,      flags, 8);
   reach_team_pane_setup_combobox(this->ui.fieldSpecies, spartanOrElite);
   reach_team_pane_setup_spinbox(this->ui.fieldFireteamCount, fireteamCount);
   reach_team_pane_setup_spinbox(this->ui.fieldInitialDesignator, initialDesignator);
   QObject::connect(this->ui.fieldButtonColorPrimary, &QPushButton::clicked, [this]() {
      auto& editor = ReachEditorState::get();
      auto  team   = editor.multiplayerTeam();
      if (!team)
         return;
      auto color = QColorDialog::getColor(_colorFromReach(team->colorPrimary), this->window());
      if (!color.isValid())
         return;
      team->colorPrimary = _colorToReach(color);
      editor.teamColorModified(team);
   });
   QObject::connect(this->ui.fieldButtonColorSecondary, &QPushButton::clicked, [this]() {
      auto& editor = ReachEditorState::get();
      auto  team   = editor.multiplayerTeam();
      if (!team)
         return;
      auto color = QColorDialog::getColor(_colorFromReach(team->colorSecondary), this->window());
      if (!color.isValid())
         return;
      team->colorSecondary = _colorToReach(color);
      editor.teamColorModified(team);
   });
   QObject::connect(this->ui.fieldButtonColorText, &QPushButton::clicked, [this]() {
      auto& editor = ReachEditorState::get();
      auto  team   = editor.multiplayerTeam();
      if (!team)
         return;
      auto color = QColorDialog::getColor(_colorFromReach(team->colorText), this->window());
      if (!color.isValid())
         return;
      team->colorText = _colorToReach(color);
      editor.teamColorModified(team);
   });
   #include "widget_macros_setup_end.h"
}
void PageMPSettingsTeamSpecific::updateFromVariant(GameVariant* variant, int8_t teamIndex, ReachTeamData* team) {
   if (!team)
      return;
   #include "widget_macros_update_start.h"
   {
      auto name = team->get_name();
      if (name) {
         this->ui.fieldName->setText(QString::fromUtf8(name->english().c_str()));
         this->ui.buttonName->setDisabled(false);
      } else {
         this->ui.fieldName->setText("");
         this->ui.buttonName->setDisabled(true);
      }
   }
   reach_team_pane_update_flag_checkbox(this->ui.fieldEnabled,              flags, 1);
   reach_team_pane_update_flag_checkbox(this->ui.fieldEnableColorPrimary,   flags, 2);
   reach_team_pane_update_flag_checkbox(this->ui.fieldEnableColorSecondary, flags, 4);
   reach_team_pane_update_flag_checkbox(this->ui.fieldEnableColorText,      flags, 8);
   reach_team_pane_update_combobox(this->ui.fieldSpecies, spartanOrElite);
   reach_team_pane_update_spinbox(this->ui.fieldFireteamCount, fireteamCount);
   reach_team_pane_update_spinbox(this->ui.fieldInitialDesignator, initialDesignator);
   #include "widget_macros_update_end.h"
   this->updateTeamColor(team);
}
void PageMPSettingsTeamSpecific::updateTeamColor(ReachTeamData* team) {
   if (!team)
      return;
   const QString style("QPushButton { background-color : %1; }");
   this->ui.fieldButtonColorPrimary->setStyleSheet(style.arg(_colorFromReach(team->colorPrimary).name()));
   this->ui.fieldButtonColorSecondary->setStyleSheet(style.arg(_colorFromReach(team->colorSecondary).name()));
   this->ui.fieldButtonColorText->setStyleSheet(style.arg(_colorFromReach(team->colorText).name()));
}