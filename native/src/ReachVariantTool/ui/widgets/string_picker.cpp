#include "string_picker.h"
#include <QBoxLayout>
#include <QFont>
#include "../../editor_state/ReachStringCentralModel.h"
#include "../../editor_state.h"
#include "../localized_string_editor.h"
#include "../../helpers/qt/widget.h"
#include "../../game_variants/types/multiplayer.h"

ReachStringPicker::ReachStringPicker(QWidget* parent, uint32_t flags) : QWidget(parent) {
   #if _DEBUG
      //
      // ReachStringPicker can crash if you forget to call (clearTarget) on it before 
      // deleting some object that contains a MegaloStringRef that the string picker 
      // was working with. These crashes are really, really nasty to debug if you're 
      // dynamically linking Qt -- you can't easily check the widget's data, such as 
      // its parent widgets, to see what's going on.
      //
      this->_debug_hierarchy = cobb::qt::dump_ancestors(this);
   #endif
   
   this->_limitToSingleLanguageStrings = flags & Flags::SingleLanguageString;

   auto& editor = ReachEditorState::get();
   auto* proxy  = editor.makeFilterableStringModel();
   proxy->setParent(this);
   this->_model = proxy;
   
   this->_combobox = new QComboBox(this);
   this->_combobox->setModel(proxy);

   this->_button = new QPushButton(this);
   this->_button->setText(tr("...", "string picker button"));

   auto layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
   this->setLayout(layout);
   layout->setContentsMargins(0, 0, 0, 0);
   layout->addWidget(this->_combobox, 1);
   layout->addWidget(this->_button,   0);

   QObject::connect(proxy, &ReachStringCentralModel::layoutChanged, this, [this]() {
      this->refreshSelection();
   });
   QObject::connect(proxy, &ReachStringCentralModel::modelReset, this, [this]() {
      this->refreshSelection();
   });
   
   //
   // Using a shared string model, in Qt specifically, means that we no longer have 
   // fine-grained control  of the combobox behavior. QComboBox  is designed on the 
   // assumption that the model you connect to it is THE place where the underlying 
   // data will be edited. We don't honor that assumption: our model is meant to be 
   // a read-only reflection of the underlying string table,  and this QComboBox is 
   // meant to wrap a pointer existing elsewhere and control what said pointer will 
   // point to.
   // 
   // We don't have separate signals for when the underlying string table has items 
   // added, moved, or removed; we just have single signals for when anything in it 
   // changes, and for when a specific string changes. The model handles the former 
   // signal by just resetting...  which prompts the combobox, per its assumptions, 
   // to assume that all the prior data is straight-up gone, forcibly selecting the 
   // first selectable option and blowing away its own state before we get to react 
   // to anything ourselves.
   // 
   // The filthy hack I'm using here is to have the combobox only directly react to 
   // user activation  (even if the user doesn't actually change the target value). 
   // We hook the model signals and update the combobox's selection state ourselves 
   // when the model changes in any way.
   // 
   // The more robust and reliable approach would be to do all string table editing 
   // and management  through the centralized  model we store on  ReachEditorState, 
   // rather than just using that model for UI display optimizations; but I've been 
   // working on this  update for too long  already, and need  to return to my main 
   // projects. If this works, I'll keep it; if it breaks, we'll go back to the old 
   // "have every combobox duplicate the string table in full" garbage.
   // 
   //QObject::connect(this->_combobox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int sel) {
   QObject::connect(this->_combobox, QOverload<int>::of(&QComboBox::activated), this, [this](int sel) {
      if (!this->_targetRef)
         return;
      auto& ref  = *this->_targetRef;
      auto  data = this->_combobox->currentData(ReachStringCentralModel::StringIndexRole);
      if (!data.isValid())
         return;
      auto index = data.toInt();
      if (index <= 0) {
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
      emit selectedStringSwitched();
      emit selectedStringChanged();
   });
   QObject::connect(this->_button, &QPushButton::clicked, this, [this]() {
      if (!this->_targetRef) // should never occur
         return;
      uint32_t flags = 0;
      if (this->_limitToSingleLanguageStrings)
         flags |= Flags::SingleLanguageString;
      if (LocalizedStringEditorModal::editString(this, flags, *this->_targetRef))
         emit selectedStringChanged();
   });
   
   QObject::connect(&editor, &ReachEditorState::variantAbandoned, this, [this]() {
      this->_targetRef = nullptr;
   });
}

void ReachStringPicker::addBlacklistedString(const QString& s) noexcept {
   this->_model->addBlacklistedString(s);
}
void ReachStringPicker::clearBlacklistedStrings() noexcept {
   this->_model->clearBlacklistedStrings();
}
void ReachStringPicker::setAllowNoString(bool is) noexcept {
   this->_model->setAllowNone(is);
}
void ReachStringPicker::setBlacklistedStrings(const QList<QString>& list) noexcept {
   this->_model->clearBlacklistedStrings();
   for (const auto& entry : list)
      this->_model->addBlacklistedString(entry);
}
void ReachStringPicker::setBlacklistedStrings(QList<QString>&& list) noexcept {
   this->_model->clearBlacklistedStrings();
   for (const auto& entry : list)
      this->_model->addBlacklistedString(entry);
}
void ReachStringPicker::setLimitedToSingleLanguageStrings(bool is) noexcept {
   this->_model->setRequireAllLanguagesSame(is);
}

void ReachStringPicker::refreshSelection() {
   if (!this->_targetRef) {
      this->_combobox->setCurrentIndex(-1);
      return;
   }
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
   //const QSignalBlocker blocker(this->_combobox);
   auto itemIndex = this->_combobox->findData(i, ReachStringCentralModel::StringIndexRole);
   if (itemIndex >= 0)
      this->_combobox->setCurrentIndex(itemIndex);
}