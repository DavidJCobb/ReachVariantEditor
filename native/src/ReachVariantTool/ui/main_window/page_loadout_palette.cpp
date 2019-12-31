#include "page_loadout_palette.h"

PageLoadoutPaletteEditor::PageLoadoutPaletteEditor(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::switchedLoadoutPalette, this, &PageLoadoutPaletteEditor::updateFromPalette);
}
void PageLoadoutPaletteEditor::updateFromPalette(ReachLoadoutPalette* palette) {
   if (!palette)
      return;
   this->ui.loadout1Content->pullFromGameVariant();
   this->ui.loadout2Content->pullFromGameVariant();
   this->ui.loadout3Content->pullFromGameVariant();
   this->ui.loadout4Content->pullFromGameVariant();
   this->ui.loadout5Content->pullFromGameVariant();
}