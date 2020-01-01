#pragma once
#include "main_window/ui_page_multiplayer_settings_team_specific.h"
#include <QWidget>
#include "editor_state.h"

class PageMPSettingsTeamSpecific : public QWidget {
   Q_OBJECT
   //
   public:
      PageMPSettingsTeamSpecific(QWidget* parent = nullptr);
      //
   private:
      Ui::PageMPSettingsTeamSpecific ui;
      int8_t team = -1;
      //
      void updateFromVariant(GameVariant*, int8_t teamIndex, ReachTeamData* team);
      void updateTeamColor(ReachTeamData* team);
};