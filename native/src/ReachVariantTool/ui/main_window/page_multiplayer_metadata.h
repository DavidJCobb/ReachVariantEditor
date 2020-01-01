#pragma once
#include "main_window/ui_page_multiplayer_metadata.h"
#include <QWidget>
#include "editor_state.h"

class PageMPMetadata : public QWidget {
   Q_OBJECT
   //
   public:
      PageMPMetadata(QWidget* parent = nullptr);
      //
   signals:
      void titleChanged(const char16_t* title);
      //
   private:
      Ui::PageMPMetadata ui;
      //
      void updateFromVariant(GameVariant*);
      void updateDescriptionCharacterCount();
};