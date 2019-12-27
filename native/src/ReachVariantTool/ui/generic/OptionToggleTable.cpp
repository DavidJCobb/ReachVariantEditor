#include "OptionToggleTable.h"
#include "../../editor_state.h"

#pragma region OptionToggleTreeRowModel - The class for option toggle tree model rows
OptionToggleTreeRowModel* OptionToggleTreeRowModel::get_parent() const noexcept {
   if (this->parent == -1)
      return nullptr;
   return this->owner.rows[this->parent];
}
OptionToggleTreeRowModel* OptionToggleTreeRowModel::get_child(int index) const noexcept {
   uint32_t counter = 0;
   for (auto* row : this->owner.rows) {
      if (row->parent == this->index) {
         if (counter == index)
            return row;
         ++counter;
      }
   }
   return nullptr;
}
int OptionToggleTreeRowModel::get_child_count() const noexcept {
   uint32_t counter = 0;
   for (auto* row : this->owner.rows)
      if (row->parent == this->index)
         ++counter;
   return counter;
}
int OptionToggleTreeRowModel::get_index_in_parent() const noexcept {
   int32_t counter = 0;
   for (auto* row : this->owner.rows) {
      if (row->parent == this->parent) {
         if (row == this)
            return counter;
         ++counter;
      }
   }
   return -1;
}
#pragma endregion

//
//
//

#pragma region OptionToggleTreeModel - The abstract class for option toggle tree models
QModelIndex OptionToggleTreeModel::index(int row, int column, const QModelIndex& parent) const {
   if (parent.isValid()) {
      row_type* parentItem = static_cast<row_type*>(parent.internalPointer());
      auto child = parentItem->get_child(row);
      if (child)
         return createIndex(row, column, child);
      return QModelIndex();
   }
   if (row < 0 || row >= this->rows.size())
      return QModelIndex();
   return createIndex(row, column, this->rows[row]);
}
QModelIndex OptionToggleTreeModel::parent(const QModelIndex& index) const {
   if (!index.isValid())
      return QModelIndex();
   auto child  = static_cast<row_type*>(index.internalPointer());
   auto pIndex = child->parent;
   if (pIndex == -1 || pIndex >= this->rows.size())
      return QModelIndex();
   return createIndex(pIndex, 0, this->rows[pIndex]);
}
int OptionToggleTreeModel::rowCount(const QModelIndex& parent) const {
   if (parent.column() > 0)
      return 0;
   int16_t pIndex = -1;
   if (parent.isValid())
      pIndex = static_cast<row_type*>(parent.internalPointer())->index;
   uint32_t counter = 0;
   for (auto* row : this->rows)
      if (row->parent == pIndex)
         ++counter;
   return counter;
}
int OptionToggleTreeModel::columnCount(const QModelIndex& item) const {
   return 3;
}
Qt::ItemFlags OptionToggleTreeModel::flags(const QModelIndex& index) const {
   if (!index.isValid())
      return Qt::NoItemFlags;
   if (index.column() > 0)
      return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
QVariant OptionToggleTreeModel::data(const QModelIndex& index, int role) const {
   if (!index.isValid())
      return QVariant();
   auto item = static_cast<row_type*>(index.internalPointer());
   if (role == Qt::CheckStateRole) {
      switch (index.column()) {
         case 1: // disabled
            return this->checkDisabledFlag(item->index) ? Qt::Checked : Qt::Unchecked;
         case 2: // hidden
            return this->checkHiddenFlag(item->index) ? Qt::Checked : Qt::Unchecked;
      }
      return QVariant();
   }
   if (role != Qt::DisplayRole)
      return QVariant();
   if (index.column() == 0)
      return item->name;
   return QVariant();
}
bool OptionToggleTreeModel::setData(const QModelIndex& index, const QVariant& value, int role) {
   if (role != Qt::CheckStateRole || !index.isValid())
      return false;
   auto item   = static_cast<row_type*>(index.internalPointer());
   bool result = false;
   auto column = index.column();
   if (column == 0 || column > 2)
      return false;
   Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
   if (column == 1)
      this->modifyFlag(OptionToggleFlagType::disabled, item->index, state == Qt::Checked);
   else if (column == 2)
      this->modifyFlag(OptionToggleFlagType::hidden, item->index, state == Qt::Checked);
   emit dataChanged(index, index);
   return true;
}
QVariant OptionToggleTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
   if (role == Qt::DisplayRole) {
      switch (section) {
         case 0: return tr("Option Name");
         case 1: return tr("Disabled");
         case 2: return tr("Hidden");
      }
   }
   return QVariant();
}
void OptionToggleTreeModel::insertItem(uint16_t index, QString name, int16_t parent) {
   if (index < this->rows.size()) {
      if (this->rows[index])
         return;
   } else {
      this->rows.resize(index + 1);
   }
   auto row = new row_type(*this, index);
   row->name   = name;
   row->parent = parent;
   this->rows[index] = row;
}
void OptionToggleTreeModel::clear() {
   for (auto row : this->rows)
      delete row;
   this->rows.clear();
}
#pragma endregion

