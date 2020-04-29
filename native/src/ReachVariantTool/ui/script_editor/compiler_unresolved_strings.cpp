#include "compiler_unresolved_strings.h"
#include "../../editor_state.h"

CompilerUnresolvedStringsDialog::CompilerUnresolvedStringsDialog(QWidget* parent) : QDialog(parent) {
   this->ui.setupUi(this);
   //
   QObject::connect(this->ui.selStrIndex, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
      this->selectReference(value, 0);
   });
   QObject::connect(this->ui.selRefIndex, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
      this->selectReference(this->_currentStr, value);
   });
   //
   this->ui.refOptExistingWhich->setTarget(this->_dummyStringRef);
   QObject::connect(this->ui.refOptExistingWhich, &ReachStringPicker::selectedStringChanged, [this]() {
      auto item = this->getUnresolvedStr();
      if (!item)
         return;
      if (this->_dummyStringRef) {
         auto index = this->_dummyStringRef->index;
         if (index < 0)
            return;
         item->pending.index = index;
      }
   });
   QObject::connect(this->ui.refOptCreate, &QRadioButton::toggled, [this](bool checked) {
      if (!checked)
         return;
      auto item = this->getUnresolvedStr();
      if (!item)
         return;
      item->pending.action = Megalo::Compiler::unresolved_string_pending_action::create;
      this->updateStatus();
   });
   QObject::connect(this->ui.refOptExisting, &QRadioButton::toggled, [this](bool checked) {
      if (!checked)
         return;
      auto item = this->getUnresolvedStr();
      if (!item)
         return;
      item->pending.action = Megalo::Compiler::unresolved_string_pending_action::use_existing;
      this->updateStatus();
   });
   //
   QObject::connect(this->ui.buttonCancel, &QPushButton::clicked, [this]() {
      this->reject();
   });
   QObject::connect(this->ui.buttonCommit, &QPushButton::clicked, [this]() {
      auto success = this->_compiler->handle_unresolved_string_references();
      ReachEditorState::get().stringTableModified();
      if (success)
         this->accept();
      else
         this->reject();
   });
}

/*static*/ bool CompilerUnresolvedStringsDialog::handleCompiler(QWidget* parent, Megalo::Compiler& compiler) {
   CompilerUnresolvedStringsDialog modal(parent);
   modal.takeCompiler(compiler);
   //
   return modal.exec() == QDialog::Accepted;
}

void CompilerUnresolvedStringsDialog::takeCompiler(Megalo::Compiler& compiler) {
   this->_compiler = &compiler;
   //
   // Update widgets:
   //
   auto& list = this->_compiler->get_unresolved_string_references();
   this->_keys = list.uniqueKeys();
   this->ui.selStrIndex->setMaximum(this->_keys.size() - 1);
   //
   this->updateStatus();
   this->selectReference(0, 0);
}
void CompilerUnresolvedStringsDialog::selectReference(int32_t str, int32_t ref) {
   const QSignalBlocker blocker0(this->ui.selStrIndex);
   const QSignalBlocker blocker1(this->ui.selRefIndex);
   //
   this->_currentStr = str;
   this->_currentRef = ref;
   if (str < 0 || ref < 0) {
      this->_currentStr = -1;
      this->_currentRef = -1;
      this->ui.stringPreview->setPlainText("");
      return;
   }
   auto& list = this->_compiler->get_unresolved_string_references();
   auto& keys = this->_keys;
   if (str >= keys.size()) {
      this->_currentStr = -1;
      this->_currentRef = -1;
      this->ui.stringPreview->setPlainText("");
      return;
   }
   auto  key  = keys[str];
   auto  refs = this->getRefsByKey(key);
   if (ref >= refs.size()) {
      this->_currentRef = -1;
      this->ui.stringPreview->setPlainText("");
      return;
   }
   //
   this->ui.selStrIndex->setValue(str);
   this->ui.selRefIndex->setValue(ref);
   this->ui.selRefIndex->setMaximum(refs.size() - 1);
   this->ui.stringPreview->setPlainText(key);
   //
   const QSignalBlocker blocker2(this->ui.refOptCreate);
   const QSignalBlocker blocker3(this->ui.refOptExisting);
   const QSignalBlocker blocker4(this->ui.refOptExistingWhich);
   //
   auto& item = *refs[ref];
   if (item.pending.action == Megalo::Compiler::unresolved_string_pending_action::create) {
      this->ui.refOptCreate->setChecked(true);
   } else {
      this->ui.refOptExisting->setChecked(true);
   }
   {
      //
      // The following only applies to strings that have been set to use an existing string, but we're going 
      // to actually go and update the widget for all of them.
      //
      auto& mp    = this->_compiler->get_variant();
      auto& table = mp.scriptData.strings;
      auto  index = item.pending.index;
      auto  str   = table.get_entry(index);
      //
      this->_dummyStringRef = str;
      this->ui.refOptExistingWhich->refreshSelection();
   }
}
void CompilerUnresolvedStringsDialog::updateStatus() {
   //
   // Update the "your settings will create %1 strings; there is room for %2 strings" message; also updates 
   // whether the Commit button is greyed out.
   //
   auto  label = this->ui.status;
   auto& mp    = this->_compiler->get_variant();
   auto& table = mp.scriptData.strings;
   //
   size_t remaining = table.max_count - table.size();
   //
   auto&  list      = this->_compiler->get_unresolved_string_references();
   auto&  keys      = this->_keys;
   size_t to_create = 0;
   for (auto& key : keys) {
      auto refs = this->getRefsByKey(key);
      for (auto* ref : refs) {
         if (ref->pending.action == Megalo::Compiler::unresolved_string_pending_action::create) {
            ++to_create;
            break;
         }
      }
   }
   label->setText(tr("The options you have selected in this dialog dictate that <b>%1</b> new strings will be created. The string table has room for <b>%2</b> new strings.").arg(to_create).arg(remaining));
   //
   this->ui.buttonCommit->setDisabled(to_create > remaining);
}
QList<CompilerUnresolvedStringsDialog::unresolved_str*> CompilerUnresolvedStringsDialog::getRefsByKey(const QString& key) {
   QList<CompilerUnresolvedStringsDialog::unresolved_str*> out;
   //
   auto& list = this->_compiler->get_unresolved_string_references();
   auto it = list.find(key);
   for (; it != list.end(); ++it) {
      if (it.key() != key)
         continue;
      out.push_back(&it.value());
   }
   return out;
}
Megalo::Compiler::unresolved_str* CompilerUnresolvedStringsDialog::getUnresolvedStr() {
   if (!this->_compiler)
      return nullptr;
   if (this->_currentRef < 0 || this->_currentStr < 0)
      return nullptr;
   auto& list = this->_compiler->get_unresolved_string_references();
   auto& keys = this->_keys;
   if (this->_currentStr >= keys.size())
      return nullptr;
   auto key    = keys[this->_currentStr];
   auto values = this->getRefsByKey(key);
   if (this->_currentRef >= values.size())
      return nullptr;
   return values[this->_currentRef];
}