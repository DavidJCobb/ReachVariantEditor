#include "page_script_traits.h"

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
   // TODO: buttons
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
      this->target = list[i];
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
      auto& traits = *list[i];
      auto  item = new QListWidgetItem;
      if (traits.name)
         item->setText(QString::fromUtf8(traits.name->english().c_str()));
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