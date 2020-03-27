#pragma once
#include "ui_page_req_obj_types.h"
#include "../../editor_state.h"

class ScriptEditorPageReqObjTypes : public QWidget {
   Q_OBJECT
   public:
      ScriptEditorPageReqObjTypes(QWidget* parent = nullptr);
      //
   protected:
      Ui::ScriptEditorPageRequiredObjectTypes ui;
      //
      void updateFromVariant(GameVariant* variant = nullptr);
};