#include "option_toggles.h"
#include <QHeaderView>
#include <QMenu>
#include "../../editor_state.h"

#pragma region OptionToggleModelItem
void OptionToggleModelItem::_destroyDescendants() noexcept {
   for (auto& child : this->_children) {
      child->_destroyDescendants();
      delete child;
      child = nullptr;
   }
   this->_children.clear();
}
void OptionToggleModelItem::appendChild(OptionToggleModelItem* child) noexcept {
   auto p = child->parent();
   if (p)
      p->removeChild(child);
   this->_children.push_back(child);
   child->_parent = this;
}
bool OptionToggleModelItem::childrenContainOptionIndex(int32_t i) const noexcept {
   for (auto& child : this->_children) {
      if (child->index() == i)
         return true;
      if (child->childrenContainOptionIndex(i))
         return true;
   }
   return false;
}
int32_t OptionToggleModelItem::indexOf(OptionToggleModelItem* child) const noexcept {
   for (size_t i = 0; i < this->_children.size(); i++)
      if (this->_children[i] == child)
         return i;
   return -1;
}
void OptionToggleModelItem::removeChild(OptionToggleModelItem* child) noexcept {
   auto& list = this->_children;
   list.erase(std::remove(list.begin(), list.end(), child), list.end());
   child->_parent = nullptr;
}
#pragma endregion

#pragma region OptionToggleModel
bool OptionToggleModel::hasOptionIndex(int32_t i) const noexcept {
   return this->root->childrenContainOptionIndex(i);
}
QModelIndex OptionToggleModel::index(int row, int column, const QModelIndex& parent) const {
   if (!this->hasIndex(row, column, parent))
      return QModelIndex();
   item_type* parentItem;
   if (!parent.isValid())
      parentItem = this->root;
   else
      parentItem = static_cast<item_type*>(parent.internalPointer());
   item_type* childItem = parentItem->child(row);
   if (childItem)
      return this->createIndex(row, column, childItem);
   return QModelIndex();
}
QModelIndex OptionToggleModel::parent(const QModelIndex& index) const {
   if (!index.isValid())
      return QModelIndex();
   item_type* childItem  = static_cast<item_type*>(index.internalPointer());
   item_type* parentItem = childItem->parent();
   if (parentItem == this->root)
      return QModelIndex();
   auto i = parentItem->parent()->indexOf(parentItem);
   return createIndex(i, 0, parentItem);
}
int OptionToggleModel::rowCount(const QModelIndex& parent) const {
   item_type* parentItem;
   if (parent.column() > 0)
      return 0;
   if (!parent.isValid())
      parentItem = this->root;
   else
      parentItem = static_cast<item_type*>(parent.internalPointer());
   return parentItem->childCount();
}
int OptionToggleModel::columnCount(const QModelIndex& item) const {
   return 3;
}
Qt::ItemFlags OptionToggleModel::flags(const QModelIndex& index) const {
   if (!index.isValid())
      return Qt::NoItemFlags;
   auto item = static_cast<item_type*>(index.internalPointer());
   if (index.column() <= 0) {
      return Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable;
   }
   if (item->index() == OptionToggleModelItem::container)
      return Qt::ItemFlag::NoItemFlags;
   auto enabled = Qt::ItemFlag::ItemIsEnabled;
   if (!item->isOption())
      enabled = Qt::ItemFlag::NoItemFlags;
   return enabled | Qt::ItemFlag::ItemIsUserCheckable;
}
QVariant OptionToggleModel::data(const QModelIndex& index, int role) const {
   if (!index.isValid())
      return QVariant();
   auto item = static_cast<item_type*>(index.internalPointer());
   if (role == Qt::CheckStateRole) {
      OptionToggleFlagType type;
      switch (index.column()) {
         case 1: type = OptionToggleFlagType::disabled; break;
         case 2: type = OptionToggleFlagType::hidden;   break;
         default:
            return QVariant();
      }
      if (item->isOption())
         return this->getFlag(item, type) ? Qt::Checked : Qt::Unchecked;
      if (item->index() != OptionToggleModelItem::container_autohide) {
         return QVariant();
      }
      bool allSet = true;
      for (auto& child : item->children()) {
         if (child->isOption()) {
            if (!this->getFlag(child, type)) {
               allSet = false;
               break;
            }
         }
      }
      return allSet ? Qt::Checked : Qt::Unchecked;
   }
   if (role != Qt::DisplayRole)
      return QVariant();
   if (index.column() == 0)
      return item->name();
   return QVariant();
}
bool OptionToggleModel::setData(const QModelIndex& index, const QVariant& value, int role) {
   if (role != Qt::CheckStateRole || !index.isValid())
      return false;
   auto item = static_cast<item_type*>(index.internalPointer());
   if (!item->isOption())
      return false;
   bool result = false;
   auto column = index.column();
   if (column == 0 || column > 2)
      return false;
   Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
   if (column == 1)
      this->modifyFlag(item, OptionToggleFlagType::disabled, state == Qt::Checked);
   else if (column == 2)
      this->modifyFlag(item, OptionToggleFlagType::hidden, state == Qt::Checked);
   emit dataChanged(index, index);
   if (auto parent = item->parent()) {
      if (parent != this->root && parent->index() == OptionToggleModelItem::container_autohide) {
         //
         // An auto-hiding submenu's checkbox state depends on the checkbox states of its 
         // children, so when a child changes the parent must change as well.
         //
         auto grandparent = parent->parent();
         auto index       = this->createIndex(grandparent->indexOf(parent), column, parent);
         emit dataChanged(index, index);
      }
   }
   return true;
}
//
QVariant OptionToggleModel::headerData(int section, Qt::Orientation orientation, int role) const {
   if (role == Qt::DisplayRole) {
      switch (section) {
         case 0: return tr("Option Name", "option toggles list");
         case 1: return tr("Disabled",    "option toggles list");
         case 2: return tr("Hidden",      "option toggles list");
      }
   }
   return QVariant();
}
//
void OptionToggleModel::clear() {
   this->beginResetModel();
   this->root->_destroyDescendants();
   this->endResetModel();
}
#pragma endregion

