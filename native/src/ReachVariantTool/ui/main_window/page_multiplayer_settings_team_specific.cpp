#include "page_multiplayer_settings_team_specific.h"
#include <QColorDialog>
#include "../../game_variants/base.h"
#include "../../game_variants/components/teams.h"
#include "../localized_string_editor.h"

#include <QDebug>

namespace {
   constexpr const std::array<std::pair<int, const char*>, 10> _team_designators = {
      std::pair{  0, "Defenders (Red)" },
      std::pair{  1, "Attackers (Blue)" },
      std::pair{  2, "Team 3 (Green)" },
      std::pair{  3, "Team 4 (Orange)" },
      std::pair{  4, "Team 5 (Purple)" },
      std::pair{  5, "Team 6 (Gold)" },
      std::pair{  6, "Team 7 (Brown)" },
      std::pair{  7, "Team 8 (Pink)" },
      std::pair{  8, "Neutral Team" },
      std::pair{ -1, "None" },
   };
}

namespace {
   QColor _colorFromReach(uint32_t c, bool use_alpha = false) {
      int a = use_alpha ? (c >> 0x18) : 255;
      return QColor((c >> 0x10) & 0xFF, (c >> 0x08) & 0xFF, c & 0xFF, a);
   }
   uint32_t _colorToReach(const QColor& c, bool use_alpha = false) {
      uint32_t result = ((c.red() & 0xFF) << 0x10) | ((c.green() & 0xFF) << 0x08) | (c.blue() & 0xFF);
      if (use_alpha)
         result |= c.alpha() << 0x18;
      return result;
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
      bool  created = false;
      auto* string  = team->get_name();
      if (!string) {
         created = true;
         string  = team->name.add_new(); // add a temporary string; if the user confirms changes, then we'll keep it
         if (!string)
            return;
      }
      auto index = string->index; // should always be zero
      if (LocalizedStringEditorModal::editString(this, ReachStringFlags::SingleLanguageString | ReachStringFlags::IsNotInStandardTable, string)) {
         auto& english = string->get_content(reach::language::english);
         //
         auto size = english.size();
         if (auto* table = string->get_owner()) { // Ugly hack to enforce max length, in case our unofficial QPlainTextEdit maxlength workaround ever fails somehow
            if (size > table->max_buffer_size) {
               auto content = english;
               content.resize(table->max_buffer_size);
               for (int i = 0; i < reach::language_count; ++i)
                  string->set_content((reach::language)i, content);
            }
         }
         //
         this->ui.fieldName->setText(QString::fromUtf8(english.c_str()));
         if (!english.size())
            //
            // I suspect the game would actually support zero-length team names, but don't allow that. If the 
            // name is cleared, revert to using the engine default.
            //
            if (index >= 0)
               team->name.remove(index);
      } else {
         if (created && index >= 0)
            team->name.remove(index);
      }
   });
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_flag_checkbox(multiplayerTeam, this->ui.fieldEnabled,              flags, ReachTeamData::Flags::enabled);
   reach_main_window_setup_flag_checkbox(multiplayerTeam, this->ui.fieldEnableColorPrimary,   flags, ReachTeamData::Flags::override_color_primary);
   reach_main_window_setup_flag_checkbox(multiplayerTeam, this->ui.fieldEnableColorSecondary, flags, ReachTeamData::Flags::override_color_secondary);
   reach_main_window_setup_flag_checkbox(multiplayerTeam, this->ui.fieldEnableColorText,      flags, ReachTeamData::Flags::override_color_text);
   reach_main_window_setup_combobox(multiplayerTeam,      this->ui.fieldSpecies,              spartanOrElite);
   reach_main_window_setup_spinbox(multiplayerTeam,       this->ui.fieldFireteamCount,        fireteamCount);

   this->ui.fieldInitialDesignator->clear();
   for (auto& dfn : _team_designators) {
      this->ui.fieldInitialDesignator->addItem(tr(dfn.second, "team designator names"), dfn.first);
   }
   QObject::connect(
      this->ui.fieldInitialDesignator,
      QOverload<int>::of(&QComboBox::currentIndexChanged),
      [this](int index) {
         auto  item_data = this->ui.fieldInitialDesignator->currentData().toInt();
         auto* team_data = ReachEditorState::get().multiplayerTeam();
         if (!team_data)
            return;
         team_data->initialDesignator = item_data;
      }
   );

   QObject::connect(this->ui.fieldButtonColorPrimary,   &QPushButton::clicked, [this]() {
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
      auto color = QColorDialog::getColor(_colorFromReach(team->colorText, true), this->window(), QString(), QColorDialog::ShowAlphaChannel);
      if (!color.isValid())
         return;
      team->colorText = _colorToReach(color, true);
      editor.teamColorModified(team);
   });
   #include "widget_macros_setup_end.h"
}
void PageMPSettingsTeamSpecific::updateFromVariant(GameVariant* variant, int8_t teamIndex, ReachTeamData* data) {
   if (!data) {
      this->ui.buttonName->setDisabled(true);
      return;
   }
   #include "widget_macros_update_start.h"
   {
      auto name = data->get_name();
      if (name)
         this->ui.fieldName->setText(QString::fromUtf8(name->get_content(reach::language::english).c_str()));
      else
         this->ui.fieldName->setText("");
      this->ui.buttonName->setDisabled(false);
   }
   reach_main_window_update_flag_checkbox(this->ui.fieldEnabled,              flags, ReachTeamData::Flags::enabled);
   reach_main_window_update_flag_checkbox(this->ui.fieldEnableColorPrimary,   flags, ReachTeamData::Flags::override_color_primary);
   reach_main_window_update_flag_checkbox(this->ui.fieldEnableColorSecondary, flags, ReachTeamData::Flags::override_color_secondary);
   reach_main_window_update_flag_checkbox(this->ui.fieldEnableColorText,      flags, ReachTeamData::Flags::override_color_text);
   reach_main_window_update_combobox(this->ui.fieldSpecies,           spartanOrElite);
   reach_main_window_update_spinbox(this->ui.fieldFireteamCount,     fireteamCount);
   {
      auto* widget = this->ui.fieldInitialDesignator;
      const QSignalBlocker blocker(widget);

      auto value = (int)data->initialDesignator;
      auto index = widget->findData(value);
      if (index >= 0)
         widget->setCurrentIndex(index);
      else
         widget->setCurrentIndex(widget->findData(-1));
   };
   #include "widget_macros_update_end.h"
   this->updateTeamColor(data);
}
void PageMPSettingsTeamSpecific::updateTeamColor(ReachTeamData* team) {
   if (!team)
      return;
   const QString style("QPushButton { background-color : %1; }");
   this->ui.fieldButtonColorPrimary->setStyleSheet(style.arg(_colorFromReach(team->colorPrimary).name()));
   this->ui.fieldButtonColorSecondary->setStyleSheet(style.arg(_colorFromReach(team->colorSecondary).name()));
   this->ui.fieldButtonColorText->setStyleSheet(style.arg(_colorFromReach(team->colorText, true).name()));
}