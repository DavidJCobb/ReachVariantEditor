#include "ReachVariantTool.h"
#include <cassert>
#include <QFileDialog>
#include <QMessageBox>
#include "editor_state.h"
#include "helpers/stream.h"

ReachVariantTool::ReachVariantTool(QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    //
    QObject::connect(this->ui.actionOpen,   &QAction::triggered, this, &ReachVariantTool::openFile);
    QObject::connect(this->ui.actionSaveAs, &QAction::triggered, this, &ReachVariantTool::saveFile);
}

void ReachVariantTool::openFile() {
   auto& editor = ReachEditorState::get();
   //
   // TODO: warn on unsaved changes
   //
   QString fileName = QFileDialog::getOpenFileName(this, tr("Open Game Variant"), "", tr("Game Variant (*.bin);;All Files (*)"));
   if (fileName.isEmpty())
      return;
   std::wstring s = fileName.toStdWString();
   auto file    = cobb::mapped_file(s.c_str());
   s.clear();
   auto variant = new GameVariant();
   if (!variant->read(file)) {
      QMessageBox::information(this, tr("Unable to open file"), tr("Failed to read the game variant data."));
      return;
   }
   editor.take_game_variant(variant);
   //
   // TODO: trigger mass update of all UI to show new game variant data
   //
}
void ReachVariantTool::saveFile() {
   auto& editor = ReachEditorState::get();
   if (!editor.currentVariant) {
      QMessageBox::information(this, tr("No game variant is open"), tr("We do not currently support creating game variants from scratch. Open a variant and then you can save an edited copy of it."));
      return;
   }
   //
   QString fileName = QFileDialog::getSaveFileName(this, tr("Save Game Variant"), "", tr("Game Variant (*.bin);;All Files (*)"));
   if (fileName.isEmpty())
      return;
   QFile file(fileName);
   if (!file.open(QIODevice::WriteOnly)) {
      QMessageBox::information(this, tr("Unable to open file for writing"), file.errorString());
      return;
   }
   QDataStream out(&file);
   out.setVersion(QDataStream::Qt_4_5);
   //
   cobb::bit_or_byte_writer writer;
   editor.currentVariant->write(writer);
   out.writeRawData((const char*)writer.bytes.data(), writer.bytes.get_bytespan());
}