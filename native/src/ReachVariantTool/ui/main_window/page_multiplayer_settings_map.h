#pragma once
#include "ui/main_window/ui_page_multiplayer_settings_map.h"
#include <QWidget>
#include "editor_state.h"

class PageMPSettingsMapAndGame : public QWidget {
   Q_OBJECT
   //
   public:
      PageMPSettingsMapAndGame(QWidget* parent = nullptr);
      //
   private:
      Ui::PageMPSettingsMapAndGame ui;
      //
      void updateFromVariant(GameVariant*);
};