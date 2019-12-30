#pragma once
#include <QWidget>
#include "editor_state.h"

class PageMPSettingsScripted : public QWidget {
   Q_OBJECT
   //
   public:
      PageMPSettingsScripted(QWidget* parent = nullptr);
      //
   private:
      void updateFromVariant(GameVariant*);
};