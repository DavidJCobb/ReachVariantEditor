#pragma once
#include "ui_page_string_table.h"
#include "../../editor_state.h"
#include <QAction>

class ScriptEditorPageStringTable : public QWidget {
   Q_OBJECT
   public:
      ScriptEditorPageStringTable(QWidget* parent = nullptr);
      //
   protected:
      Ui::ScriptEditorPageStringTable ui;
      //
      ReachString* getSelected();
      void updateFromVariant(GameVariant* variant = nullptr);
      //
      ReachString* _selected = nullptr; // This is ONLY used for maintaining the selection after redrawing the list; it should be considered opaque.
};