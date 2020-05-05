#pragma once
#include "ui_localized_string_library.h"
#include "../services/localization_library.h"

class LocalizedStringLibraryDialog : public QDialog {
   Q_OBJECT
   public:
      using entry_t = LocalizedStringLibrary::Entry;
   public:
      LocalizedStringLibraryDialog(QWidget* parent = nullptr);
      //
      static bool openForString(QWidget* parent, ReachString* target = nullptr);
      //
   private:
      Ui::LocalizedStringLibraryDialog ui;
      //
      ReachString* target  = nullptr; // string to apply to
      entry_t*     current = nullptr;
      reach::language previewLanguage = reach::language::english;
      //
      void selectItem(entry_t*);
      void updateControls();
      void updateEntryList();
      void updatePreview();
      //
      void onOpened(); // called by openForString when the modal is first opened
};