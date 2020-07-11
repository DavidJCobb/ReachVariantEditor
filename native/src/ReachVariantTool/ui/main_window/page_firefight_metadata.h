#pragma once
#include "main_window/ui_page_firefight_metadata.h"
#include <QWidget>
#include "../../editor_state.h"

class PageFFMetadata : public QWidget {
   Q_OBJECT
   //
   public:
      PageFFMetadata(QWidget* parent = nullptr);
      //
   signals:
      void titleChanged(const char16_t* title);
      //
   private:
      Ui::PageFFMetadata ui;
      //
      void updateFromVariant(GameVariant*);
      void updateDescriptionCharacterCount();
};