#pragma region EngineOptionToggleModel
EngineOptionToggleModel::bitset_type* EngineOptionToggleModel::_getToggles(OptionToggleFlagType type) const noexcept {
   auto data = ReachEditorState::get().multiplayerData();
   if (!data)
      return nullptr;
   auto& flagset = data->optionToggles.engine;
   switch (type) {
      case OptionToggleFlagType::disabled: return &flagset.disabled;
      case OptionToggleFlagType::hidden:   return &flagset.hidden;
   }
   return nullptr;
}
//
bool EngineOptionToggleModel::getFlag(const item_type* item, OptionToggleFlagType type) const noexcept {
   if (!item)
      return false;
   auto index = item->index();
   if (index < 0) {
      bool allSet = true;
      for (auto& child : item->children()) {
         if (child->isOption()) {
            if (!this->getFlag(child, type)) {
               allSet = false;
               break;
            }
         }
      }
      return allSet;
   }
   if (index >= bitset_type::flag_count)
      return false;
   auto toggles = this->_getToggles(type);
   if (!toggles)
      return false;
   return toggles->bits.test(index);
}
void EngineOptionToggleModel::modifyFlag(const item_type* item, OptionToggleFlagType type, bool state) const noexcept {
   if (!item)
      return;
   auto index = item->index();
   if (index < 0) {
      for (auto& child : item->children())
         this->modifyFlag(child, type, state);
      return;
   }
   if (index >= bitset_type::flag_count)
      return;
   auto toggles = this->_getToggles(type);
   if (!toggles)
      return;
   return toggles->bits.modify(index, state);
}
void EngineOptionToggleModel::modifyFlagForAll(OptionToggleFlagType type, bool state) const noexcept {
   auto toggles = this->_getToggles(type);
   if (!toggles)
      return;
   for (uint16_t i = 0; i < bitset_type::flag_count; i++)
      toggles->bits.modify(i, state);
}
#pragma endregion

