#include "page_req_obj_types.h"
#include "../../game_variants/data/object_types.h"

ScriptEditorPageReqObjTypes::ScriptEditorPageReqObjTypes(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   //
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &ScriptEditorPageReqObjTypes::updateFromVariant);
   QObject::connect(&editor, &ReachEditorState::variantAbandoned, [this]() {
      this->updateFromVariant();
   });
   //
   auto& list   = Megalo::enums::object_type;
   auto  widget = this->ui.list;
   widget->clear();
   for (size_t i = 0; i < list.size(); i++) {
      auto& definition = list[i];
      auto  name = definition.get_friendly_name();
      if (name.isEmpty())
         name = QString::fromLatin1(definition.name.c_str());
      auto item = new QListWidgetItem(name, widget);
      item->setData(Qt::ItemDataRole::UserRole, i);
      item->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsUserCheckable);
   }
   if (auto m = widget->model())
      m->sort(0);
   QObject::connect(widget, &QListWidget::itemChanged, [](QListWidgetItem* item) {
      auto data = item->data(Qt::ItemDataRole::UserRole);
      if (!data.isValid())
         return;
      auto index = data.toInt();
      if (index < 0)
         return;
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& list = mp->scriptContent.usedMPObjectTypes;
      if (index >= list.bits.size())
         return;
      list.bits.modify(index, item->data(Qt::ItemDataRole::CheckStateRole) == Qt::CheckState::Checked);
   });
   //
   this->updateFromVariant(nullptr);
}
void ScriptEditorPageReqObjTypes::updateFromVariant(GameVariant* variant) {
   GameVariantDataMultiplayer* mp = nullptr;
   if (variant)
      mp = variant->get_multiplayer_data();
   else
      mp = ReachEditorState::get().multiplayerData();
   if (!mp)
      return;
   //
   auto& types = mp->scriptContent.usedMPObjectTypes;
   //
   auto list = this->ui.list;
   auto size = list->count();
   const QSignalBlocker blocker(list);
   for (uint32_t i = 0; i < size; i++) {
      auto item = list->item(i);
      if (!item)
         continue;
      item->setCheckState(Qt::CheckState::Unchecked);
      auto data = item->data(Qt::ItemDataRole::UserRole);
      if (!data.isValid())
         continue;
      auto index = data.toInt();
      if (index >= 0 && index < types.bits.size() && types.bits.test(index))
         item->setCheckState(Qt::CheckState::Checked);
   }
}