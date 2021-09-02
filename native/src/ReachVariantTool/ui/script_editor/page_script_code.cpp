#include "page_script_code.h"
#include "compiler_unresolved_strings.h"
#include <QMessageBox>
#include <QListWidget>
#include <QTextBlock>
#include "../generic/MegaloSyntaxHighlighter.h"

namespace {
   static const QString ce_icon_success = ":/ScriptEditor/compiler_log/success.png";
   static const QString ce_icon_warning = ":/ScriptEditor/compiler_log/warning.png";
   static const QString ce_icon_error   = ":/ScriptEditor/compiler_log/error.png";
   static const QString ce_icon_fatal   = ":/ScriptEditor/compiler_log/fatal.png";
   //
   enum class _icon_type {
      unknown = 0,
      success,
      warning,
      error,
      fatal,
   };
   constexpr int role_offset = Qt::ItemDataRole::UserRole + 0;
   constexpr int role_line   = Qt::ItemDataRole::UserRole + 1;
   constexpr int role_col    = Qt::ItemDataRole::UserRole + 2;
   constexpr int role_icon   = Qt::ItemDataRole::UserRole + 3;
}

ScriptEditorPageScriptCode::ScriptEditorPageScriptCode(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   {
      new MegaloSyntaxHighlighter(this->ui.textEditor->document());
   }
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
      switch ((_icon_type)item->data(role_icon).toInt()) {
         case _icon_type::success:
            out.prepend("[GOOD!] ");
            break;
         case _icon_type::warning:
            out.prepend("[WARN!] ");
            break;
         case _icon_type::error:
            out.prepend("[ERROR] ");
            break;
         case _icon_type::fatal:
            out.prepend("[FATAL] ");
            break;
      }
      //
      int line = item->data(role_line).toInt();
      int col  = item->data(role_col).toInt();
      if (line < 1 || col < 0)
         return;
      out.prepend(QString("Line %1 col %2: ").arg(line).arg(col));
   });
   QObject::connect(this->ui.compileLog, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* item) {
      int line = item->data(role_line).toInt();
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
   //
   auto ico_success = QIcon(ce_icon_success);
   auto ico_warning = QIcon(ce_icon_warning);
   auto ico_error   = QIcon(ce_icon_error);
   auto ico_fatal   = QIcon(ce_icon_fatal);
   //
   if (this->ui.compileLogFilterWarn->isChecked()) {
      for (auto& entry : this->_lastWarnings) {
         auto item = new QListWidgetItem;
         item->setText(entry.text);
         item->setData(role_offset, entry.pos.offset);
         item->setData(role_line,   entry.pos.line + 1);
         item->setData(role_col,    entry.pos.col());
         item->setData(role_icon,   (int)_icon_type::warning);
         item->setIcon(ico_warning);
         widget->addItem(item);
      }
   }
   if (this->ui.compileLogFilterError->isChecked()) {
      for (auto& entry : this->_lastErrors) {
         auto item = new QListWidgetItem;
         item->setText(entry.text);
         item->setData(role_offset, entry.pos.offset);
         item->setData(role_line,   entry.pos.line + 1);
         item->setData(role_col,    entry.pos.col());
         item->setData(role_icon,   (int)_icon_type::error);
         item->setIcon(ico_error);
         widget->addItem(item);
      }
      for (auto& entry : this->_lastFatals) {
         auto item = new QListWidgetItem;
         item->setText(entry.text);
         item->setData(role_offset, entry.pos.offset);
         item->setData(role_line,   entry.pos.line + 1);
         item->setData(role_col,    entry.pos.col());
         item->setData(role_icon,   (int)_icon_type::fatal);
         item->setIcon(ico_fatal);
         widget->addItem(item);
      }
   }
   if (!this->_lastErrors.size() && !this->_lastFatals.size()) {
      auto item = new QListWidgetItem;
      item->setText(tr("The script is valid and contains no errors!"));
      item->setData(role_offset, -1);
      item->setData(role_line,   -1);
      item->setData(role_col,    -1);
      item->setData(role_icon,   (int)_icon_type::success);
      item->setIcon(ico_success);
      widget->addItem(item);
   }
}