#pragma region MegaloOptionToggleModel
MegaloOptionToggleModel::bitset_type* MegaloOptionToggleModel::_getToggles(OptionToggleFlagType type) const noexcept {
   auto data = ReachEditorState::get().multiplayerData();
   if (!data)
      return nullptr;
   auto& flagset = data->optionToggles.megalo;
   switch (type) {
      case OptionToggleFlagType::disabled: return &flagset.disabled;
      case OptionToggleFlagType::hidden:   return &flagset.hidden;
   }
   return nullptr;
}
//
bool MegaloOptionToggleModel::getFlag(const item_type* item, OptionToggleFlagType type) const noexcept {
   if (!item)
      return false;
   auto index = item->index();
   if (index < 0) {
      bool allSet = true;
      for (auto& child : item->children()) {
         if (child->isOption()) {
            if (!this->getFlag(child, type)) {
               allSet = false;
               break;
            }
         }
      }
      return allSet;
   }
   if (index >= bitset_type::flag_count)
      return false;
   auto toggles = this->_getToggles(type);
   if (!toggles)
      return false;
   return toggles->bits.test(index);
}
void MegaloOptionToggleModel::modifyFlag(const item_type* item, OptionToggleFlagType type, bool state) const noexcept {
   if (!item)
      return;
   auto index = item->index();
   if (index < 0) {
      for (auto& child : item->children())
         this->modifyFlag(child, type, state);
      return;
   }
   if (index >= bitset_type::flag_count)
      return;
   auto toggles = this->_getToggles(type);
   if (!toggles)
      return;
   return toggles->bits.modify(index, state);
}
void MegaloOptionToggleModel::modifyFlagForAll(OptionToggleFlagType type, bool state) const noexcept {
   auto toggles = this->_getToggles(type);
   if (!toggles)
      return;
   for (uint16_t i = 0; i < bitset_type::flag_count; i++)
      toggles->bits.modify(i, state);
}
#pragma endregion

#pragma region OptionToggleTree - base class for the tree view
OptionToggleTree::OptionToggleTree(QWidget* parent) : QTreeView(parent) {
   this->setSelectionMode(QAbstractItemView::ExtendedSelection);
   {  // HeaderContext menu items
      {  // Disabled
         this->actionDisableCheckAll = new QAction(tr("Check All",   "Option Toggles: Disabled"), this);
         this->actionDisableClearAll = new QAction(tr("Uncheck All", "Option Toggles: Disabled"), this);
         QObject::connect(this->actionDisableCheckAll, &QAction::triggered, [this]() {
            auto model = static_cast<OptionToggleModel*>(this->model());
            model->modifyFlagForAll(OptionToggleFlagType::disabled, true);
         });
         QObject::connect(this->actionDisableClearAll, &QAction::triggered, [this]() {
            auto model = static_cast<OptionToggleModel*>(this->model());
            model->modifyFlagForAll(OptionToggleFlagType::disabled, false);
         });
      }
      {  // Hidden
         this->actionHiddenCheckAll = new QAction(tr("Check All",   "Option Toggles: Hidden"), this);
         this->actionHiddenClearAll = new QAction(tr("Uncheck All", "Option Toggles: Hidden"), this);
         QObject::connect(this->actionHiddenCheckAll, &QAction::triggered, [this]() {
            auto model = static_cast<OptionToggleModel*>(this->model());
            model->modifyFlagForAll(OptionToggleFlagType::hidden, true);
         });
         QObject::connect(this->actionHiddenClearAll, &QAction::triggered, [this]() {
            auto model = static_cast<OptionToggleModel*>(this->model());
            model->modifyFlagForAll(OptionToggleFlagType::hidden, false);
         });
      }
      auto* header = this->header();
      if (header) {
         header->setContextMenuPolicy(Qt::CustomContextMenu);
         QObject::connect(header, &QWidget::customContextMenuRequested, [this, header](const QPoint& pos) {
            QPoint globalPos = header->mapToGlobal(pos);
            int col = header->logicalIndexAt(pos);
            this->showHeaderContextMenu(globalPos, col);
         });
      }
   }
   {  // Body context menu
      this->actionDisableCheckAllSelected = new QAction(tr("Disable All Selected", "Option Toggles"), this);
      this->actionDisableClearAllSelected = new QAction(tr("Enable All Selected",  "Option Toggles"), this);
      this->actionHiddenCheckAllSelected  = new QAction(tr("Hide All Selected",    "Option Toggles"), this);
      this->actionHiddenClearAllSelected  = new QAction(tr("Show All Selected",    "Option Toggles"), this);
      QObject::connect(this->actionDisableCheckAllSelected, &QAction::triggered, [this]() {
         this->doneBodyContextMenu(OptionToggleFlagType::disabled, true);
      });
      QObject::connect(this->actionDisableClearAllSelected, &QAction::triggered, [this]() {
         this->doneBodyContextMenu(OptionToggleFlagType::disabled, false);
      });
      QObject::connect(this->actionHiddenCheckAllSelected, &QAction::triggered, [this]() {
         this->doneBodyContextMenu(OptionToggleFlagType::hidden, true);
      });
      QObject::connect(this->actionHiddenClearAllSelected, &QAction::triggered, [this]() {
         this->doneBodyContextMenu(OptionToggleFlagType::hidden, false);
      });
      //
      this->setContextMenuPolicy(Qt::CustomContextMenu);
      QObject::connect(this, &QWidget::customContextMenuRequested, [this](const QPoint& pos) {
         this->showBodyContextMenu(this->mapToGlobal(pos));
      });
   }
}
void OptionToggleTree::showHeaderContextMenu(const QPoint& pos, int column) {
   if (column != 1 && column != 2)
      return;
   QMenu menu(this);
   if (column == 1) {
      menu.addAction(this->actionDisableCheckAll);
      menu.addAction(this->actionDisableClearAll);
   } else if (column == 2) {
      menu.addAction(this->actionHiddenCheckAll);
      menu.addAction(this->actionHiddenClearAll);
   }
   menu.exec(pos);
}
void OptionToggleTree::showBodyContextMenu(const QPoint& screenPos) {
   QMenu menu(this);
   menu.addAction(this->actionDisableCheckAllSelected);
   menu.addAction(this->actionDisableClearAllSelected);
   menu.addAction(this->actionHiddenCheckAllSelected);
   menu.addAction(this->actionHiddenClearAllSelected);
   menu.exec(screenPos);
}
void OptionToggleTree::doneBodyContextMenu(OptionToggleFlagType which, bool check) {
   auto sel = this->selectionModel();
   auto rows = sel->selectedRows();
   auto model = dynamic_cast<OptionToggleModel*>(this->model());
   for (const auto& row : rows) {
      const auto* item = static_cast<OptionToggleModel::item_type*>(row.internalPointer());
      if (!item)
         continue;
      model->modifyFlag(item, which, check);
   }
}
#pragma endregion

