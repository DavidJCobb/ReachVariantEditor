#include "page_script_traits.h"
#include <QMessageBox>

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
}
ScriptEditorPageScriptTraits::ScriptEditorPageScriptTraits(QWidget* parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   //
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &ScriptEditorPageScriptTraits::updateTraitsListFromVariant);
   QObject::connect(&editor, &ReachEditorState::variantAbandoned, [this]() {
      this->target = nullptr;
      this->updateTraitsListFromVariant();
   });
   //
   QObject::connect(this->ui.list, &QListWidget::currentRowChanged, this, &ScriptEditorPageScriptTraits::selectTraits);
   //
   QObject::connect(this->ui.name, &ReachStringPicker::selectedStringChanged, [this]() {
      this->updateTraitsListFromVariant();
      ReachEditorState::get().scriptTraitsModified(this->target);
   });
   QObject::connect(this->ui.desc, &ReachStringPicker::selectedStringChanged, [this]() {
      ReachEditorState::get().scriptTraitsModified(this->target); // main window shows descriptions as tooltips
   });
   //
   QObject::connect(this->ui.buttonNew, &QPushButton::clicked, [this]() {
      auto& editor = ReachEditorState::get();
      auto  mp     = editor.multiplayerData();
      if (!mp)
         return;
      auto& list = mp->scriptData.traits;
      if (list.size() >= Megalo::Limits::max_script_options) {
         QMessageBox::information(this, tr("Cannot add player traits"), tr("Game variants cannot have more than %1 sets of player traits.").arg(Megalo::Limits::max_script_traits));
         return;
      }
      this->target = list.emplace_back();
      this->target->is_defined = true;
      {  // If possible, default the new trait-set's name and description to an empty string
         auto str = mp->scriptData.strings.get_empty_entry();
         if (str) {
            this->target->name = str;
            this->target->desc = str;
         }
      }
      this->updateTraitsFromVariant();
      this->updateTraitsListFromVariant();
      ReachEditorState::get().scriptTraitsModified(nullptr);
   });
   QObject::connect(this->ui.buttonMoveUp, &QPushButton::clicked, [this]() {
      if (!this->target)
         return;
      auto& editor = ReachEditorState::get();
      auto  mp     = editor.multiplayerData();
      if (!mp)
         return;
      auto& list  = mp->scriptData.traits;
      auto  index = list.index_of(this->target);
      if (index < 0)
         return;
      if (index == 0) // can't move the first item up
         return;
      list.swap_items(index, index - 1);
      this->updateTraitsListFromVariant();
      ReachEditorState::get().scriptTraitsModified(nullptr);
   });
   QObject::connect(this->ui.buttonMoveDown, &QPushButton::clicked, [this]() {
      if (!this->target)
         return;
      auto& editor = ReachEditorState::get();
      auto  mp     = editor.multiplayerData();
      if (!mp)
         return;
      auto& list  = mp->scriptData.traits;
      auto  index = list.index_of(this->target);
      if (index < 0)
         return;
      if (index == list.size() - 1) // can't move the last item down
         return;
      list.swap_items(index, index + 1);
      this->updateTraitsListFromVariant();
      ReachEditorState::get().scriptTraitsModified(nullptr);
   });
   QObject::connect(this->ui.buttonDelete, &QPushButton::clicked, [this]() {
      if (!this->target)
         return;
      if (this->target->get_refcount()) {
         QMessageBox::information(this, tr("Cannot remove player traits"), tr("This set of player traits is still in use by the gametype's script. It cannot be removed at this time."));
         return;
      }
      auto& editor = ReachEditorState::get();
      auto  mp     = editor.multiplayerData();
      if (!mp)
         return;
      auto& list  = mp->scriptData.traits;
      auto  index = list.index_of(this->target);
      if (index < 0)
         return;
      list.erase(index);
      size_t size = list.size();
      if (size) {
         if (index >= size)
            this->target = &list[size - 1];
         else
            this->target = &list[index];
      } else
         this->target = nullptr;
      this->updateTraitsFromVariant();
      this->updateTraitsListFromVariant();
      ReachEditorState::get().scriptTraitsModified(nullptr);
   });
   //
   this->updateTraitsListFromVariant();
}

void ScriptEditorPageScriptTraits::selectTraits(int32_t i) {
   if (i < 0) {
      this->target = nullptr;
   } else {
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& list = mp->scriptData.traits;
      if (i >= list.size())
         return;
      this->target = &list[i];
   }
   this->updateTraitsFromVariant();
}

void ScriptEditorPageScriptTraits::updateTraitsListFromVariant(GameVariant* variant) {
   auto widget = this->ui.list;
   const QSignalBlocker blocker(widget);
   //
   widget->clear();
   //
   GameVariantDataMultiplayer* mp = nullptr;
   if (variant)
      mp = variant->get_multiplayer_data();
   else
      mp = ReachEditorState::get().multiplayerData();
   if (!mp)
      return;
   auto& list = mp->scriptData.traits;
   for (size_t i = 0; i < list.size(); i++) {
      auto& traits = list[i];
      auto  item = new QListWidgetItem;
      if (traits.name)
         item->setText(QString::fromUtf8(traits.name->get_content(reach::language::english).c_str()));
      else
         item->setText(tr("<unnamed traits %1>", "scripted traits editor").arg(i));
      item->setData(Qt::ItemDataRole::UserRole, QVariant::fromValue((void*)&traits));
      widget->addItem(item);
   }
   if (this->target)
      _selectByPointerData(widget, this->target);
}
void ScriptEditorPageScriptTraits::updateTraitsFromVariant() {
   const QSignalBlocker blocker3(this->ui.name);
   const QSignalBlocker blocker4(this->ui.desc);
   //
   if (!this->target) {
      this->ui.name->clearTarget();
      this->ui.desc->clearTarget();
      return;
   }
   auto& t = *this->target;
   this->ui.name->setTarget(t.name);
   this->ui.desc->setTarget(t.desc);
}