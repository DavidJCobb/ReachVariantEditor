#pragma once
#include "ui/main_window/ui_page_multiplayer_title_update_1.h"
#include <QWidget>
#include "editor_state.h"

class PageMPTU1Config : public QWidget {
   Q_OBJECT
   //
   public:
      PageMPTU1Config(QWidget* parent = nullptr);
      //
   private:
      Ui::PageMPTU1Config ui;
      //
      void updateFromVariant(GameVariant*);
};