#include "localized_string_library.h"

namespace {
   bool _selectByPointerData(QListWidget* widget, void* target) {
      auto count = widget->count();
      for (size_t row = 0; row < count; row++) {
         auto item = widget->item(row);
         if (!item)
            continue;
         auto data = item->data(Qt::ItemDataRole::UserRole);
         if (!data.isValid())
            continue;
         auto ptr = data.value<void*>();
         if (ptr == target) {
            widget->setCurrentItem(item);
            return true;
         }
      }
      return false; // not found
   }
   QString _languageCodeToName(reach::language lang) {
      switch (lang) {
         case reach::language::english: return QObject::tr("English");
         case reach::language::japanese: return QObject::tr("Japanese");
         case reach::language::german: return QObject::tr("German");
         case reach::language::french: return QObject::tr("French");
         case reach::language::spanish: return QObject::tr("Spanish");
         case reach::language::mexican: return QObject::tr("Mexican");
         case reach::language::italian: return QObject::tr("Italian");
         case reach::language::korean: return QObject::tr("Korean");
         case reach::language::chinese_traditional: return QObject::tr("Chinese (Traditional)");
         case reach::language::chinese_simplified: return QObject::tr("Chinese (Simplified)");
         case reach::language::portugese: return QObject::tr("Portugese");
         case reach::language::polish: return QObject::tr("Polish");
      }
      return QObject::tr("???", "language");
   }
}
LocalizedStringLibraryDialog::LocalizedStringLibraryDialog(QWidget* parent) : QDialog(parent) {
   ui.setupUi(this);
   //
   QObject::connect(this->ui.list, &QListWidget::currentItemChanged, [this](QListWidgetItem* item, QListWidgetItem* previous) {
      if (!item)
         return;
      auto data = item->data(Qt::ItemDataRole::UserRole);
      if (!data.isValid())
         return;
      auto ptr = (entry_t*)data.value<void*>();
      this->selectItem(ptr);
   });
   QObject::connect(this->ui.search, &QLineEdit::textChanged, [this]() {
      //
      // TODO: ability to filter the list of strings
      //
   });
   //
   QObject::connect(this->ui.buttonLanguageLeft, &QPushButton::clicked, [this]() {
      auto lang = (int)this->previewLanguage - 1;
      if (lang < 0)
         lang = reach::language_count - 1;
      this->previewLanguage = (reach::language)lang;
      this->updatePreview();
   });
   QObject::connect(this->ui.buttonLanguageRight, &QPushButton::clicked, [this]() {
      auto lang = (int)this->previewLanguage + 1;
      if (lang >= reach::language_count)
         lang = 0;
      this->previewLanguage = (reach::language)lang;
      this->updatePreview();
   });
   QObject::connect(this->ui.buttonApply, &QPushButton::clicked, [this]() {
      //
      // TODO: modify (this->target)
      //
      this->accept();
   });
   QObject::connect(this->ui.buttonCancel, &QPushButton::clicked, [this]() {
      this->reject();
   });
}

/*static*/ bool LocalizedStringLibraryDialog::openForString(QWidget* parent, ReachString* target) {
   LocalizedStringLibraryDialog modal(parent);
   modal.target = target;
   modal.onOpened();
   return modal.exec() == QDialog::Accepted;
}

void LocalizedStringLibraryDialog::selectItem(entry_t* entry) {
   this->current = entry;
   this->updateControls();
}
void LocalizedStringLibraryDialog::updateControls() {
   this->updatePreview();
   if (!this->target || !this->current) {
      this->ui.description->setText(tr("N/A", "localized string library description"));
      this->ui.buttonLanguageLeft->setDisabled(true);
      this->ui.buttonLanguageRight->setDisabled(true);
      //
      // TODO: Delete all children of (this->ui.stringOptionsPane), and add a label whose text is 
      // tr("No additional options.").
      //
      return;
   }
   {
      auto& desc  = this->current->description;
      auto  label = this->ui.description;
      if (desc.isEmpty())
         label->setText(tr("No description available for this string.", "localized string library description"));
      else
         label->setText(desc);
   }
   this->ui.buttonLanguageLeft->setDisabled(false);
   this->ui.buttonLanguageRight->setDisabled(false);
   //
   // TODO: Delete all children of (this->ui.stringOptionsPane).
   //
   if (this->current->has_token()) {
      //
      // TODO: Generate form controls based on the token type. Currently the only defined token 
      // type is a single integer. The integer that the user selects will determine the string 
      // permutation used (we'll need to live-update the preview accordingly).
      //
   } else {
      //
      // TODO: Add a label whose text is tr("No additional options.").
      //
   }
}
void LocalizedStringLibraryDialog::updateEntryList() {
   auto  list = this->ui.list;
   auto& lib  = LocalizedStringLibrary::get();
   auto  size = lib.size();
   const QSignalBlocker blocker(list);
   for (size_t i = 0; i < size; ++i) {
      //
      // TODO: FILTER LIST ITEMS BASED ON (this->ui.search). SEARCHING SHOULD INCLUDE ITEMS 
      // BASED ON THEIR NAME, TAGS, AND TEXT IN ANY LANGUAGE, BUT NOT BASED ON ANY OTHER DATA.
      //
      auto* entry = lib[i];
      auto  item  = new QListWidgetItem;
      item->setText(entry->friendly_name);
      item->setData(Qt::UserRole, QVariant::fromValue((void*)entry));
      list->addItem(item);
   }
   //
   // TODO: SORT THE LIST ALPHABETICALLY
   //
}
void LocalizedStringLibraryDialog::updatePreview() {
   const QSignalBlocker blocker0(this->ui.preview);
   this->ui.previewLanguageName->setText(_languageCodeToName(this->previewLanguage));
   if (!this->current || !this->current->content) {
      this->ui.preview->setPlainText("");
      return;
   }
   auto& lang = this->current->content->language(this->previewLanguage);
   this->ui.preview->setPlainText(QString::fromStdString(lang));
   //
   // TODO: If the current entry has a token, we need to apply permutations to the preview 
   // text as appropriate.
   //
}
void LocalizedStringLibraryDialog::onOpened() { // called by openForString when the modal is first opened
   this->updateEntryList();
   this->updateControls();
}