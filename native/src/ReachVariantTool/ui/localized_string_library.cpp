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
      this->selectItem(this->_getListSelection(item));
   });
   QObject::connect(this->ui.search, &QLineEdit::textEdited, [this]() {
      this->updateEntryList();
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
      QString text;
      for (uint8_t i = 0; i < reach::language_count; ++i) {
         this->_makeString(text, (reach::language)i);
         this->target->strings[i] = text.toStdString();
      }
      this->accept();
   });
   QObject::connect(this->ui.buttonCancel, &QPushButton::clicked, [this]() {
      this->reject();
   });
   //
   QObject::connect(this->ui.stringOptionInteger_Value, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
      this->updatePreview();
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
   if (this->last_token_type != this->current->token) {
      const QSignalBlocker blocker0(this->ui.stringOptionInteger_Value);
      switch (this->last_token_type) {
         case token_t::integer:
            this->ui.stringOptionInteger_Value->setValue(0);
            break;
      }
      this->last_token_type = this->current->token;
   }
   this->updateControls();
}
void LocalizedStringLibraryDialog::updateControls() {
   this->updatePreview();
   if (!this->target || !this->current) {
      this->ui.description->setText(tr("N/A", "localized string library description"));
      this->ui.buttonLanguageLeft->setDisabled(true);
      this->ui.buttonLanguageRight->setDisabled(true);
      this->ui.stringOptionsPane->setCurrentWidget(this->ui.stringOptionsPageNone);
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
   switch (this->current->token) {
      case token_t::none:
         this->ui.stringOptionsPane->setCurrentWidget(this->ui.stringOptionsPageNone);
         break;
      case token_t::integer:
         this->ui.stringOptionsPane->setCurrentWidget(this->ui.stringOptionsPageInteger);
         break;
   }
}
void LocalizedStringLibraryDialog::updateEntryList() {
   auto  list = this->ui.list;
   auto& lib  = LocalizedStringLibrary::get();
   auto  size = lib.size();
   //
   auto filters = this->ui.search->text().split(' ');
   //
   const QSignalBlocker blocker(list);
   list->clear();
   for (size_t i = 0; i < size; ++i) {
      auto* entry = lib[i];
      if (filters.size() && entry != this->current) {
         bool matches = false;
         for (auto& filter : filters) {
            if (entry->matches(filter)) {
               matches = true;
               break;
            }
         }
         if (!matches)
            continue;
      }
      auto item = new QListWidgetItem;
      item->setText(entry->friendly_name);
      item->setData(Qt::UserRole, QVariant::fromValue((void*)entry));
      list->addItem(item);
   }
   if (this->current)
      _selectByPointerData(list, this->current);
}
void LocalizedStringLibraryDialog::updatePreview() {
   const QSignalBlocker blocker0(this->ui.preview);
   this->ui.previewLanguageName->setText(_languageCodeToName(this->previewLanguage));
   if (!this->current || !this->current->content) {
      this->ui.preview->setPlainText("");
      return;
   }
   //
   QString text;
   this->_makeString(text, this->previewLanguage);
   this->ui.preview->setPlainText(text);
}
void LocalizedStringLibraryDialog::onOpened() { // called by openForString when the modal is first opened
   this->updateEntryList();
   this->updateControls();
}
LocalizedStringLibraryDialog::entry_t* LocalizedStringLibraryDialog::_getListSelection(QListWidgetItem* item) {
   if (!item) {
      item = this->ui.list->currentItem();
      if (!item)
         return nullptr;
   }
   auto data = item->data(Qt::ItemDataRole::UserRole);
   if (!data.isValid())
      return nullptr;
   return (entry_t*)data.value<void*>();
}
void LocalizedStringLibraryDialog::_makeString(QString& out, reach::language lang) {
   assert(this->current);
   if (this->current->token == token_t::integer) {
      this->current->apply_permutation(lang, out, this->ui.stringOptionInteger_Value->value());
   } else {
      this->current->copy(lang, out);
   }
}