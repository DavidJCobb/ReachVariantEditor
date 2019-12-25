#pragma once
#include <cstdint>
#include <QCheckBox>
#include <QHeaderView>
#include <QLabel>
#include <QTreeView>

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
            assert(!this->rows[index] && "Index already taken!");
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
         for (uint16_t i = 0; i < 1272; i++) {
            QString label;
            if (i == 1) {
               label = tr("Teams Enabled");
            } else {
               label = QString("Unknown #%1").arg(i);
            }
            model->insertItem(i, label);
         }
      }
};