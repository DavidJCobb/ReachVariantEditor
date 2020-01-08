#include "localized_string_editor.h"
#include "../editor_state.h"

LocalizedStringEditorModal::LocalizedStringEditorModal(QWidget* parent) : QDialog(parent) {
   this->ui.setupUi(this);
   //
   this->languageFields.reserve(reach::language_count);
   this->languageFields.push_back(this->ui.fieldEnglish);
   this->languageFields.push_back(this->ui.fieldJapanese);
   this->languageFields.push_back(this->ui.fieldGerman);
   this->languageFields.push_back(this->ui.fieldFrench);
   this->languageFields.push_back(this->ui.fieldSpanish);
   this->languageFields.push_back(this->ui.fieldMexican);
   this->languageFields.push_back(this->ui.fieldItalian);
   this->languageFields.push_back(this->ui.fieldKorean);
   this->languageFields.push_back(this->ui.fieldChineseTrad);
   this->languageFields.push_back(this->ui.fieldChineseSimp);
   this->languageFields.push_back(this->ui.fieldPortugese);
   this->languageFields.push_back(this->ui.fieldPolish);
   this->ui.fieldEnglish->setProperty("ReachLanguage",  (int)reach::language::english);
   this->ui.fieldJapanese->setProperty("ReachLanguage", (int)reach::language::japanese);
   this->ui.fieldGerman->setProperty("ReachLanguage",   (int)reach::language::german);
   this->ui.fieldFrench->setProperty("ReachLanguage",   (int)reach::language::french);
   this->ui.fieldSpanish->setProperty("ReachLanguage",  (int)reach::language::spanish);
   this->ui.fieldMexican->setProperty("ReachLanguage",  (int)reach::language::mexican);
   this->ui.fieldItalian->setProperty("ReachLanguage",  (int)reach::language::italian);
   this->ui.fieldKorean->setProperty("ReachLanguage",   (int)reach::language::korean);
   this->ui.fieldChineseTrad->setProperty("ReachLanguage", (int)reach::language::chinese_traditional);
   this->ui.fieldChineseSimp->setProperty("ReachLanguage", (int)reach::language::chinese_simplified);
   this->ui.fieldPortugese->setProperty("ReachLanguage", (int)reach::language::portugese);
   this->ui.fieldPolish->setProperty("ReachLanguage",    (int)reach::language::polish);
   //
   QObject::connect(this->ui.buttonCancel, &QPushButton::clicked, [this]() {
      this->reject();
   });
   QObject::connect(this->ui.buttonSave, &QPushButton::clicked, [this]() {
      auto s = this->_target;
      for (auto& control : this->languageFields) {
         auto lang = control->property("ReachLanguage");
         if (!lang.isValid())
            continue;
         auto index = lang.toInt();
         if (index < 0 || index >= reach::language_count)
            continue;
         s->strings[index] = control->text().toUtf8();
      }
      ReachEditorState::get().stringModified(s->index());
      this->accept();
   });
   //
   // TODO: Save button
   // TODO: Save as New button
   //
}
/*static*/ void LocalizedStringEditorModal::startEditing(QWidget* parent, uint32_t flags, ReachString* target) {
   LocalizedStringEditorModal modal(parent);
   modal._target = target;
   modal._limitToSingleLanguageStrings = flags & Flags::SingleLanguageString;
   //
   // TODO: anything else?
   //
   modal.updateControls();
   modal.exec();
}
void LocalizedStringEditorModal::updateControls() {
   auto target = this->_target;
   //
   if (target) {
      this->ui.labelStringIndex->setText(this->ui.labelStringIndex->text().arg(target->index()));
      //
      for (auto& control : this->languageFields) {
         auto lang = control->property("ReachLanguage");
         if (!lang.isValid())
            continue;
         auto index = lang.toInt();
         if (index < 0 || index >= reach::language_count)
            continue;
         control->setText(QString::fromUtf8(target->strings[index].c_str()));
      }
   } else {
      this->ui.labelStringIndex->setText(tr("New string", "string editor"));
      this->ui.buttonSave->setDisabled(true);
   }
   if (this->_limitToSingleLanguageStrings) {
      for (auto& control : this->languageFields) {
         auto lang = control->property("ReachLanguage");
         if (!lang.isValid())
            continue;
         auto index = lang.toInt();
         if (index < 0 || index >= reach::language_count)
            continue;
         if (index == 0)
            continue;
         control->setDisabled(true);
      }
      QObject::connect(this->ui.fieldEnglish, &QLineEdit::textEdited, [this]() {
         auto text = this->ui.fieldEnglish->text();
         for (auto& control : this->languageFields) {
            if (control == this->ui.fieldEnglish)
               continue;
            control->setText(text);
         }
      });
   }
}

// TODO:
//
// Send signal ReachEditorState::stringModified with the index of the modified string when saving, but not when using 
// save as new.
//
// Send signal ReachEditorState::stringTableModified when using save as new.