//
//
//

#pragma region MegaloOptionToggleTreeModel - The model for the Megalo options list; a subclass of OptionToggleTreeModel
MegaloOptionToggleTreeModel::bitset_type* MegaloOptionToggleTreeModel::_getToggles(OptionToggleFlagType type) const noexcept {
   auto variant = ReachEditorState::get().currentVariant;
   if (!variant)
      return nullptr;
   auto& flagset = variant->multiplayer.optionToggles.megalo;
   switch (type) {
      case OptionToggleFlagType::disabled: return &flagset.disabled;
      case OptionToggleFlagType::hidden:   return &flagset.hidden;
   }
   return nullptr;
}
bool MegaloOptionToggleTreeModel::checkDisabledFlag(uint16_t index) const noexcept {
   if (index >= bitset_type::flag_count)
      return false;
   auto variant = ReachEditorState::get().currentVariant;
   if (!variant)
      return false;
   return variant->multiplayer.optionToggles.megalo.disabled.bits.test(index);
}
bool MegaloOptionToggleTreeModel::checkHiddenFlag(uint16_t index) const noexcept {
   if (index >= bitset_type::flag_count)
      return false;
   auto variant = ReachEditorState::get().currentVariant;
   if (!variant)
      return false;
   return variant->multiplayer.optionToggles.megalo.hidden.bits.test(index);
}
void MegaloOptionToggleTreeModel::modifyFlag(OptionToggleFlagType type, uint16_t index, bool state) noexcept {
   auto toggles = this->_getToggles(type);
   if (!toggles)
      return;
   if (state)
      toggles->bits.set(index);
   else
      toggles->bits.reset(index);
}
void MegaloOptionToggleTreeModel::modifyAllFlagsOfType(OptionToggleFlagType type, bool state) noexcept {
   auto toggles = this->_getToggles(type);
   if (!toggles)
      return;
   if (state)
      toggles->bits.set_all();
   else
      toggles->bits.clear();
}
#pragma endregion

