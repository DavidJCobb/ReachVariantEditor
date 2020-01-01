#pragma once
#include "main_window/ui_page_multiplayer_settings_loadout.h"
#include <QWidget>
#include "editor_state.h"

class PageMPSettingsLoadoutsOverall : public QWidget {
   Q_OBJECT
   //
   public:
      PageMPSettingsLoadoutsOverall(QWidget* parent = nullptr);
      //
   private:
      Ui::PageMPSettingsLoadoutsOverall ui;
      //
      void updateFromVariant(GameVariant*);
};