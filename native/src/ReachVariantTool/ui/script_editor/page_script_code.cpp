#include "page_script_code.h"
#include "compiler_unresolved_strings.h"
#include <QMessageBox>
#include <QListWidget>
#include <QTextBlock>

ScriptEditorPageScriptCode::ScriptEditorPageScriptCode(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   //
   QObject::connect(this->ui.buttonDecompile, &QPushButton::clicked, [this]() {
      auto& editor  = ReachEditorState::get();
      auto  variant = editor.variant();
      if (!variant)
         return;
      auto mp = variant->get_multiplayer_data();
      if (!mp)
         return;
      Megalo::Decompiler decompiler(*variant);
      decompiler.decompile();
      //
      const QSignalBlocker blocker(this->ui.textEditor);
      this->ui.textEditor->setPlainText(decompiler.current_content);
   });
   QObject::connect(this->ui.buttonCompile, &QPushButton::clicked, [this]() {
      auto& editor  = ReachEditorState::get();
      auto  variant = editor.variant();
      if (!variant)
         return;
      auto mp = variant->get_multiplayer_data();
      if (!mp)
         return;
      Megalo::Compiler compiler(*mp);
      compiler.parse(this->ui.textEditor->toPlainText());
      //
      this->updateLog(compiler);
      if (!compiler.has_errors()) {
         auto& unresolved_str = compiler.get_unresolved_string_references();
         if (unresolved_str.size()) {
            if (!CompilerUnresolvedStringsDialog::handleCompiler(this, compiler)) {
               QMessageBox::information(this, tr("Unable to commit the compiled data"), tr("Unresolved string references were not handled."));
               this->_lastFatals.emplace_back(tr("Unable to commit the compiled data. Unresolved string references were not handled."), pos_t(-1, -1, -1));
               this->redrawLog();
               return;
            }
         }
         compiler.apply();
         ReachEditorState::get().variantRecompiled(variant);
         QMessageBox::information(this, tr("Compiled contents applied!"), tr("The compiled content has been applied."));
      }
   });
   QObject::connect(this->ui.compileLogFilterWarn, &QAbstractButton::toggled, [this](bool checked) {
      this->redrawLog();
   });
   QObject::connect(this->ui.compileLogFilterError, &QAbstractButton::toggled, [this](bool checked) {
      this->redrawLog();
   });
   QObject::connect(this->ui.buttonCopyCompileLog, &QPushButton::clicked, [this]() {
      this->ui.compileLog->copyAllToClipboard();
   });
   this->ui.compileLog->setCopyTransformFunctor([](QString& out, QListWidgetItem* item) {
      int line = item->data(Qt::ItemDataRole::UserRole + 1).toInt();
      int col  = item->data(Qt::ItemDataRole::UserRole + 2).toInt();
      if (line < 1 || col < 0)
         return;
      //
      // TODO: Prepend "warning", "error", or "fatal" based on the item's icon, too.
      //
      out.prepend(QString("Line %1 col %2: ").arg(line).arg(col));
   });
   QObject::connect(this->ui.compileLog, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* item) {
      int line = item->data(Qt::ItemDataRole::UserRole + 1).toInt();
      if (line < 1)
         return;
      --line;
      //
      auto editor = this->ui.textEditor;
      auto doc    = editor->document();
      QTextBlock  block  = doc->findBlockByLineNumber(line);
      QTextCursor cursor = editor->textCursor();
      cursor.setPosition(block.position());
      editor->setFocus();
      editor->setTextCursor(cursor);
   });
}
void ScriptEditorPageScriptCode::updateLog(Compiler& compiler) {
   this->_lastWarnings = compiler.get_warnings();
   this->_lastErrors   = compiler.get_non_fatal_errors();
   this->_lastFatals   = compiler.get_fatal_errors();
   //
   this->redrawLog();
}
void ScriptEditorPageScriptCode::redrawLog() {
   auto widget = this->ui.compileLog;
   //
   const QSignalBlocker blocker(widget);
   widget->clear();
   if (this->ui.compileLogFilterWarn->isChecked()) {
      for (auto& entry : this->_lastWarnings) {
         auto item = new QListWidgetItem;
         item->setText(entry.text);
         item->setData(Qt::ItemDataRole::UserRole + 0, entry.pos.offset);
         item->setData(Qt::ItemDataRole::UserRole + 1, entry.pos.line + 1);
         item->setData(Qt::ItemDataRole::UserRole + 2, entry.pos.col());
         // TODO: icon for warnings
         widget->addItem(item);
      }
   }
   if (this->ui.compileLogFilterError->isChecked()) {
      for (auto& entry : this->_lastErrors) {
         auto item = new QListWidgetItem;
         item->setText(entry.text);
         item->setData(Qt::ItemDataRole::UserRole + 0, entry.pos.offset);
         item->setData(Qt::ItemDataRole::UserRole + 1, entry.pos.line + 1);
         item->setData(Qt::ItemDataRole::UserRole + 2, entry.pos.col());
         // TODO: icon for non-fatal errors
         widget->addItem(item);
      }
      for (auto& entry : this->_lastFatals) {
         auto item = new QListWidgetItem;
         item->setText(entry.text);
         item->setData(Qt::ItemDataRole::UserRole + 0, entry.pos.offset);
         item->setData(Qt::ItemDataRole::UserRole + 1, entry.pos.line + 1);
         item->setData(Qt::ItemDataRole::UserRole + 2, entry.pos.col());
         // TODO: icon for fatal errors
         widget->addItem(item);
      }
   }
   if (!this->_lastErrors.size() && !this->_lastFatals.size()) {
      auto item = new QListWidgetItem;
      item->setText(tr("The script is valid and contains no errors!"));
      item->setData(Qt::ItemDataRole::UserRole + 0, -1);
      item->setData(Qt::ItemDataRole::UserRole + 1, -1);
      item->setData(Qt::ItemDataRole::UserRole + 2, -1);
      // TODO: icon for success
      widget->addItem(item);
   }
}