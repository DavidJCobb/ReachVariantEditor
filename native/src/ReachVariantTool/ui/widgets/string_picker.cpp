#include "string_picker.h"
#include <QBoxLayout>
#include "../../editor_state.h"

ReachStringPicker::ReachStringPicker(QWidget* parent, uint32_t flags) : QWidget(parent) {
   this->_limitToSingleLanguageStrings = flags & Flags::SingleLanguageStrings;
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
   QObject::connect(this->_combobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int index) {
   });
   QObject::connect(this->_button, &QPushButton::clicked, [this]() {
   });
   //
   this->refreshList();
}
void ReachStringPicker::setLimitedToSingleLanguageStrings(bool is) noexcept {
   this->_limitToSingleLanguageStrings = is;
   this->refreshList();
}
void ReachStringPicker::refreshList() {
   this->_combobox->clear();
   auto mp = ReachEditorState::get().multiplayerData();
   if (!mp)
      return;
   auto& table = mp->scriptData.strings;
   for (size_t i = 0; i < table.strings.size(); i++) {
      auto& string = *table.strings[i];
      //
      if (this->_limitToSingleLanguageStrings)
         if (!string.can_be_forge_label())
            continue;
      //
      this->_combobox->addItem(QString::fromUtf8(string.english().c_str()), QVariant(i));
   }
   this->refreshSelection();
}
void ReachStringPicker::refreshSelection() {
   if (!this->_targetRef)
      return;
   auto& ptr = *this->_targetRef;
   if (!ptr)
      return;
   int32_t i = ptr->index();
   if (i < 0)
      return;
   const QSignalBlocker blocker(this->_combobox);
   this->_combobox->setCurrentIndex(i);
}