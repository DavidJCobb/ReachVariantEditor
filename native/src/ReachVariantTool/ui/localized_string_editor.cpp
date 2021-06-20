#include "localized_string_editor.h"
#include "localized_string_library.h"
#include "../editor_state.h"
#include "../game_variants/types/multiplayer.h"
#include <QAbstractTextDocumentLayout>

namespace {
   ReachStringTable _dummy_string_owner(1, 99999);
}
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
      if (!this->_target) { // should never happen
         this->reject();
         return;
      }
      auto s = this->_target;
      for (auto& control : this->languageFields) {
         auto lang = control->property("ReachLanguage");
         if (!lang.isValid())
            continue;
         auto index = lang.toInt();
         if (index < 0 || index >= reach::language_count)
            continue;
         s->set_content((reach::language)index, control->toPlainText().toUtf8());
      }
      if (!this->_isNotInStandardStringTable)
         ReachEditorState::get().stringModified(s->index);
      this->accept();
   });
   QObject::connect(this->ui.buttonSaveAsNew, &QPushButton::clicked, [this]() {
      if (this->_disallowSaveAsNew || this->_isNotInStandardStringTable)
         return;
      auto& editor = ReachEditorState::get();
      auto  mp     = editor.multiplayerData();
      if (!mp) { // should never happen
         this->reject();
         return;
      }
      auto s = mp->scriptData.strings.add_new();
      if (!s)
         return;
      auto r = this->_targetRef;
      for (auto& control : this->languageFields) {
         auto lang = control->property("ReachLanguage");
         if (!lang.isValid())
            continue;
         auto index = lang.toInt();
         if (index < 0 || index >= reach::language_count)
            continue;
         s->set_content((reach::language)index, control->toPlainText().toUtf8());
      }
      if (this->_targetRef) {
         *this->_targetRef = s;
      }
      ReachEditorState::get().stringTableModified();
      ReachEditorState::get().stringModified(mp->scriptData.strings.size() - 1);
      this->accept();
   });
   QObject::connect(this->ui.buttonOpenLibrary, &QPushButton::clicked, [this]() {
      ReachString str;
      if (this->_target)
         str = *this->_target;
      else if (this->_targetRef) {
         ReachString* p = this->_targetRef->pointer_cast<ReachString>();
         if (p)
            str = *p;
      }
      if (LocalizedStringLibraryDialog::openForString(this, &str)) {
         this->updateTextboxes(&str);
      }
   });
   //
   // QPlainTextEdit doesn't have a built-in max-length. If you search online for workarounds to that, 
   // you'll find a bunch of ideas, but they all suck:
   // 
   //  - Set an eventFilter, and block input events that would generate input if the textbox is full. 
   //    Problem with this is, if the textbox is nearly full and someone pastes a long string in, you 
   //    won't stop that, so now you've gone out of bounds.
   // 
   //  - Truncate the textbox contents whenever it exceeds the max length, to give the illusion that 
   //    you're preventing the user from entering text. This breaks if the user types into the middle 
   //    of the existing value, as you end up deleting from the end. Pasting breaks it as well; you 
   //    don't know what they pasted, so you can't truly undo their input even if they're pasting at 
   //    the end.
   // 
   //  - Use QPlainTextEdit::undo. This almost works, except that if you hold down a key (say, "A") 
   //    and have it repeat, such that the textbox fills with As, all of those As are one shared edit 
   //    operation, and calling "undo" wipes all of them, not just the most recent one.
   // 
   // The approach I settled on was to hook QTextDocument::contentsChange.
   //
   for (auto* widget : this->languageFields) {
      auto* doc = widget->document();
      assert(doc);
      QObject::connect(doc, &QTextDocument::contentsChange, this, [this, widget](int pos, int removed, int added) {
         if (added - removed <= 0)
            return;
         auto maximum = this->_maxLength;
         if (maximum < 0)
            return;
         auto text = widget->toPlainText();
         if (text.size() <= maximum)
            return;
         //
         auto* doc = widget->document();
         auto  cur = QTextCursor(doc);
         const auto blocker = QSignalBlocker(doc);
         cur.setPosition(pos);
         cur.setPosition(pos + added, QTextCursor::MoveMode::KeepAnchor);
         if (cur.hasSelection()) {
            cur.deleteChar();
            text = widget->toPlainText();
         }
         if (text.size() > maximum) { // failsafe
            text = text.left(maximum);
            widget->setPlainText(text);
         }
         QApplication::beep();
      });
   }
}
/*static*/ bool LocalizedStringEditorModal::editString(QWidget* parent, uint32_t flags, ReachString* target) {
   LocalizedStringEditorModal modal(parent);
   modal._targetRef = nullptr;
   modal._target    = target;
   modal._disallowSaveAsNew            = flags & Flags::DisallowSaveAsNew;
   modal._isNotInStandardStringTable   = flags & Flags::IsNotInStandardTable;
   modal._limitToSingleLanguageStrings = flags & Flags::SingleLanguageString;
   //
   if (modal._isNotInStandardStringTable) {
      if (modal._limitToSingleLanguageStrings) {
         //
         // Special-case behavior for team names:
         //
         auto* table = target->get_owner();
         if (table && table->max_count == 1) {
            modal._maxLength = table->max_buffer_size;
         }
      }
   } else if (!modal._limitToSingleLanguageStrings) {
      //
      // If this string is currently in use as a Forge label, force single-language content.
      //
      auto mp = ReachEditorState::get().multiplayerData();
      if (mp && mp->get_forge_label_using_string(modal._target)) {
         modal._limitToSingleLanguageStrings = true;
      }
   }
   //
   modal.updateControls();
   return modal.exec() == QDialog::Accepted;
}
/*static*/ bool LocalizedStringEditorModal::editString(QWidget* parent, uint32_t flags, MegaloStringRef& targetRef) {
   LocalizedStringEditorModal modal(parent);
   modal._targetRef = &targetRef;
   modal._target    = targetRef;
   modal._disallowSaveAsNew            = flags & Flags::DisallowSaveAsNew;
   modal._isNotInStandardStringTable   = flags & Flags::IsNotInStandardTable;
   modal._limitToSingleLanguageStrings = flags & Flags::SingleLanguageString;
   //
   if (modal._isNotInStandardStringTable) {
      if (modal._limitToSingleLanguageStrings) {
         //
         // Special-case behavior for team names:
         //
         auto* table = targetRef->get_owner();
         if (table && table->max_count == 1) {
            modal._maxLength = table->max_buffer_size;
         }
      }
   } else if (!modal._limitToSingleLanguageStrings) {
      //
      // If this string is currently in use as a Forge label, force single-language content.
      //
      auto mp = ReachEditorState::get().multiplayerData();
      if (mp && mp->get_forge_label_using_string(modal._target)) {
         modal._limitToSingleLanguageStrings = true;
      }
   }
   //
   modal.updateControls();
   return modal.exec() == QDialog::Accepted;
}
void LocalizedStringEditorModal::updateControls() {
   auto  target = this->_target;
   auto& editor = ReachEditorState::get();
   auto  mp     = editor.multiplayerData();
   //
   if (target) {
      this->ui.labelStringIndex->setText(this->ui.labelStringIndex->text().arg(target->index));
      this->updateTextboxes();
   } else {
      this->ui.labelStringIndex->setText(tr("New string", "string editor"));
      this->ui.buttonSave->setDisabled(true);
   }
   if (this->_limitToSingleLanguageStrings) {
      this->ui.buttonOpenLibrary->setDisabled(true);
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
      QObject::connect(this->ui.fieldEnglish, &QPlainTextEdit::textChanged, [this]() {
         auto text = this->ui.fieldEnglish->toPlainText();
         for (auto& control : this->languageFields) {
            if (control == this->ui.fieldEnglish)
               continue;
            control->setPlainText(text); // let this fire a signal so the other strings are actually changed
         }
      });
   }
   if (this->_disallowSaveAsNew || this->_isNotInStandardStringTable || (mp && mp->scriptData.strings.is_at_count_limit())) {
      this->ui.buttonSaveAsNew->setDisabled(true);
   }
}
void LocalizedStringEditorModal::updateTextboxes(ReachString* use) {
   auto target = use ? use : this->_target;
   if (!target)
      return;
   for (auto& control : this->languageFields) {
      auto lang = control->property("ReachLanguage");
      if (!lang.isValid())
         continue;
      auto index = lang.toInt();
      if (index < 0 || index >= reach::language_count)
         continue;
      const QSignalBlocker blocker(control);
      control->setPlainText(QString::fromUtf8(target->get_content((reach::language)index).c_str()));
   }
}