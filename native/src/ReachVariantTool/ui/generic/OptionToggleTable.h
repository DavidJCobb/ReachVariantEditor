#pragma once
#include <cstdint>
#include <QAction>
#include <QCheckBox>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QTreeView>
#include "../../formats/bitset.h"
#include "../../game_variants/base.h"

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

   Testing also indicates that we may have too few toggles. In all of 
   our test variants, Elite Loadouts, Tier 3, Loadout 5 Grenade Count 
   is visible -- we may be one bit short in our bitsets.

*/

//
// TODO: Create a model and treeview class for Megalo option toggles.
//

enum class OptionToggleFlagType {
   none = -1,
   disabled,
   hidden,
};

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
      virtual void modifyFlag(OptionToggleFlagType, uint16_t index, bool state) noexcept = 0;
      virtual void modifyAllFlagsOfType(OptionToggleFlagType, bool state) noexcept = 0;
      virtual void modifyFlagsInRange(OptionToggleFlagType, bool state, uint16_t start, uint16_t end) noexcept = 0;

      OptionToggleTreeModel(QObject* parent = nullptr) : QAbstractItemModel(parent) {}
      ~OptionToggleTreeModel() {
         qDeleteAll(this->rows);
      }
      QModelIndex index(int row, int column, const QModelIndex& parent) const override;
      QModelIndex parent(const QModelIndex& index) const;
      int rowCount(const QModelIndex& parent) const override;
      int columnCount(const QModelIndex& item) const override;
      Qt::ItemFlags flags(const QModelIndex& index) const override;
      QVariant data(const QModelIndex& index, int role) const override;
      bool setData(const QModelIndex& index, const QVariant& value, int role);
      //
      QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
      //
      void insertItem(uint16_t index, QString name, int16_t parent = -1);
      //
      QVector<OptionToggleTreeRowModel*> topLevelItems;
};
class EngineOptionToggleTreeModel : public OptionToggleTreeModel {
   public:
      using bitset_type = ReachGameVariantEngineOptionToggles;
      //
      bitset_type* _getToggles(OptionToggleFlagType type) const noexcept;
      //
      virtual bool checkDisabledFlag(uint16_t index) const noexcept override;
      virtual bool checkHiddenFlag(uint16_t index) const noexcept override;
      virtual void modifyFlag(OptionToggleFlagType, uint16_t index, bool state) noexcept override;
      virtual void modifyAllFlagsOfType(OptionToggleFlagType, bool state) noexcept override;
      virtual void modifyFlagsInRange(OptionToggleFlagType, bool state, uint16_t start, uint16_t end) noexcept override;
};

class EngineOptionToggleTree : public QTreeView {
   Q_OBJECT
   public:
      using model_type = EngineOptionToggleTreeModel;
      //
      EngineOptionToggleTree(QWidget* parent);
      //
      QAction* actionDisableCheckAll = nullptr;
      QAction* actionDisableClearAll = nullptr;
      QAction* actionHiddenCheckAll  = nullptr;
      QAction* actionHiddenClearAll  = nullptr;
      QAction* actionDisableCheckAllSelected = nullptr;
      QAction* actionDisableClearAllSelected = nullptr;
      QAction* actionHiddenCheckAllSelected  = nullptr;
      QAction* actionHiddenClearAllSelected  = nullptr;
      //
      void showHeaderContextMenu(const QPoint& screenPos, int column);
      //
      void showBodyContextMenu(const QPoint& screenPos);
      void doneBodyContextMenu(OptionToggleFlagType which, bool check);
};