#pragma region EngineOptionToggleTree - the tree view
EngineOptionToggleTree::EngineOptionToggleTree(QWidget* parent) : OptionToggleTree(parent) {
   this->setModel(new model_type);
   {
      auto* header = this->header();
      if (header) {
         header->setStretchLastSection(false);
         header->setSectionResizeMode(0, QHeaderView::Stretch);
         header->setSectionResizeMode(1, QHeaderView::ResizeToContents); // doing this in the superclass asserts; there has to already be a model which defines multiple columns
         header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
         header->setSectionsMovable(false);
      }
   }

   //
   // NOTES:
   //
   //  - Toggles 0000 - 0159 don't contain anything inside of player traits, or anything 
   //    inside of loadouts.
   //
   //  - Toggles 0320 - 0478 contain all player traits?
   //
   //     - Toggles 0361 - 0399 contain all of the "defense" player traits.
   //
   //  - Toggles 0956 - 1114 contain Spartan Tier 1 loadouts 1, 2, and 3, as well as all 
   //    settings except grenade count in loadout 4.
   //
   //  - Toggles 1115 - 1271 contain Spartan Tier 1 loadout 4's grenade count, all of 
   //    loadout 5, and all other loadouts for Spartans and Elites EXCEPT the grenade 
   //    count for Elite Tier 3 loadout 5.
   //
   //     - Each loadout has six flags. That's 30 flags per loadout palette, for a total 
   //       of 180 flags for all loadouts.
   //

   constexpr char* disambig = "engine option toggles";
   auto model = static_cast<OptionToggleModel*>(this->model());
   {
      auto parent = new OptionToggleModelItem(OptionToggleModelItem::container, tr("General Settings", disambig));
      model->invisibleRootItem()->appendChild(parent);
      //
      parent->appendChild(new OptionToggleModelItem(0, tr("Score to Win",  disambig)));
      parent->appendChild(new OptionToggleModelItem(1, tr("Teams Enabled", disambig)));
      parent->appendChild(new OptionToggleModelItem(3, tr("Round Time Limit", disambig)));
      parent->appendChild(new OptionToggleModelItem(4, tr("Sudden Death", disambig)));
      parent->appendChild(new OptionToggleModelItem(6, tr("Number of Rounds", disambig)));
   }
   {
      auto parent = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Respawn Settings", disambig));
      model->invisibleRootItem()->appendChild(parent);
      //
      parent->appendChild(new OptionToggleModelItem(8, tr("Synchronize with Team", disambig)));
      parent->appendChild(new OptionToggleModelItem(12, tr("Lives per Round", disambig)));
      parent->appendChild(new OptionToggleModelItem(14, tr("Respawn Time", disambig)));
      parent->appendChild(new OptionToggleModelItem(15, tr("Suicide Penalty", disambig)));
      parent->appendChild(new OptionToggleModelItem(16, tr("Betrayal Penalty", disambig)));
      parent->appendChild(new OptionToggleModelItem(17, tr("Respawn Time Growth", disambig)));
      parent->appendChild(new OptionToggleModelItem(19, tr("Respawn Traits Duration", disambig)));
   }
   {
      auto parent = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Social Settings", disambig));
      model->invisibleRootItem()->appendChild(parent);
      //
      parent->appendChild(new OptionToggleModelItem(57, tr("Friendly Fire", disambig)));
      parent->appendChild(new OptionToggleModelItem(58, tr("Betrayal Booting", disambig)));
   }
   {
      auto parent = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Team Settings", disambig));
      model->invisibleRootItem()->appendChild(parent);
      //
      parent->appendChild(new OptionToggleModelItem(56, tr("Team Changing", disambig)));
   }
   {
      auto parent = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Map and Game Settings", disambig));
      model->invisibleRootItem()->appendChild(parent);
      //
      parent->appendChild(new OptionToggleModelItem(62, tr("Grenades on Map", disambig)));
      parent->appendChild(new OptionToggleModelItem(63, tr("Abilities on Map", disambig)));
      parent->appendChild(new OptionToggleModelItem(64, tr("Turrets on Map", disambig)));
      parent->appendChild(new OptionToggleModelItem(66, tr("Powerups on Map", disambig)));
      parent->appendChild(new OptionToggleModelItem(67, tr("Indestructible Vehicles", disambig)));
      parent->appendChild(new OptionToggleModelItem(80, tr("Primary Weapon", disambig)));
      parent->appendChild(new OptionToggleModelItem(81, tr("Secondary Weapon", disambig)));
      parent->appendChild(new OptionToggleModelItem(103, tr("Weapon Set", disambig)));
      parent->appendChild(new OptionToggleModelItem(104, tr("Vehicle Set", disambig)));
   }
   {
      auto base = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Loadouts", disambig));
      model->invisibleRootItem()->appendChild(base);
      //
      constexpr uint8_t options_in_loadout = 6;
      constexpr uint8_t loadouts_in_palette = 5;
      constexpr uint8_t palettes_per_species = 3;
      //
      // 1092 == Spartan Tier 1 Loadout 1 Hide Loadout
      // 1093 == Spartan Tier 1 Loadout 1 Name
      // 1094 == Spartan Tier 1 Loadout 1 Primary Weapon
      // 1181 == Spartan Tier 3 Loadout 5 Grenade Count
      // 1182 == Unknown
      // 1183 == Elite   Tier 1 Loadout 1 Hide Loadout
      // 1266 == Elite   Tier 3 Loadout 4 Grenade Count
      // 1267 == Elite   Tier 3 Loadout 5 Hide Loadout
      // 1272 == Elite   Tier 3 Loadout 5 Grenade Count
      //
      uint16_t baseIndex = 1092;
      for (uint8_t i = 0; i < palettes_per_species; i++) {
         auto tier = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Spartan Tier %1", disambig).arg(i + 1));
         base->appendChild(tier);
         for (uint8_t j = 0; j < loadouts_in_palette; j++) {
            auto parent = new OptionToggleModelItem(OptionToggleModelItem::container_autohide, tr("Loadout %1", disambig).arg(j + 1));
            tier->appendChild(parent);
            //
            uint16_t oi = baseIndex + (j * options_in_loadout) + (i * loadouts_in_palette * options_in_loadout);
            //
            parent->appendChild(new OptionToggleModelItem(oi + 1, tr("Name", disambig)));
            parent->appendChild(new OptionToggleModelItem(oi + 2, tr("Primary Weapon", disambig)));
            parent->appendChild(new OptionToggleModelItem(oi + 3, tr("Secondary Weapon", disambig)));
            parent->appendChild(new OptionToggleModelItem(oi + 4, tr("Armor Ability", disambig)));
            parent->appendChild(new OptionToggleModelItem(oi + 5, tr("Grenade Count", disambig))); // 1272
            parent->appendChild(new OptionToggleModelItem(oi + 0, tr("Hide Loadout", disambig)));  // 1267
         }
      }
      // skip 1182; we don't know what it is, but it's a gap between the two species' loadout palettes
      baseIndex = 1183;
      for (uint8_t i = 0; i < palettes_per_species; i++) {
         auto tier = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Elite Tier %1", disambig).arg(i + 1));
         base->appendChild(tier);
         for (uint8_t j = 0; j < loadouts_in_palette; j++) {
            auto parent = new OptionToggleModelItem(OptionToggleModelItem::container_autohide, tr("Loadout %1", disambig).arg(j + 1));
            tier->appendChild(parent);
            //
            uint16_t oi = baseIndex + (j * options_in_loadout) + (i * loadouts_in_palette * options_in_loadout);
            //
            parent->appendChild(new OptionToggleModelItem(oi + 1, tr("Name", disambig)));
            parent->appendChild(new OptionToggleModelItem(oi + 2, tr("Primary Weapon", disambig)));
            parent->appendChild(new OptionToggleModelItem(oi + 3, tr("Secondary Weapon", disambig)));
            parent->appendChild(new OptionToggleModelItem(oi + 4, tr("Armor Ability", disambig)));
            parent->appendChild(new OptionToggleModelItem(oi + 5, tr("Grenade Count", disambig))); // 1272
            parent->appendChild(new OptionToggleModelItem(oi + 0, tr("Hide Loadout", disambig)));  // 1267
         }
      }
      /*//
      {
         auto tier = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Elite Tier 3", disambig));
         base->appendChild(tier);
         //
         {
            auto parent = new OptionToggleModelItem(OptionToggleModelItem::container_autohide, tr("Loadout Epsilon", disambig));
            tier->appendChild(parent);
            //
            parent->appendChild(new OptionToggleModelItem(1268, tr("Name", disambig)));
            parent->appendChild(new OptionToggleModelItem(1269, tr("Primary Weapon", disambig)));
            parent->appendChild(new OptionToggleModelItem(1270, tr("Secondary Weapon", disambig)));
            parent->appendChild(new OptionToggleModelItem(1271, tr("Armor Ability", disambig)));
            parent->appendChild(new OptionToggleModelItem(1272, tr("Grenade Count", disambig)));
            parent->appendChild(new OptionToggleModelItem(1267, tr("Hide Loadout", disambig)));
         }
      }*/
   }
   {
      auto traits = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Player Traits", disambig));
      model->invisibleRootItem()->appendChild(traits);
      //
      {
         auto parent = new OptionToggleModelItem(OptionToggleModelItem::container_autohide, tr("Defense", disambig));
         traits->appendChild(parent);
         //
         parent->appendChild(new OptionToggleModelItem(391, tr("Damage Resistance", disambig)));
         // 392: Health Multiplier?
         parent->appendChild(new OptionToggleModelItem(393, tr("Health Recharge Rate", disambig)));
         parent->appendChild(new OptionToggleModelItem(394, tr("Shield Multiplier", disambig)));
         parent->appendChild(new OptionToggleModelItem(395, tr("Shield Recharge Rate", disambig)));
         parent->appendChild(new OptionToggleModelItem(397, tr("Headshot Immunity", disambig)));
         parent->appendChild(new OptionToggleModelItem(398, tr("Assassination Immunity", disambig)));
      }
      {
         auto parent = new OptionToggleModelItem(OptionToggleModelItem::container_autohide, tr("Offense", disambig));
         traits->appendChild(parent);
         //
         parent->appendChild(new OptionToggleModelItem(401, tr("Damage Multiplier", disambig)));
         parent->appendChild(new OptionToggleModelItem(402, tr("Melee Multiplier", disambig)));
         parent->appendChild(new OptionToggleModelItem(403, tr("Primary Weapon", disambig)));
         parent->appendChild(new OptionToggleModelItem(404, tr("Secondary Weapon", disambig)));
         parent->appendChild(new OptionToggleModelItem(405, tr("Armor Ability", disambig)));
         parent->appendChild(new OptionToggleModelItem(406, tr("Grenade Count", disambig)));
         parent->appendChild(new OptionToggleModelItem(408, tr("Infinite Ammo", disambig)));
         parent->appendChild(new OptionToggleModelItem(410, tr("Weapon Pickup", disambig)));
         parent->appendChild(new OptionToggleModelItem(411, tr("Ability Usage", disambig)));
      }
      {
         auto parent = new OptionToggleModelItem(OptionToggleModelItem::container_autohide, tr("Movement", disambig));
         traits->appendChild(parent);
         //
         parent->appendChild(new OptionToggleModelItem(413, tr("Movement Speed", disambig)));
         parent->appendChild(new OptionToggleModelItem(414, tr("Player Gravity", disambig)));
         parent->appendChild(new OptionToggleModelItem(415, tr("Vehicle Use", disambig)));
         parent->appendChild(new OptionToggleModelItem(416, tr("Jump Height", disambig)));
      }
      {
         auto parent = new OptionToggleModelItem(OptionToggleModelItem::container_autohide, tr("Appearance", disambig));
         traits->appendChild(parent);
         //
         parent->appendChild(new OptionToggleModelItem(422, tr("Active Camo", disambig)));
         parent->appendChild(new OptionToggleModelItem(421, tr("Visible Waypoint", disambig)));
         parent->appendChild(new OptionToggleModelItem(425, tr("Visible Name", disambig)));
         parent->appendChild(new OptionToggleModelItem(424, tr("Forced Color", disambig)));
      }
      {
         auto parent = new OptionToggleModelItem(OptionToggleModelItem::container_autohide, tr("Sensors", disambig));
         traits->appendChild(parent);
         //
         parent->appendChild(new OptionToggleModelItem(418, tr("Motion Tracker Mode",  disambig)));
         parent->appendChild(new OptionToggleModelItem(419, tr("Motion Tracker Range", disambig)));
      }
   }
   model_type::item_type* unknownRoot = nullptr;
   for (uint16_t i = 0; i < model_type::bitset_type::flag_count; i++) {
      if (model->hasOptionIndex(i))
         continue;
      if (!unknownRoot) {
         unknownRoot = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Unknown Toggles", disambig));
         model->invisibleRootItem()->appendChild(unknownRoot);
      }
      unknownRoot->appendChild(new OptionToggleModelItem(i, tr("Unknown #%1", disambig).arg(i)));
   }
}
#pragma endregion

