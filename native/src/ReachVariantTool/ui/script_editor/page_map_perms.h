#pragma once
#include "ui_page_map_perms.h"
#include "../../editor_state.h"
#include "../../game_variants/components/map_permissions.h"

class ScriptEditorPageMapPerms : public QWidget {
   Q_OBJECT
   public:
      ScriptEditorPageMapPerms(QWidget* parent = nullptr);
      //
   protected:
      Ui::ScriptEditorPageMapPerms ui;
      //
      void updateFromVariant(GameVariant* variant = nullptr);
};