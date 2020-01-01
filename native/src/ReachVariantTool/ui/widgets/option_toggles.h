#pragma once
#include <cstdint>
#include <vector>
#include <QAbstractItemModel>
#include <QAction>
#include <QString>
#include <QTreeView>
#include "../../formats/bitset.h"
#include "../../game_variants/base.h"

enum class OptionToggleFlagType {
   disabled,
   hidden,
};

class OptionToggleModel;
class OptionToggleModelItem {
   friend OptionToggleModel;
   protected:
      QString _name;
      int32_t _index = -1;
      OptionToggleModelItem* _parent = nullptr;
      std::vector<OptionToggleModelItem*> _children;
      //
      void _destroyDescendants() noexcept;
      //
   public:
      static constexpr int32_t container = -1;
      static constexpr int32_t container_autohide = -2;
      //
      OptionToggleModelItem(int32_t i, QString n) : _index(i), _name(n) {};
      //
      void appendChild(OptionToggleModelItem*) noexcept;
      inline bool autoHideIfAllChildrenHidden() const noexcept { return this->_index == container_autohide; }
      bool childrenContainOptionIndex(int32_t i) const noexcept;
      void removeChild(OptionToggleModelItem*) noexcept;
      inline const std::vector<OptionToggleModelItem*>& children() const noexcept { return this->_children; }
      inline OptionToggleModelItem* child(size_t i) const noexcept {
         if (i < 0 || i >= this->_children.size())
            return nullptr;
         return this->_children[i];
      }
      inline size_t childCount() const noexcept { return this->_children.size(); }
      inline int32_t index() const noexcept { return this->_index; }
      int32_t indexOf(OptionToggleModelItem*) const noexcept;
      inline bool isOption() const noexcept { return this->_index >= 0; }
      inline const QString& name() const noexcept { return this->_name; }
      inline OptionToggleModelItem* parent() const noexcept { return this->_parent; }
};
class OptionToggleModel : public QAbstractItemModel {
   Q_OBJECT
   public:
      using item_type = OptionToggleModelItem;
   protected:
      item_type* root = nullptr;
      //
   public:
      OptionToggleModel() {
         this->root = new item_type(-1, QString(""));
      }
      ~OptionToggleModel() {
         this->clear();
      }
      //
      virtual bool getFlag(const item_type*, OptionToggleFlagType) const noexcept = 0;
      virtual void modifyFlag(const item_type*, OptionToggleFlagType, bool state) const noexcept = 0;
      virtual void modifyFlagForAll(OptionToggleFlagType, bool state) const noexcept = 0;
      //
      bool hasOptionIndex(int32_t i) const noexcept;
      QModelIndex index(int row, int column, const QModelIndex& parent) const override;
      inline item_type* invisibleRootItem() const noexcept { return this->root; }
      QModelIndex parent(const QModelIndex& index) const;
      int rowCount(const QModelIndex& parent) const override;
      int columnCount(const QModelIndex& item) const override;
      Qt::ItemFlags flags(const QModelIndex& index) const override;
      QVariant data(const QModelIndex& index, int role) const override;
      bool setData(const QModelIndex& index, const QVariant& value, int role);
      //
      QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
      //
      void clear();
};
class EngineOptionToggleModel : public OptionToggleModel {
   public:
      using bitset_type = ReachGameVariantEngineOptionToggles;
      //
      bitset_type* _getToggles(OptionToggleFlagType type) const noexcept;
      //
      virtual bool getFlag(const item_type*, OptionToggleFlagType) const noexcept override;
      virtual void modifyFlag(const item_type*, OptionToggleFlagType, bool state) const noexcept override;
      virtual void modifyFlagForAll(OptionToggleFlagType, bool state) const noexcept override;
};
class MegaloOptionToggleModel : public OptionToggleModel {
   public:
      using bitset_type = ReachGameVariantMegaloOptionToggles;
      //
      bitset_type* _getToggles(OptionToggleFlagType type) const noexcept;
      //
      virtual bool getFlag(const item_type*, OptionToggleFlagType) const noexcept override;
      virtual void modifyFlag(const item_type*, OptionToggleFlagType, bool state) const noexcept override;
      virtual void modifyFlagForAll(OptionToggleFlagType, bool state) const noexcept override;
};

class OptionToggleTree : public QTreeView {
   Q_OBJECT
   public:
      OptionToggleTree(QWidget* parent);
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
class EngineOptionToggleTree : public OptionToggleTree {
   Q_OBJECT
   public:
      using model_type = EngineOptionToggleModel;
      //
      EngineOptionToggleTree(QWidget* parent);
};
class MegaloOptionToggleTree : public OptionToggleTree {
   Q_OBJECT
   public:
      using model_type = MegaloOptionToggleModel;
      //
      MegaloOptionToggleTree(QWidget* parent);
      void updateModelFromGameVariant();
};