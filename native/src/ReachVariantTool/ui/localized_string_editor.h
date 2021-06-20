#pragma once
#include "ui_localized_string_editor.h"
#include <vector>
#include "../formats/localized_string_table.h"

class LocalizedStringEditorModal : public QDialog {
   Q_OBJECT
   public:
      using Flags = ReachStringFlags::type;
   public:
      LocalizedStringEditorModal(QWidget* parent = nullptr);
      //
      static bool editString(QWidget* parent, uint32_t flags = 0, ReachString* target = nullptr);
      static bool editString(QWidget* parent, uint32_t flags, MegaloStringRef& targetRef);
      //
   private:
      Ui::LocalizedStringEditorModal ui;
      //
      std::vector<QPlainTextEdit*> languageFields;
      //
      ReachString*     _target    = nullptr;
      MegaloStringRef* _targetRef = nullptr;
      bool _disallowSaveAsNew            = false;
      bool _isNotInStandardStringTable   = false; // for metadata, team names, etc.
      bool _limitToSingleLanguageStrings = false;
      int  _maxLength = -1; // special-case for team names
      //
      void updateControls();
      void updateTextboxes(ReachString* use = nullptr);
};