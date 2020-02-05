#include "script_editor.h"
#include "../game_variants/data/mp_object_types.h"
#include "localized_string_editor.h"

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
}
MegaloScriptEditorWindow::MegaloScriptEditorWindow(QWidget* parent) : QDialog(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   //
   QObject::connect(this->ui.navigation, &QListWidget::currentItemChanged, this, [this](QListWidgetItem* current, QListWidgetItem* previous) {
      auto stack = this->ui.stack;
      if (current->text() == "Metadata Strings") {
         stack->setCurrentWidget(this->ui.pageMetadata);
         return;
      }
      if (current->text() == "All Other Strings") {
         stack->setCurrentWidget(this->ui.pageStringTable);
         return;
      }
      if (current->text() == "Forge Labels") {
         stack->setCurrentWidget(this->ui.pageForgeLabels);
         return;
      }
      if (current->text() == "Map Permissions") {
         stack->setCurrentWidget(this->ui.pageMapPerms);
         return;
      }
      if (current->text() == "Player Rating Parameters") {
         stack->setCurrentWidget(this->ui.pageRatingParams);
         return;
      }
      if (current->text() == "Required Object Types") {
         stack->setCurrentWidget(this->ui.pageReqObjectTypes);
         return;
      }
      if (current->text() == "Scripted Options") {
         stack->setCurrentWidget(this->ui.pageScriptOptions);
         return;
      }
      if (current->text() == "Scripted Player Traits") {
         stack->setCurrentWidget(this->ui.pageScriptTraits);
         return;
      }
      if (current->text() == "Scripted Stats") {
         stack->setCurrentWidget(this->ui.pageScriptStats);
         return;
      }
      if (current->text() == "Scripted HUD Widgets") {
         stack->setCurrentWidget(this->ui.pageScriptWidgets);
         return;
      }
      if (current->text() == "Gametype Code") {
         stack->setCurrentWidget(this->ui.pageScriptCode);
         return;
      }
   });
   {  // MP object type list
      auto& list   = MPObjectTypeList::get();
      auto& widget = this->ui.reqObjectTypeList;
      widget->clear();
      for (size_t i = 0; i < list.size(); i++) {
         auto& id  = list[i];
         auto item = new QListWidgetItem(QString::fromUtf8(id.name.c_str()), widget);
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
         auto mp    = ReachEditorState::get().multiplayerData();
         if (!mp)
            return;
         auto& list = mp->scriptContent.usedMPObjectTypes;
         if (index >= list.bits.size())
            return;
         list.bits.modify(index, item->data(Qt::ItemDataRole::CheckStateRole) == Qt::CheckState::Checked);
      });
   }
   //
   this->updateFromVariant(nullptr);
}
void MegaloScriptEditorWindow::updateFromVariant(GameVariant* variant) {
   if (!variant) {
      variant = ReachEditorState::get().variant();
      if (!variant)
         return;
   }
   auto mp = variant->get_multiplayer_data();
   if (!mp)
      return;
   { // MP object type list
      auto& types = mp->scriptContent.usedMPObjectTypes;
      //
      auto list = this->ui.reqObjectTypeList;
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
         if (index >= 0 && index < types.bits.size() && types.bits.test(index))
            item->setCheckState(Qt::CheckState::Checked);
      }
   }
}