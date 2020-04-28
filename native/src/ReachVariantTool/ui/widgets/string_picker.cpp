#include "string_picker.h"
#include <QBoxLayout>
#include <QFont>
#include "../../editor_state.h"
#include "../localized_string_editor.h"

ReachStringPicker::ReachStringPicker(QWidget* parent, uint32_t flags) : QWidget(parent) {
   this->_limitToSingleLanguageStrings = flags & Flags::SingleLanguageString;
   //
   auto layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
   this->setLayout(layout);
   this->_combobox = new QComboBox(this);
   this->_button = new QPushButton(this);
   //
   this->_button->setText(tr("...", "string picker button"));
   //
   layout->setContentsMargins(0, 0, 0, 0);
   layout->addWidget(this->_combobox, 1);
   layout->addWidget(this->_button,   0);
   //
   QObject::connect(this->_combobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int sel) {
      if (!this->_targetRef)
         return;
      auto& ref  = *this->_targetRef;
      auto  data = this->_combobox->currentData(Qt::ItemDataRole::UserRole);
      if (!data.isValid())
         return;
      auto index = data.toInt();
      if (index < 0) {
         ref = nullptr;
         return;
      }
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& table = mp->scriptData.strings;
      if (index >= table.strings.size())
         return;
      ref = &table.strings[index];
      //
      emit selectedStringChanged();
   });
   QObject::connect(this->_button, &QPushButton::clicked, [this]() {
      if (!this->_targetRef) // should never occur
         return;
      uint32_t flags = 0;
      if (this->_limitToSingleLanguageStrings)
         flags |= Flags::SingleLanguageString;
      if (LocalizedStringEditorModal::editString(this, flags, *this->_targetRef))
         emit selectedStringChanged();
   });
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::stringModified, this, &ReachStringPicker::refreshListItem);
   QObject::connect(&editor, &ReachEditorState::stringTableModified, this, &ReachStringPicker::refreshList);
   QObject::connect(&editor, &ReachEditorState::variantAbandoned, [this]() {
      this->_targetRef = nullptr;
   });
   //
   this->refreshList();
}

void ReachStringPicker::addBlacklistedString(const QString& s) noexcept {
   this->_blacklist.push_back(s);
   this->refreshList();
}
void ReachStringPicker::clearBlacklistedStrings() noexcept {
   this->_blacklist.clear();
   this->refreshList();
}
void ReachStringPicker::setAllowNoString(bool is) noexcept {
   this->_allowNoString = is;
   this->refreshList();
}
void ReachStringPicker::setBlacklistedStrings(const QList<QString>& list) noexcept {
   this->_blacklist.clear();
   this->_blacklist = list;
   this->refreshList();
}
void ReachStringPicker::setBlacklistedStrings(QList<QString>&& list) noexcept {
   this->_blacklist.clear();
   this->_blacklist.swap(list);
   this->refreshList();
}
void ReachStringPicker::setLimitedToSingleLanguageStrings(bool is) noexcept {
   this->_limitToSingleLanguageStrings = is;
   this->refreshList();
}

void ReachStringPicker::refreshList() {
   const QSignalBlocker blocker(this->_combobox);
   //
   this->_combobox->clear();
   auto mp = ReachEditorState::get().multiplayerData();
   if (!mp)
      return;
   if (this->_allowNoString) {
      auto italics = QFont("FontFamily", -1, -1, true);
      this->_combobox->addItem(tr("<no name>", "string picker"), QVariant(-1));
      this->_combobox->setItemData(0, italics, Qt::ItemDataRole::FontRole);
   }
   auto& table = mp->scriptData.strings;
   for (size_t i = 0; i < table.strings.size(); i++) {
      auto& string  = table.strings[i];
      bool  allSame = string.can_be_forge_label();
      //
      if (this->_limitToSingleLanguageStrings)
         if (!allSame)
            continue;
      //
      QString text = QString::fromUtf8(string.english().c_str());
      if (this->_blacklist.size()) {
         bool skip = false;
         if (allSame) {
            for (auto& bl : this->_blacklist) {
               if (bl == text) {
                  skip = true;
                  break;
               }
            }
         } else {
            for (auto& bl : this->_blacklist) {
               for (auto& tl : string.strings) {
                  if (QString::fromUtf8(tl.c_str()) == text) {
                     skip = true;
                     break;
                  }
               }
               if (skip)
                  break;
            }
         }
         if (skip)
            continue;
      }
      //
      this->_combobox->addItem(text, QVariant(i));
   }
   this->refreshSelection();
}
void ReachStringPicker::refreshSelection() {
   if (!this->_targetRef)
      return;
   auto& ptr = *this->_targetRef;
   int32_t i = -1;
   if (ptr) {
      i = ptr->index;
      if (i < 0) {
         if (!this->_allowNoString)
            return;
         i = -1;
      }
   }
   const QSignalBlocker blocker(this->_combobox);
   auto itemIndex = this->_combobox->findData(i);
   if (itemIndex >= 0)
      this->_combobox->setCurrentIndex(itemIndex);
}
void ReachStringPicker::refreshListItem(uint32_t index) {
   if (index > this->_combobox->count()) {
      this->refreshList();
      return;
   }
   auto mp = ReachEditorState::get().multiplayerData();
   if (!mp)
      return;
   auto& table = mp->scriptData.strings;
   if (index >= table.strings.size())
      return;
   auto text      = QString::fromUtf8(table.strings[index].english().c_str());
   auto itemIndex = this->_combobox->findData(index);
   if (itemIndex >= 0)
      this->_combobox->setItemText(itemIndex, text);
}