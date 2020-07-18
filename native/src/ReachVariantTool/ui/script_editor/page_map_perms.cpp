#include "page_map_perms.h"
#include "../../game_variants/types/multiplayer.h"
#include "../../game_variants/components/map_permissions.h"

namespace {
   struct _MapID {
      QString  name;
      uint16_t index;
      //
      _MapID(uint16_t i, QString n) : index(i), name(n) {}
      _MapID(uint16_t i) : index(i), name(QObject::tr("Unknown %1").arg(i)) {}
   };
   constexpr char* disambig = "Reach map names";
   _MapID _mapIDList[] = {
      _MapID(1),
      _MapID(1000, QObject::tr("Sword Base", disambig)),
      _MapID(1001, QObject::tr("Unused (DLC Sword Base)", disambig)),
      _MapID(1020, QObject::tr("Countdown", disambig)),
      _MapID(1035, QObject::tr("Boardwalk", disambig)),
      _MapID(1040, QObject::tr("Zealot", disambig)),
      _MapID(1055, QObject::tr("Powerhouse", disambig)),
      _MapID(1056, QObject::tr("Unused (DLC Powerhouse)", disambig)),
      _MapID(1080, QObject::tr("Boneyard", disambig)),
      _MapID(1150, QObject::tr("Reflection", disambig)),
      _MapID(1200, QObject::tr("Spire", disambig)),
      _MapID(1500, QObject::tr("Condemned", disambig)),
      _MapID(1510, QObject::tr("Highlands", disambig)),
      _MapID(2001, QObject::tr("Anchor 9", disambig)),
      _MapID(2002, QObject::tr("Breakpoint", disambig)),
      _MapID(2004, QObject::tr("Tempest", disambig)),
      _MapID(3006, QObject::tr("Forge World", disambig)),
      /*// No point in letting the user specify these
      _MapID(5005, QObject::tr("Campaign Mission 0: NOBLE Actual", disambig)),
      _MapID(5010, QObject::tr("Campaign Mission 1: Winter Contingency", disambig)),
      _MapID(5020, QObject::tr("Campaign Mission 2: ONI: Sword Base", disambig)),
      _MapID(5030, QObject::tr("Campaign Mission 3: Nightfall", disambig)),
      _MapID(5035, QObject::tr("Campaign Mission 4: Tip of the Spear", disambig)),
      _MapID(5045, QObject::tr("Campaign Mission 5: Long Night of Solace", disambig)),
      _MapID(5050, QObject::tr("Campaign Mission 6: Exodus", disambig)),
      _MapID(5052, QObject::tr("Campaign Mission 7: New Alexandria", disambig)),
      _MapID(5060, QObject::tr("Campaign Mission 8: The Package", disambig)),
      _MapID(5070, QObject::tr("Campaign Mission 9: The Pillar of Autumn", disambig)),
      _MapID(5080, QObject::tr("Campaign Mission 10: Lone Wolf", disambig)),
      _MapID(7000, QObject::tr("Firefight: Overlook", disambig)),
      _MapID(7020, QObject::tr("Firefight: Courtyard", disambig)),
      _MapID(7030, QObject::tr("Firefight: Outpost", disambig)),
      _MapID(7040, QObject::tr("Firefight: Waterfront", disambig)),
      _MapID(7060, QObject::tr("Firefight: Beachhead", disambig)),
      _MapID(7080, QObject::tr("Firefight: Holdout", disambig)),
      _MapID(7110, QObject::tr("Firefight: Corvette", disambig)),
      _MapID(7130, QObject::tr("Firefight: Glacier", disambig)),
      _MapID(7500, QObject::tr("Firefight: Unearthed", disambig)),
      //*/
      _MapID(10010, QObject::tr("Penance", disambig)),
      _MapID(10020, QObject::tr("Battle Canyon", disambig)),
      _MapID(10030, QObject::tr("Ridgeline", disambig)),
      _MapID(10050, QObject::tr("Breakneck", disambig)),
      _MapID(10060, QObject::tr("High Noon", disambig)),
      _MapID(10070, QObject::tr("Solitary", disambig)),
      /*// No point in letting the user specify these
      _MapID(10080, QObject::tr("Firefight: Installation 04", disambig)),
      //*/
   };
   static_assert(std::extent<decltype(_mapIDList)>::value <= ReachMapPermissions::max_count, "There's a limit on how many map IDs the game variant's map permissions can list.");
}
ScriptEditorPageMapPerms::ScriptEditorPageMapPerms(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   //
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &ScriptEditorPageMapPerms::updateFromVariant);
   QObject::connect(&editor, &ReachEditorState::variantAbandoned, [this]() {
      this->updateFromVariant();
   });
   //
   auto& list = this->ui.list;
   for (uint32_t i = 0; i < std::extent<decltype(_mapIDList)>::value; i++) {
      auto& id = _mapIDList[i];
      auto item = new QListWidgetItem(id.name, list);
      item->setData(Qt::ItemDataRole::UserRole, id.index);
      item->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsUserCheckable);
   }
   if (auto m = list->model())
      m->sort(0);
   QObject::connect(this->ui.type, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int index) {
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      mp->mapPermissions.type = (reach::map_permission_type)index;
   });
   QObject::connect(list, &QListWidget::itemChanged, [](QListWidgetItem* item) {
      auto data = item->data(Qt::ItemDataRole::UserRole);
      if (!data.isValid())
         return;
      auto index = data.toInt();
      auto mp    = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& list = mp->mapPermissions.mapIDs;
      auto  it   = std::find(list.begin(), list.end(), index);
      if (item->data(Qt::ItemDataRole::CheckStateRole) == Qt::CheckState::Checked) {
         if (it != list.end())
            return;
         list.push_back(index);
      } else {
         if (it == list.end())
            return;
         list.erase(it);
      }
   });
   //
   this->updateFromVariant(nullptr);
}
void ScriptEditorPageMapPerms::updateFromVariant(GameVariant* variant) {
   GameVariantDataMultiplayer* mp = nullptr;
   if (variant)
      mp = variant->get_multiplayer_data();
   else
      mp = ReachEditorState::get().multiplayerData();
   if (!mp)
      return;
   const QSignalBlocker blocker0(this->ui.type);
   const QSignalBlocker blocker1(this->ui.list);
   //
   auto& perms = mp->mapPermissions;
   this->ui.type->setCurrentIndex((int)perms.type);
   //
   auto list = this->ui.list;
   auto size = list->count();
   for (uint32_t i = 0; i < size; i++) {
      auto item = list->item(i);
      if (!item)
         continue;
      item->setCheckState(Qt::CheckState::Unchecked);
      auto data = item->data(Qt::ItemDataRole::UserRole);
      if (!data.isValid())
         continue;
      auto index = data.toInt();
      for (auto mapID : perms.mapIDs) {
         if (mapID == index) {
            item->setCheckState(Qt::CheckState::Checked);
            break;
         }
      }
   }
}