#pragma region EngineOptionToggleTreeModel - The model for the engine options tree; a subclass of OptionToggleTreeModel
EngineOptionToggleTreeModel::bitset_type* EngineOptionToggleTreeModel::_getToggles(OptionToggleFlagType type) const noexcept {
   auto variant = ReachEditorState::get().currentVariant;
   if (!variant)
      return nullptr;
   auto& flagset = variant->multiplayer.optionToggles.engine;
   switch (type) {
      case OptionToggleFlagType::disabled: return &flagset.disabled;
      case OptionToggleFlagType::hidden:   return &flagset.hidden;
   }
   return nullptr;
}
bool EngineOptionToggleTreeModel::checkDisabledFlag(uint16_t index) const noexcept {
   if (index >= bitset_type::flag_count)
      return false;
   auto variant = ReachEditorState::get().currentVariant;
   if (!variant)
      return false;
   return variant->multiplayer.optionToggles.engine.disabled.bits.test(index);
}
bool EngineOptionToggleTreeModel::checkHiddenFlag(uint16_t index) const noexcept {
   if (index >= bitset_type::flag_count)
      return false;
   auto variant = ReachEditorState::get().currentVariant;
   if (!variant)
      return false;
   return variant->multiplayer.optionToggles.engine.hidden.bits.test(index);
}
void EngineOptionToggleTreeModel::modifyFlag(OptionToggleFlagType type, uint16_t index, bool state) noexcept {
   auto toggles = this->_getToggles(type);
   if (!toggles)
      return;
   if (state)
      toggles->bits.set(index);
   else
      toggles->bits.reset(index);
}
void EngineOptionToggleTreeModel::modifyAllFlagsOfType(OptionToggleFlagType type, bool state) noexcept {
   auto toggles = this->_getToggles(type);
   if (!toggles)
      return;
   if (state)
      toggles->bits.set_all();
   else
      toggles->bits.clear();
}
#pragma endregion

//
//
//

