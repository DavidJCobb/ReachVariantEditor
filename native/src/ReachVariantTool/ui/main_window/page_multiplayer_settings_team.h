#pragma once
#include "ui/main_window/ui_page_multiplayer_settings_team.h"
#include <QWidget>
#include "editor_state.h"

class PageMPSettingsTeamOverall : public QWidget {
   Q_OBJECT
   //
   public:
      PageMPSettingsTeamOverall(QWidget* parent = nullptr);
      //
   private:
      Ui::PageMPSettingsTeamOverall ui;
      //
      void updateFromVariant(GameVariant*);
};