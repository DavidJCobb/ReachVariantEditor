#pragma once
#include <QGridLayout>
#include <QWidget>
#include "editor_state.h"

class ReachMegaloOption;

class PageMPSettingsScripted : public QWidget {
   Q_OBJECT
   //
   public:
      PageMPSettingsScripted(QWidget* parent = nullptr);
      //
   private:
      void _renderOptionRow(uint32_t index, QGridLayout* layout, ReachMegaloOption& option);
      void updateFromVariant(GameVariant*);
      void updateFromOption(ReachMegaloOption*);
};