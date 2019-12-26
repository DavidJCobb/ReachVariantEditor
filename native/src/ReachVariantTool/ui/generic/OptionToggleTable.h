#pragma once
#include <cstdint>
#include <QAction>
#include <QCheckBox>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QTreeView>

/*

   The model I'm using here is ugly as sin, but for now my sole interest is in 
   building something that works. If I have time, and if I manage to identify 
   a significant number of the option toggles, then I'll pretty it up later.

   The game stores option toggles as a flat list... so we do that as well. I've 
   added very rudimentary support for nesting -- any toggle can specify another 
   toggle's index as its "parent," and we identify parents, children, and so on 
   by just looping over the whole list and counting things up. The only reason 
   I've even added nesting support is because there are 1,272 engine option 
   toggles and I don't know what they all refer to -- whether it might be 
   possible to block off entire submenus, for example, in which case nesting 
   would be appropriate.

   Notes:

   Logical column index 0 is always the option name
   Logical column index 1 is always the "disabled" flag
   Logical column index 2 is always the "hidden" flag

   Testing indicates that all options can be hidden but important submenus cannot 
   (i.e. if you empty them out, then they're just blank when entered). As such, 
   here's our nesting order for all submenus that can't be hidden:

   All Options
      Megalo Player Traits
         ...
      General Settings
         Player Traits
            ...
         Spartan Loadouts
            Tier 1 Loadouts
               ...
            Tier 2 Loadouts
               ...
            Tier 3 Loadouts
               ...
         Elite Loadouts
            Tier 1 Loadouts
               ...
            Tier 2 Loadouts
               ...
            Tier 3 Loadouts
               ...
         Respawn Settings
            Advanced Respawn Settings
               ...
               Respawn Traits
                  ...
            ...
         ...
      Weapons and Vehicles
         ...
         Custom Powerup Traits
            ...
      Specific Game Options
         [Megalo options]

*/

//
// TODO: Create a model and treeview class for Megalo option toggles.
//

class OptionToggleTreeModel;
class OptionToggleTreeRowModel {
   public:
      OptionToggleTreeModel& owner;
      QString name;
      int16_t index    =  0;
      int16_t parent   = -1;
      //
      OptionToggleTreeRowModel(OptionToggleTreeModel& owner, int16_t i) : owner(owner), index(i) {};
      //
      OptionToggleTreeRowModel* get_parent() const noexcept;
      OptionToggleTreeRowModel* get_child(int index) const noexcept;
      int get_child_count() const noexcept;
      int get_index_in_parent() const noexcept;
};
class OptionToggleTreeModel : public QAbstractItemModel {
   Q_OBJECT
   public:
      using row_type = OptionToggleTreeRowModel;
      //
      QVector<row_type*> rows;

      virtual bool checkDisabledFlag(uint16_t index) const noexcept = 0;
      virtual bool checkHiddenFlag(uint16_t index) const noexcept = 0;
      virtual void modifyDisabledFlag(uint16_t index, bool state) noexcept = 0;
      virtual void modifyHiddenFlag(uint16_t index, bool state) noexcept = 0;
      virtual void modifyAllDisableFlags(bool disabled) noexcept = 0;
      virtual void modifyAllHiddenFlags(bool hidden) noexcept = 0;

