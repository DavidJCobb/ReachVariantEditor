#pragma once
#include "main_window/ui_page_multiplayer_settings_social.h"
#include <QWidget>
#include "../../editor_state.h"

class PageMPSettingsSocial : public QWidget {
   Q_OBJECT
   //
   public:
      PageMPSettingsSocial(QWidget* parent = nullptr);
      //
   private:
      Ui::PageMPSettingsSocial ui;
      //
      void updateFromVariant(GameVariant*);
};