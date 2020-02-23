#include "page_script_code.h"

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
      auto& triggers = mp->scriptContent.triggers;
      for (auto& trigger : triggers) {
         if (trigger.entryType == Megalo::entry_type::subroutine)
            continue;
         trigger.decompile(decompiler);
         decompiler.write_line("");
      }
      const QSignalBlocker blocker(this->ui.output);
      this->ui.output->setPlainText(decompiler.current_content);
   });
}