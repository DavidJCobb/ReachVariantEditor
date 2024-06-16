#include "option_toggles.h"
#include <QHeaderView>
#include <QMenu>
#include "../../editor_state.h"
#include "../../game_variants/types/multiplayer.h"

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
   assert(child != this);
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
   //  - Toggles [1092, 1272] are loadout options except for 1182, located exactly between 
   //    the Spartan and Elite loadouts, which doesn't correspond to anything known.
   //
   //     - Each loadout has six flags. That's 30 flags per loadout palette, for a total 
   //       of 180 flags for all loadouts.
   // 
   // In MegaloEdit, you define whether options are hidden or locked at the same time 
   // that you set their values. As such, `c_game_engine_custom_variant.ReadOverrideOption` 
   // does double-duty, pulling the specified value from your script AND returning the ID 
   // of the option -- that ID being used as the index in the option toggle bitsets. However, 
   // the function doesn't return an ID for every option (e.g. things inside of traits, 
   // loadouts, or other struct-like options).
   //

   static constexpr const char* disambig = "engine option toggles";
   auto model = static_cast<OptionToggleModel*>(this->model());

   constexpr size_t ids_per_player_trait_tree = 35;
   //
   auto _populate_player_trait_tree = [](OptionToggleModelItem& traits, size_t base_id, bool skip_in_map_and_game = false) {
      {
         auto parent = new OptionToggleModelItem(OptionToggleModelItem::container_autohide, tr("Defense", disambig));
         traits.appendChild(parent);
         //
         parent->appendChild(new OptionToggleModelItem(base_id + 0, tr("Damage Resistance", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 1, tr("Health Multiplier", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 2, tr("Health Recharge Rate", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 3, tr("Shield Multiplier", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 4, tr("Shield Recharge Rate", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 5, tr("Overshield Recharge Rate", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 6, tr("Headshot Immunity", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 7, tr("Assassination Immunity", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 8, tr("Cannot Die from Damage", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 9, tr("Shield Vampirism", disambig)));
      }
      {
         auto parent = new OptionToggleModelItem(OptionToggleModelItem::container_autohide, tr("Offense", disambig));
         traits.appendChild(parent);
         //
         parent->appendChild(new OptionToggleModelItem(base_id + 10, tr("Damage Multiplier", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 11, tr("Melee Multiplier", disambig)));
         if (!skip_in_map_and_game) {
            //
            // These two traits are part of Base Player Traits, but they're the only Base Player Traits that 
            // have working toggles. When we generate the tree for the unused/non-functional B.P.T., we'll 
            // want to exclude these, since they, ah, aren't unused.
            //
            parent->appendChild(new OptionToggleModelItem(base_id + 12, tr("Primary Weapon", disambig)));
            parent->appendChild(new OptionToggleModelItem(base_id + 13, tr("Secondary Weapon", disambig)));
         }
         parent->appendChild(new OptionToggleModelItem(base_id + 14, tr("Armor Ability", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 15, tr("Grenade Count", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 16, tr("Grenade Regeneration", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 19, tr("Weapon Pickup", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 17, tr("Infinite Ammo", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 20, tr("Ability Usage", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 18, tr("Infinite Abilities", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 21, tr("Drop Armor Ability on Death", disambig)));
      }
      {
         auto parent = new OptionToggleModelItem(OptionToggleModelItem::container_autohide, tr("Movement", disambig));
         traits.appendChild(parent);
         //
         parent->appendChild(new OptionToggleModelItem(base_id + 22, tr("Movement Speed", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 23, tr("Player Gravity", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 24, tr("Vehicle Use", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 25, tr("Jump Height", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 26, tr("Double Jump", disambig)));
      }
      {
         auto parent = new OptionToggleModelItem(OptionToggleModelItem::container_autohide, tr("Appearance", disambig));
         traits.appendChild(parent);
         //
         parent->appendChild(new OptionToggleModelItem(base_id + 31, tr("Active Camo", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 30, tr("Visible Waypoint", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 34, tr("Visible Name", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 33, tr("Forced Color", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 32, tr("Aura", disambig)));
      }
      {
         auto parent = new OptionToggleModelItem(OptionToggleModelItem::container_autohide, tr("Sensors", disambig));
         traits.appendChild(parent);
         //
         parent->appendChild(new OptionToggleModelItem(base_id + 27, tr("Motion Tracker Mode",  disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 28, tr("Motion Tracker Range", disambig)));
         parent->appendChild(new OptionToggleModelItem(base_id + 29, tr("Directional Damage Indicator", disambig)));
      }
   };

   constexpr size_t ids_per_ai_wave_trait_tree = 10;
   //
   auto _populate_ai_wave_trait_tree = [](OptionToggleModelItem& traits, size_t base_id) {
      traits.appendChild(new OptionToggleModelItem(base_id + 0, tr("Vision", disambig)));
      traits.appendChild(new OptionToggleModelItem(base_id + 1, tr("Hearing", disambig)));
      traits.appendChild(new OptionToggleModelItem(base_id + 2, tr("Luck", disambig)));
      traits.appendChild(new OptionToggleModelItem(base_id + 3, tr("Shootiness", disambig)));
      traits.appendChild(new OptionToggleModelItem(base_id + 4, tr("Grenades", disambig)));
      traits.appendChild(new OptionToggleModelItem(base_id + 5, tr("Drop Equipment on Death", disambig)));
      traits.appendChild(new OptionToggleModelItem(base_id + 6, tr("Assassination Immunity", disambig)));
      traits.appendChild(new OptionToggleModelItem(base_id + 7, tr("Headshot Immunity", disambig)));
      traits.appendChild(new OptionToggleModelItem(base_id + 8, tr("Damage Resistance", disambig)));
      traits.appendChild(new OptionToggleModelItem(base_id + 9, tr("Damage Modifier", disambig)));
   };

   auto _populate_respawn_options_tree = [](OptionToggleModelItem& parent, size_t base_id) {
      parent.appendChild(new OptionToggleModelItem(base_id +  0, tr("Synchronize with Team", disambig)));
      parent.appendChild(new OptionToggleModelItem(base_id +  1, tr("Respawn with Teammate", disambig)));
      parent.appendChild(new OptionToggleModelItem(base_id +  2, tr("Respawn at Location", disambig)));
      parent.appendChild(new OptionToggleModelItem(base_id +  3, tr("Respawn on Kills", disambig)));
      parent.appendChild(new OptionToggleModelItem(base_id +  4, tr("Lives per Round", disambig)));
      parent.appendChild(new OptionToggleModelItem(base_id +  5, tr("Team Lives per Round", disambig)));
      parent.appendChild(new OptionToggleModelItem(base_id +  6, tr("Respawn Time", disambig)));
      parent.appendChild(new OptionToggleModelItem(base_id +  7, tr("Suicide Penalty", disambig)));
      parent.appendChild(new OptionToggleModelItem(base_id +  8, tr("Betrayal Penalty", disambig)));
      parent.appendChild(new OptionToggleModelItem(base_id +  9, tr("Respawn Time Growth", disambig)));
      parent.appendChild(new OptionToggleModelItem(base_id + 10, tr("Loadout Camera Time", disambig)));
      parent.appendChild(new OptionToggleModelItem(base_id + 11, tr("Respawn Traits Duration", disambig)));
   };

   {
      auto parent = new OptionToggleModelItem(OptionToggleModelItem::container, tr("General Settings", disambig));
      model->invisibleRootItem()->appendChild(parent);
      //
      parent->appendChild(new OptionToggleModelItem(0, tr("Score to Win",  disambig)));
      parent->appendChild(new OptionToggleModelItem(1, tr("Teams Enabled", disambig)));
      parent->appendChild(new OptionToggleModelItem(2, tr("Reset Map on New Round", disambig)));
      parent->appendChild(new OptionToggleModelItem(3, tr("Round Time Limit", disambig)));
      parent->appendChild(new OptionToggleModelItem(4, tr("Sudden Death Time", disambig)));
      parent->appendChild(new OptionToggleModelItem(5, tr("Perfection Enabled", disambig)));
      parent->appendChild(new OptionToggleModelItem(6, tr("Number of Rounds", disambig)));
      parent->appendChild(new OptionToggleModelItem(7, tr("Rounds to Win", disambig)));
   }
   {
      auto parent = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Respawn Settings", disambig));
      model->invisibleRootItem()->appendChild(parent);
      _populate_respawn_options_tree(*parent, 8);
   }
   {
      auto parent = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Social Settings", disambig));
      model->invisibleRootItem()->appendChild(parent);
      //
      parent->appendChild(new OptionToggleModelItem(55, tr("Observers", disambig)));
      parent->appendChild(new OptionToggleModelItem(56, tr("Team Changing", disambig)));
      parent->appendChild(new OptionToggleModelItem(57, tr("Friendly Fire", disambig)));
      parent->appendChild(new OptionToggleModelItem(58, tr("Betrayal Booting", disambig)));
      parent->appendChild(new OptionToggleModelItem(59, tr("Proximity Voice", disambig)));
      parent->appendChild(new OptionToggleModelItem(60, tr("Don't Team-Restrict Voice Chat", disambig)));
      parent->appendChild(new OptionToggleModelItem(61, tr("Allow Dead Players to Talk", disambig)));
   }
   {
      auto parent = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Map and Game Settings", disambig));
      model->invisibleRootItem()->appendChild(parent);
      //
      parent->appendChild(new OptionToggleModelItem(62, tr("Grenades on Map", disambig)));
      parent->appendChild(new OptionToggleModelItem(63, tr("Abilities on Map", disambig)));
      parent->appendChild(new OptionToggleModelItem(64, tr("Turrets on Map", disambig)));
      parent->appendChild(new OptionToggleModelItem(65, tr("Shortcuts on Map", disambig)));
      parent->appendChild(new OptionToggleModelItem(66, tr("Powerups on Map", disambig)));
      parent->appendChild(new OptionToggleModelItem(67, tr("Indestructible Vehicles", disambig)));
      parent->appendChild(new OptionToggleModelItem(80, tr("Primary Weapon", disambig)));
      parent->appendChild(new OptionToggleModelItem(81, tr("Secondary Weapon", disambig)));
      parent->appendChild(new OptionToggleModelItem(103, tr("Weapon Set", disambig)));
      parent->appendChild(new OptionToggleModelItem(104, tr("Vehicle Set", disambig)));
      parent->appendChild(new OptionToggleModelItem(210, tr("Red Powerup Duration", disambig)));
      parent->appendChild(new OptionToggleModelItem(211, tr("Blue Powerup Duration", disambig)));
      parent->appendChild(new OptionToggleModelItem(212, tr("Custom Powerup Duration", disambig)));
   }
   {
      auto base = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Loadouts", disambig));
      model->invisibleRootItem()->appendChild(base);

      base->appendChild(new OptionToggleModelItem(1091, tr("Spartan Loadouts Enabled", disambig)));
      base->appendChild(new OptionToggleModelItem(1182, tr("Elite Loadouts Enabled", disambig)));

      constexpr uint8_t options_in_loadout = 6;
      constexpr uint8_t loadouts_in_palette = 5;
      constexpr uint8_t palettes_per_species = 3;
      //
      // 1091 == Spartan Loadouts Enabled
      // 1092 == Spartan Tier 1 Loadout 1 Hide Loadout
      // 1093 == Spartan Tier 1 Loadout 1 Name
      // 1094 == Spartan Tier 1 Loadout 1 Primary Weapon
      // 1181 == Spartan Tier 3 Loadout 5 Grenade Count
      // 1182 == Elite Loadouts Enabled
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
   }
   {
      auto* traits = new OptionToggleModelItem(OptionToggleModelItem::container, tr("All Player Traits", disambig));
      model->invisibleRootItem()->appendChild(traits);
      //
      _populate_player_trait_tree(*traits, 391);
   }
   {
      auto* forge = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Forge", disambig));
      model->invisibleRootItem()->appendChild(forge);

      forge->appendChild(new OptionToggleModelItem(213, tr("Enable Open-Channel Voice", disambig)));
      forge->appendChild(new OptionToggleModelItem(214, tr("Edit Mode Access", disambig)));
      forge->appendChild(new OptionToggleModelItem(215, tr("Edit Mode Respawn Time", disambig)));

      {
         auto* traits = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Editor Traits", disambig));
         forge->appendChild(traits);
         _populate_player_trait_tree(*traits, 216);
      }
   }

   {
      auto* unused_root = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Unused/Non-Functional Toggles", disambig));
      model->invisibleRootItem()->appendChild(unused_root);

      {
         auto* traits = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Base Player Traits", disambig));
         unused_root->appendChild(traits);
         _populate_player_trait_tree(*traits, 68, true);
      }
      {
         auto* traits = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Respawn Traits", disambig));
         unused_root->appendChild(traits);
         _populate_player_trait_tree(*traits, 20);
      }
      {
         auto* traits = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Red Powerup Traits", disambig));
         unused_root->appendChild(traits);
         _populate_player_trait_tree(*traits, 105);
      }
      {
         auto* traits = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Blue Powerup Traits", disambig));
         unused_root->appendChild(traits);
         _populate_player_trait_tree(*traits, 140);
      }
      {
         auto* traits = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Custom Powerup Traits", disambig));
         unused_root->appendChild(traits);
         _populate_player_trait_tree(*traits, 175);
      }
      {
         auto* folder = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Scripted Traits", disambig));
         unused_root->appendChild(folder);

         for (size_t i = 0; i < 4; ++i) {
            QString name = tr("Scripted Traits %1", disambig).arg(i);

            auto* traits = new OptionToggleModelItem(OptionToggleModelItem::container, name);
            folder->appendChild(traits);
            _populate_player_trait_tree(*traits, 251 + ids_per_player_trait_tree * i);
         }
      }
      
      {
         auto* mode_root = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Firefight (unused)", disambig));
         unused_root->appendChild(mode_root);
         //
         {
            auto* folder = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Firefight Settings", disambig));
            mode_root->appendChild(folder);
            //
            folder->appendChild(new OptionToggleModelItem(436, tr("Hazards", disambig)));
            folder->appendChild(new OptionToggleModelItem(437, tr("Weapon Drops", disambig)));
            folder->appendChild(new OptionToggleModelItem(438, tr("Ammo Crates", disambig)));
            folder->appendChild(new OptionToggleModelItem(450, tr("Generator Count", disambig)));
            folder->appendChild(new OptionToggleModelItem(439, tr("Generator Fail Condition", disambig)));
            folder->appendChild(new OptionToggleModelItem(440, tr("Generator Spawn Locations", disambig)));
            folder->appendChild(new OptionToggleModelItem(441, tr("Difficulty", disambig)));
            folder->appendChild(new OptionToggleModelItem(442, tr("Wave Limit", disambig)));
            folder->appendChild(new OptionToggleModelItem(443, tr("Bonus Lives (unused?)", disambig)));
            folder->appendChild(new OptionToggleModelItem(444, tr("Bonus Target Score (unused)", disambig)));
         }
         {
            auto* folder = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Player Lives", disambig));
            mode_root->appendChild(folder);
            //
            folder->appendChild(new OptionToggleModelItem(447, tr("Spartan Starting Lives", disambig)));
            folder->appendChild(new OptionToggleModelItem(445, tr("Reward for Killing Elite Players", disambig)));
            folder->appendChild(new OptionToggleModelItem(449, tr("Max Spartan Extra Lives", disambig)));
            folder->appendChild(new OptionToggleModelItem(448, tr("Elite Starting Lives", disambig)));
            folder->appendChild(new OptionToggleModelItem(446, tr("Extra Life Score Target (unused)", disambig)));
         }
         {
            auto* traits = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Base Spartan Traits", disambig));
            mode_root->appendChild(traits);
            _populate_player_trait_tree(*traits, 498);
         }
         {
            auto* traits = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Base Elite Traits", disambig));
            mode_root->appendChild(traits);
            _populate_player_trait_tree(*traits, 533);
         }
         {
            auto* traits = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Base Wave Traits", disambig));
            mode_root->appendChild(traits);
            _populate_ai_wave_trait_tree(*traits, 568);
         }
         {
            auto* folder = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Elite Respawn Settings", disambig));
            mode_root->appendChild(folder);
            _populate_respawn_options_tree(*folder, 451);

            {
               auto* traits = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Elite Respawn Traits", disambig));
               folder->appendChild(traits);
               _populate_player_trait_tree(*traits, 463);
            }
         }
         {
            constexpr size_t base_id = 578;

            constexpr size_t skulls_per = 18;
            constexpr size_t waves_per  = 3;
            constexpr size_t squads_per_wave = 12;

            constexpr size_t ids_per_wave  = 2 + squads_per_wave;
            constexpr size_t ids_per_round = skulls_per + waves_per * ids_per_wave;

            constexpr size_t number_of_rounds = 3;

            auto _append_skulls = [](OptionToggleModelItem& folder, size_t start) {
               //
               // Skulls are in the same order as in the game variant file format.
               //
               folder.appendChild(new OptionToggleModelItem(start + 0, tr("Skull: Iron", disambig)));
               folder.appendChild(new OptionToggleModelItem(start + 1, tr("Skull: Black Eye", disambig)));
               folder.appendChild(new OptionToggleModelItem(start + 2, tr("Skull: Catch", disambig)));
               folder.appendChild(new OptionToggleModelItem(start + 3, tr("Skull: Fog", disambig)));
               folder.appendChild(new OptionToggleModelItem(start + 4, tr("Skull: Famine", disambig)));
               folder.appendChild(new OptionToggleModelItem(start + 5, tr("Skull: Thunderstorm", disambig)));
               folder.appendChild(new OptionToggleModelItem(start + 6, tr("Skull: Tilt", disambig)));
               folder.appendChild(new OptionToggleModelItem(start + 7, tr("Skull: Mythic", disambig)));
               folder.appendChild(new OptionToggleModelItem(start + 8, tr("Skull: Assassin", disambig)));
               folder.appendChild(new OptionToggleModelItem(start + 9, tr("Skull: Blind", disambig)));
               folder.appendChild(new OptionToggleModelItem(start + 10, tr("Skull: Cowbell", disambig)));
               folder.appendChild(new OptionToggleModelItem(start + 11, tr("Skull: Grunt Birthday Party", disambig)));
               folder.appendChild(new OptionToggleModelItem(start + 12, tr("Skull: IWHBYD", disambig)));
               folder.appendChild(new OptionToggleModelItem(start + 13, tr("Skull: Red", disambig)));
               folder.appendChild(new OptionToggleModelItem(start + 14, tr("Skull: Yellow", disambig)));
               folder.appendChild(new OptionToggleModelItem(start + 15, tr("Skull: Blue", disambig)));
               folder.appendChild(new OptionToggleModelItem(start + 16, tr("Skull 17 (unused)", disambig)));
               folder.appendChild(new OptionToggleModelItem(start + 17, tr("Skull 18 (unused)", disambig)));
            };

            for (size_t i = 0; i < number_of_rounds; ++i) {
               auto* round = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Round %1", disambig).arg(i + 1));
               mode_root->appendChild(round);

               size_t start = base_id + ids_per_round * i;
               {
                  auto* folder = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Skulls", disambig));
                  round->appendChild(folder);
                  _append_skulls(*folder, start);
               }

               auto _make_wave = [start](OptionToggleModelItem* folder, size_t nth, size_t vanilla_wave_count) {
                  size_t wave_start = start + skulls_per + (nth * ids_per_wave);

                  folder->appendChild(new OptionToggleModelItem(wave_start + 0, tr("Uses Dropships", disambig)));
                  folder->appendChild(new OptionToggleModelItem(wave_start + 1, tr("Squad Order", disambig)));
                  //
                  for (size_t j = 0; j < squads_per_wave; ++j) {
                     QString text;
                     if (j < vanilla_wave_count) {
                        text = tr("Squad Type %1", disambig).arg(j + 1);
                     } else {
                        text = tr("Squad Type %1 (unused)", disambig).arg(j + 1);
                     }
                     folder->appendChild(new OptionToggleModelItem(wave_start + 2 + j, tr("Squad Order", disambig)));
                  }
               };
               //
               {
                  auto* folder = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Initial Wave", disambig));
                  round->appendChild(folder);
                  //
                  _make_wave(folder, 0, 3);
               }
               {
                  auto* folder = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Main Wave", disambig));
                  round->appendChild(folder);
                  //
                  _make_wave(folder, 1, 5);
               }
               {
                  auto* folder = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Boss Wave", disambig));
                  round->appendChild(folder);
                  //
                  _make_wave(folder, 2, 3);
               }
            }
            {
               auto* folder = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Bonus Wave", disambig));
               mode_root->appendChild(folder);

               constexpr size_t wave_start = 758;

               folder->appendChild(new OptionToggleModelItem(wave_start + 0, tr("Duration", disambig)));
               _append_skulls(*folder, wave_start + 1);
               folder->appendChild(new OptionToggleModelItem(wave_start + 1 + skulls_per, tr("Uses Dropships", disambig)));
               folder->appendChild(new OptionToggleModelItem(wave_start + 1 + skulls_per + 1, tr("Squad Order", disambig)));
               //
               for (size_t j = 0; j < squads_per_wave; ++j) {
                  QString text;
                  if (j < 3) {
                     text = tr("Squad Type %1", disambig).arg(j + 1);
                  } else {
                     text = tr("Squad Type %1 (unused)", disambig).arg(j + 1);
                  }
                  folder->appendChild(new OptionToggleModelItem(wave_start + 1 + skulls_per + 2 + j, text));
               }
            }
         }
         {
            size_t ids_per_skull = ids_per_player_trait_tree * 2 + ids_per_ai_wave_trait_tree;

            auto _populate_custom_skull_traits = [&_populate_player_trait_tree, &_populate_ai_wave_trait_tree](OptionToggleModelItem& skull, size_t base_id) {
               {
                  auto* traits = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Spartan Traits", disambig));
                  skull.appendChild(traits);
                  _populate_player_trait_tree(*traits, base_id);
               }
               {
                  auto* traits = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Elite Traits", disambig));
                  skull.appendChild(traits);
                  _populate_player_trait_tree(*traits, base_id + ids_per_player_trait_tree);
               }
               {
                  auto* traits = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Wave Traits", disambig));
                  skull.appendChild(traits);
                  _populate_ai_wave_trait_tree(*traits, base_id + ids_per_player_trait_tree * 2);
               }
            };

            {
               auto* skull = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Red Skull", disambig));
               mode_root->appendChild(skull);
               _populate_custom_skull_traits(*skull, 791);
            }
            {
               auto* skull = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Blue Skull", disambig));
               mode_root->appendChild(skull);
               _populate_custom_skull_traits(*skull, 791 + ids_per_skull * 2);
            }
            {
               auto* skull = new OptionToggleModelItem(OptionToggleModelItem::container, tr("Yellow Skull", disambig));
               mode_root->appendChild(skull);
               _populate_custom_skull_traits(*skull, 791 + ids_per_skull);
            }
         }
         // "Round template" menu items
         {
            auto* traits = new OptionToggleModelItem(OptionToggleModelItem::container, tr("All Firefight Wave Traits", disambig));
            mode_root->appendChild(traits);
            _populate_ai_wave_trait_tree(*traits, 426);
         }
      }
   }

   #if _DEBUG
   {
      cobb::bitset<1272> seen;
      const auto* root = model->invisibleRootItem();

      auto find = [](this auto&& recurse, const OptionToggleModelItem& item, size_t desired) -> const OptionToggleModelItem* {
         auto index = item.index();
         if (index == desired)
            return& item;
         for (size_t i = 0; i < item.childCount(); ++i)
            if (const auto* child = item.child(i))
               if (auto* result = recurse(*child, desired))
                  return result;
         return nullptr;
      };
      
      auto traverse = [&seen, &find, root](this auto&& recurse, const OptionToggleModelItem& item) -> void {
         auto index = item.index();
         if (index >= 0 && index < seen.size()) {
            if (seen.test(index)) {
               const auto* the_other_one = find(*root, index);
               assert(!seen.test(index) && "Multiple options should not have the same ID!");
            }
            seen.set(index);
         }
         for (size_t i = 0; i < item.childCount(); ++i)
            if (const auto* child = item.child(i))
               recurse(*child);
      };
      if (auto* root = model->invisibleRootItem())
         traverse(*root);
   }
   #endif

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
