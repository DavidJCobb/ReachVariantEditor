#include "ReachVariantTool.h"
#include <cassert>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QTreeWidget>
#include "editor_state.h"
#include "helpers/stream.h"

ReachVariantTool::ReachVariantTool(QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    //
    QObject::connect(this->ui.actionOpen,   &QAction::triggered, this, &ReachVariantTool::openFile);
    QObject::connect(this->ui.actionSaveAs, &QAction::triggered, this, &ReachVariantTool::saveFile);
    //
    QObject::connect(this->ui.MainTreeview, &QTreeWidget::itemSelectionChanged, this, &ReachVariantTool::onSelectedPageChanged);
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
   this->refreshWidgetsFromVariant();
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
void ReachVariantTool::onSelectedPageChanged() {
   auto widget     = this->ui.MainTreeview;
   auto selections = widget->selectedItems();
   if (!selections.size())
      return;
   auto sel   = selections[0];
   auto text  = sel->text(0);
   auto stack = this->ui.MainContentView;
   if (text == tr("Metadata", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageGameVariantHeader);
      return;
   }
   if (text == tr("General Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsGeneral);
      return;
   }
   //
   // TODO: add other panes
   //
}
void ReachVariantTool::refreshWidgetsFromVariant() {
   auto& editor = ReachEditorState::get();
   auto variant = editor.currentVariant;
   if (!variant)
      return;
   this->isUpdatingFromVariant = true;
   {  // Metadata
      this->ui.headerName->setText(QString::fromWCharArray(variant->multiplayer.variantHeader.title));
      this->ui.headerDesc->setPlainText(QString::fromWCharArray(variant->multiplayer.variantHeader.description));
      this->ui.authorGamertag->setText(QString::fromLatin1(variant->multiplayer.variantHeader.createdBy.author));
      this->ui.editorGamertag->setText(QString::fromLatin1(variant->multiplayer.variantHeader.modifiedBy.author));
   }
   {  // General Settings
      auto& o = variant->multiplayer.options.misc;
      auto  f = o.flags;
      //
      // TODO: Flag 0 may not actually be Teams Enabled? Or at least, it may not be the only team-enabling flag?
      //
      this->ui.optionsGeneralTeamsEnabled->setChecked(f & 1);
      this->ui.optionsGeneralNewRoundResetsPlayers->setChecked(f & 2);
      this->ui.optionsGeneralNewRoundResetsMap->setChecked(f & 4);
      this->ui.optionsGeneralFlag3->setChecked(f & 8);
      this->ui.optionsGeneralRoundTimeLimit->setValue(o.timeLimit);
      this->ui.optionsGeneralRoundLimit->setValue(o.roundLimit);
      this->ui.optionsGeneralRoundsToWin->setValue(o.roundsToWin);
      this->ui.optionsGeneralSuddenDeathTime->setValue(o.suddenDeathTime);
      this->ui.optionsGeneralGracePeriod->setValue(o.gracePeriod);
   }
   this->isUpdatingFromVariant = false;
}