#pragma region OptionToggleTree - base class for the tree view
OptionToggleTree::OptionToggleTree(QWidget* parent) : QTreeView(parent) {
   this->setSelectionMode(QAbstractItemView::ExtendedSelection);
   {  // HeaderContext menu items
      {  // Disabled
         this->actionDisableCheckAll = new QAction(tr("Check All",   "Option Toggles: Disabled"), this);
         this->actionDisableClearAll = new QAction(tr("Uncheck All", "Option Toggles: Disabled"), this);
         QObject::connect(this->actionDisableCheckAll, &QAction::triggered, [this]() {
            auto model = static_cast<OptionToggleTreeModel*>(this->model());
            model->modifyAllFlagsOfType(OptionToggleFlagType::disabled, true);
         });
         QObject::connect(this->actionDisableClearAll, &QAction::triggered, [this]() {
            auto model = static_cast<OptionToggleTreeModel*>(this->model());
            model->modifyAllFlagsOfType(OptionToggleFlagType::disabled, false);
         });
      }
      {  // Hidden
         this->actionHiddenCheckAll = new QAction(tr("Check All",   "Option Toggles: Hidden"), this);
         this->actionHiddenClearAll = new QAction(tr("Uncheck All", "Option Toggles: Hidden"), this);
         QObject::connect(this->actionHiddenCheckAll, &QAction::triggered, [this]() {
            auto model = static_cast<OptionToggleTreeModel*>(this->model());
            model->modifyAllFlagsOfType(OptionToggleFlagType::hidden, true);
         });
         QObject::connect(this->actionHiddenClearAll, &QAction::triggered, [this]() {
            auto model = static_cast<OptionToggleTreeModel*>(this->model());
            model->modifyAllFlagsOfType(OptionToggleFlagType::hidden, false);
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
   auto model = dynamic_cast<OptionToggleTreeModel*>(this->model());
   for (const auto& row : rows) {
      const auto* item = static_cast<OptionToggleTreeModel::row_type*>(row.internalPointer());
      if (!item)
         continue;
      model->modifyFlag(which, item->index, check);
   }
}
#pragma endregion

#pragma region EngineOptionToggleTree - the tree view
EngineOptionToggleTree::EngineOptionToggleTree(QWidget* parent) : OptionToggleTree(parent) {
   this->setModel(new EngineOptionToggleTreeModel);
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

   auto model = static_cast<OptionToggleTreeModel*>(this->model());
   model->insertItem(0, tr("Score to Win"));
   model->insertItem(1, tr("Teams Enabled"));
   // 2
   model->insertItem(3, tr("Time Limit"));
   model->insertItem(4, tr("Sudden Death"));
   // 5
   model->insertItem(6, tr("Number of Rounds"));
   // 7
   model->insertItem(8, tr("Respawn Options, Synchronize With Team"));
   // ...
   model->insertItem(12, tr("Respawn Options, Lives per Round"));
   // 13
   model->insertItem(14, tr("Respawn Options, Respawn Time"));
   model->insertItem(15, tr("Respawn Options, Suicide Penalty"));
   model->insertItem(16, tr("Respawn Options, Betrayal Penalty"));
   model->insertItem(17, tr("Respawn Options, Respawn Growth"));
   // 18
   model->insertItem(19, tr("Respawn Options, Respawn Traits Duration"));
   // ...
   model->insertItem(56, tr("Team Changing"));
   model->insertItem(57, tr("Friendly Fire"));
   model->insertItem(58, tr("Betrayal Booting"));
   // ...
   model->insertItem(62, tr("Grenades On Map"));
   model->insertItem(63, tr("Abilities On Map"));
   model->insertItem(64, tr("Turrets On Map"));
   // 65
   model->insertItem(66, tr("Powerups On Map"));
   model->insertItem(67, tr("Indestructible Vehicles"));
   // ...
   model->insertItem(80, tr("Map, Primary Weapon?"));
   model->insertItem(81, tr("Map, Secondary Weapon?"));
   // ...
   model->insertItem(103, tr("Weapon Set"));
   model->insertItem(104, tr("Vehicle Set"));
   // ...
   // 120
   // ...
   model->insertItem(393, tr("Player Traits, Health Recharge Rate"));
   model->insertItem(394, tr("Player Traits, Shield Multiplier"));
   model->insertItem(395, tr("Player Traits, Shield Recharge Rate"));
   // 396
   model->insertItem(397, tr("Player Traits, Headshot Immunity"));
   model->insertItem(398, tr("Player Traits, Assassination Immunity"));
   // 399
   model->insertItem(404, tr("Player Traits, Secondary Weapon"));
   model->insertItem(405, tr("Player Traits, Armor Ability"));
   model->insertItem(406, tr("Player Traits, Grenade Count"));
   // 407
   model->insertItem(408, tr("Player Traits, Infinite Ammo"));
   // 409
   model->insertItem(410, tr("Player Traits, Weapon Pickup"));
   model->insertItem(411, tr("Player Traits, Ability Usage"));
   // 412
   model->insertItem(413, tr("Player Traits, Movement Speed"));
   model->insertItem(414, tr("Player Traits, Player Gravity"));
   model->insertItem(415, tr("Player Traits, Vehicle Use"));
   model->insertItem(416, tr("Player Traits, Jump Height"));
   // 417
   model->insertItem(418, tr("Player Traits, Motion Tracker Mode"));
   model->insertItem(419, tr("Player Traits, Motion Tracker Range"));
   // 420
   model->insertItem(421, tr("Player Traits, Visible Waypoint"));
   model->insertItem(422, tr("Player Traits, Active Camo"));
   // 423
   model->insertItem(424, tr("Player Traits, Forced Color"));
   model->insertItem(425, tr("Player Traits, Visible Name"));
   // ...
   for (uint16_t i = 0; i < model_type::bitset_type::flag_count; i++) {
      model->insertItem(i, QString("Unknown #%1").arg(i));
   }
}
#pragma endregion

#pragma region MegaloOptionToggleTree - the tree view
MegaloOptionToggleTree::MegaloOptionToggleTree(QWidget* parent) : OptionToggleTree(parent) {
   this->setModel(new MegaloOptionToggleTreeModel);
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
   auto model = dynamic_cast<MegaloOptionToggleTreeModel*>(this->model());
   if (!model)
      return;
   model->clear();
   auto variant = ReachEditorState::get().currentVariant;
   if (!variant)
      return;
   auto& options = variant->multiplayer.scriptData.options;
   for (uint32_t i = 0; i < options.size(); i++) {
      auto& option = options[i];
      QString name;
      if (option.name) {
         name = QString::fromUtf8(option.name->english().c_str());
      } else {
         name = QString(tr("Unnamed Option #%1")).arg(i);
      }
      model->insertItem(i, name);
   }
}
#pragma endregion
