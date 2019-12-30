#pragma once
#include "ui/main_window/ui_page_multiplayer_settings_respawn.h"
#include <QWidget>
#include "editor_state.h"

class PageMPSettingsRespawn : public QWidget {
   Q_OBJECT
   //
   public:
      PageMPSettingsRespawn(QWidget* parent = nullptr);
      //
   private:
      Ui::PageMPSettingsRespawn ui;
      //
      void updateFromVariant(GameVariant*);
};