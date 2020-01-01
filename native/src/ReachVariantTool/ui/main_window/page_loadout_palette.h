#pragma once
#include "main_window/ui_page_loadout_palette.h"
#include <QWidget>
#include "editor_state.h"

class PageLoadoutPaletteEditor : public QWidget {
   Q_OBJECT
   //
   public:
      PageLoadoutPaletteEditor(QWidget* parent = nullptr);
      //
   private:
      Ui::PageLoadoutPaletteEditor ui;
      //
      void updateFromPalette(ReachLoadoutPalette*);
};