      OptionToggleTreeModel(QObject* parent = nullptr) : QAbstractItemModel(parent) {}
      ~OptionToggleTreeModel() {
         qDeleteAll(this->rows);
      }
      QModelIndex index(int row, int column, const QModelIndex& parent) const override {
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
      QModelIndex parent(const QModelIndex& index) const {
         if (!index.isValid())
            return QModelIndex();
         auto child  = static_cast<row_type*>(index.internalPointer());
         auto pIndex = child->parent;
         if (pIndex == -1 || pIndex >= this->rows.size())
            return QModelIndex();
         return createIndex(pIndex, 0, this->rows[pIndex]);
      }
      int rowCount(const QModelIndex& parent) const override {
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
      int columnCount(const QModelIndex& item) const override {
         return 3;
      }
      Qt::ItemFlags flags(const QModelIndex& index) const override {
         if (!index.isValid())
            return Qt::NoItemFlags;
         if (index.column() > 0)
            return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
         return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
      }
      QVariant data(const QModelIndex& index, int role) const override {
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
      bool setData(const QModelIndex& index, const QVariant& value, int role) {
         if (role != Qt::CheckStateRole || !index.isValid())
            return false;
         auto item   = static_cast<row_type*>(index.internalPointer());
         bool result = false;
         auto column = index.column();
         if (column == 0 || column > 2)
            return false;
         Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
         if (column == 1)
            this->modifyDisabledFlag(item->index, state == Qt::Checked);
         else if (column == 2)
            this->modifyHiddenFlag(item->index, state == Qt::Checked);
         emit dataChanged(index, index);
         return true;
      }
      //
      QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
         if (role == Qt::DisplayRole) {
            switch (section) {
               case 0: return tr("Option Name");
               case 1: return tr("Disabled");
               case 2: return tr("Hidden");
            }
         }
         return QVariant();
      }
      //
      void insertItem(uint16_t index, QString name, int16_t parent = -1) {
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
      //
      QVector<OptionToggleTreeRowModel*> topLevelItems;
};
class EngineOptionToggleTreeModel : public OptionToggleTreeModel {
   public:
      virtual bool checkDisabledFlag(uint16_t index) const noexcept override;
      virtual bool checkHiddenFlag(uint16_t index) const noexcept override;
      virtual void modifyDisabledFlag(uint16_t index, bool state) noexcept override;
      virtual void modifyHiddenFlag(uint16_t index, bool state) noexcept override;
      virtual void modifyAllDisableFlags(bool disabled) noexcept override;
      virtual void modifyAllHiddenFlags(bool hidden) noexcept override;
};

class EngineOptionToggleTree : public QTreeView {
   Q_OBJECT
   public:
      EngineOptionToggleTree(QWidget* parent) : QTreeView(parent) {
         this->setModel(new EngineOptionToggleTreeModel);
         {
            auto* header = this->header();
            if (header) {
               header->setStretchLastSection(false);
               header->setSectionResizeMode(0, QHeaderView::Stretch);
               header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
               header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
               header->setSectionsMovable(false);
            }
         }
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
         model->insertItem(80, tr("Base Player Traits, Offense, Primary Weapon"));
         model->insertItem(81, tr("Base Player Traits, Offense, Secondary Weapon"));
         // ...
         model->insertItem(103, tr("Weapons On Map"));
         for (uint16_t i = 0; i < 1272; i++) {
            model->insertItem(i, QString("Unknown #%1").arg(i));
         }
         //
         {  // Context menu items
            {  // Disabled
               this->actionDisableCheckAll = new QAction(tr("Check All",   "Engine Option Toggles: Disabled"), this);
               this->actionDisableClearAll = new QAction(tr("Uncheck All", "Engine Option Toggles: Disabled"), this);
               QObject::connect(this->actionDisableCheckAll, &QAction::triggered, [this]() {
                  auto model = static_cast<OptionToggleTreeModel*>(this->model());
                  model->modifyAllDisableFlags(true);
               });
               QObject::connect(this->actionDisableClearAll, &QAction::triggered, [this]() {
                  auto model = static_cast<OptionToggleTreeModel*>(this->model());
                  model->modifyAllDisableFlags(false);
               });
            }
            {  // Hidden
               this->actionHiddenCheckAll = new QAction(tr("Check All",   "Engine Option Toggles: Hidden"), this);
               this->actionHiddenClearAll = new QAction(tr("Uncheck All", "Engine Option Toggles: Hidden"), this);
               QObject::connect(this->actionHiddenCheckAll, &QAction::triggered, [this]() {
                  auto model = static_cast<OptionToggleTreeModel*>(this->model());
                  model->modifyAllHiddenFlags(true);
               });
               QObject::connect(this->actionHiddenClearAll, &QAction::triggered, [this]() {
                  auto model = static_cast<OptionToggleTreeModel*>(this->model());
                  model->modifyAllHiddenFlags(false);
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
      }
      //
      QAction* actionDisableCheckAll = nullptr;
      QAction* actionDisableClearAll = nullptr;
      QAction* actionHiddenCheckAll  = nullptr;
      QAction* actionHiddenClearAll  = nullptr;
      //
      void showHeaderContextMenu(const QPoint& pos, int column) {
         QMenu menu(this);
         if (column == 1) {
            menu.addAction(this->actionDisableCheckAll);
            menu.addAction(this->actionDisableClearAll);
         } else if (column == 2) {
            menu.addAction(this->actionHiddenCheckAll);
            menu.addAction(this->actionHiddenClearAll);
         } else {
            return;
         }
         menu.exec(pos);
      }
};