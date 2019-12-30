#pragma once
#include "ui/main_window/ui_page_forge_settings_general.h"
#include <QWidget>
#include "editor_state.h"

class PageForgeSettingsGeneral : public QWidget {
   Q_OBJECT
   //
   public:
      PageForgeSettingsGeneral(QWidget* parent = nullptr);
      //
   private:
      Ui::PageForgeSettingsGeneral ui;
      //
      void updateFromVariant(GameVariant*);
};