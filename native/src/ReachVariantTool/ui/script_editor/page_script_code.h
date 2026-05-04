#pragma once
#include "ui_page_script_code.h"
#include "../../game_variants/components/megalo/compiler/compiler.h"
namespace ui::megalo_syntax_highlighting {
   class highlighter;
}

class ScriptEditorPageScriptCode : public QWidget {
   Q_OBJECT
   public:
      using Compiler = Megalo::Compiler;
      using log_t    = Compiler::log_t;
      using pos_t    = Compiler::pos;

      using highlighter_type = ui::megalo_syntax_highlighting::highlighter;
      
   public:
      ScriptEditorPageScriptCode(QWidget* parent = nullptr);
      
   protected:
      Ui::ScriptEditorPageScriptCode ui;
      highlighter_type* highlighter = nullptr;
      //
      log_t _lastNotices;
      log_t _lastWarnings;
      log_t _lastErrors;
      log_t _lastFatals;
      
      void updateLog(Compiler&);
      void redrawLog();

      void jumpToLogItem(QListWidgetItem&);

      void updateCodeEditorStyle();
      void updateSyntaxHighlightingEnabled();
};