#include "page_script_code.h"
#include "../../game_variants/components/megalo/compiler/compiler.h"

ScriptEditorPageScriptCode::ScriptEditorPageScriptCode(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   //
   QObject::connect(this->ui.testButton, &QPushButton::clicked, [this]() {
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
      const QSignalBlocker blocker(this->ui.output);
      this->ui.output->setPlainText(decompiler.current_content);
   });
   QObject::connect(this->ui.testCompile, &QPushButton::clicked, [this]() {
      auto& editor  = ReachEditorState::get();
      auto  variant = editor.variant();
      if (!variant)
         return;
      auto mp = variant->get_multiplayer_data();
      if (!mp)
         return;
      Megalo::Compiler compiler(*mp);
      compiler.parse(this->ui.output->toPlainText());
      //
      const QSignalBlocker blocker(this->ui.compileLog);
      if (compiler.has_errors()) {
         auto    widget = this->ui.compileLog;
         QString text;
         //
         for (auto& entry : compiler.get_fatal_errors()) {
            text += QString("[FATAL] Line %1 col %2: %3\n").arg(entry.pos.line + 1).arg(entry.pos.col()).arg(entry.text);
         }
         for (auto& entry : compiler.get_non_fatal_errors()) {
            text += QString("[ERROR] Line %1 col %2: %3\n").arg(entry.pos.line + 1).arg(entry.pos.col()).arg(entry.text);
         }
         for (auto& entry : compiler.get_warnings()) {
            text += QString("[WARN] Line %1 col %2: %3\n").arg(entry.pos.line + 1).arg(entry.pos.col()).arg(entry.text);
         }
         this->ui.compileLog->setPlainText(text);
      } else {
         this->ui.compileLog->setPlainText("No errors!");
      }
   });
}