#pragma region MegaloOptionToggleTree - the tree view
MegaloOptionToggleTree::MegaloOptionToggleTree(QWidget* parent) : OptionToggleTree(parent) {
   this->setModel(new MegaloOptionToggleModel);
   {
      auto* header = this->header();
      if (header) {
         header->setStretchLastSection(false);
         header->setSectionResizeMode(0, QHeaderView::Stretch);
         header->setSectionResizeMode(1, QHeaderView::ResizeToContents); // doing this in the superclass asserts; there has to already be a model which defines multiple columns
         header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
         header->setSectionsMovable(false);
      }
   }
}
void MegaloOptionToggleTree::updateModelFromGameVariant() {
   auto model = dynamic_cast<MegaloOptionToggleModel*>(this->model());
   if (!model)
      return;
   model->clear();
   auto data = ReachEditorState::get().multiplayerData();
   if (!data)
      return;
   auto& options = data->scriptData.options;
   auto  root    = model->invisibleRootItem();
   for (uint32_t i = 0; i < options.size(); i++) {
      auto& option = options[i];
      QString name;
      if (option.name) {
         name = QString::fromUtf8(option.name->get_content(reach::language::english).c_str());
      } else {
         name = QString(tr("Unnamed Option #%1")).arg(i);
      }
      root->appendChild(new OptionToggleModelItem(i, name));
   }
}
#pragma endregion
