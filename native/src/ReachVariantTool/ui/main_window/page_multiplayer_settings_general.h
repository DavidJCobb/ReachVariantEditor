#pragma once
#include "main_window/ui_page_multiplayer_settings_general.h"
#include <QWidget>
#include "../../editor_state.h"

class PageMPSettingsGeneral : public QWidget {
   Q_OBJECT
   //
   public:
      PageMPSettingsGeneral(QWidget* parent = nullptr);
      //
   private:
      Ui::PageMPSettingsGeneral ui;
      //
      void updateFromVariant(GameVariant*);
};