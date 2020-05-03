#include "page_forge_labels.h"
#include "../../helpers/bitwise.h"
#include <QMessageBox>

ScriptEditorPageForgeLabels::ScriptEditorPageForgeLabels(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   this->ui.name->setAllowNoString(true);
   this->ui.name->setLimitedToSingleLanguageStrings(true);
   //
   auto& editor = ReachEditorState::get();
   //
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &ScriptEditorPageForgeLabels::updateListFromVariant);
   QObject::connect(this->ui.list, &QListWidget::currentRowChanged, this, &ScriptEditorPageForgeLabels::selectLabel);
   QObject::connect(this->ui.name, &ReachStringPicker::selectedStringChanged, [this]() {
      this->updateListFromVariant();
   });
   //
   QObject::connect(this->ui.buttonAdd, &QPushButton::clicked, [this]() {
      auto  mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& list  = mp->scriptContent.forgeLabels;
      auto& label = *list.emplace_back();
      label.is_defined = true;
      this->updateListFromVariant(ReachEditorState::get().variant());
      this->selectLabel(label.index);
   });
   QObject::connect(this->ui.buttonMoveUp, &QPushButton::clicked, [this]() {
      auto index = this->currentForgeLabel;
      if (index < 1) // can't move the last item up
         return;
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& list = mp->scriptContent.forgeLabels;
      auto  size = list.size();
      if (index >= size)
         return;
      list.swap_items(index, index - 1);
      --this->currentForgeLabel;
      const QSignalBlocker blocker(this->ui.list);
      this->ui.list->setCurrentRow(this->currentForgeLabel);
      this->updateListFromVariant();
   });
   QObject::connect(this->ui.buttonMoveDown, &QPushButton::clicked, [this]() {
      auto index = this->currentForgeLabel;
      if (index < 0)
         return;
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& list = mp->scriptContent.forgeLabels;
      auto  size = list.size();
      if (index >= size - 1) // can't move the last item down
         return;
      list.swap_items(index, index + 1);
      ++this->currentForgeLabel;
      const QSignalBlocker blocker(this->ui.list);
      this->ui.list->setCurrentRow(this->currentForgeLabel);
      this->updateListFromVariant();
   });
   QObject::connect(this->ui.buttonDelete, &QPushButton::clicked, [this]() {
      auto label = this->getLabel();
      if (!label)
         return;
      if (label->get_refcount()) {
         QMessageBox::information(this, tr("Cannot remove Forge label"), tr("This Forge label is still in use by the gametype script. It cannot be removed at this time."));
         return;
      }
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& list  = mp->scriptContent.forgeLabels;
      auto  index = list.index_of(label);
      if (index < 0)
         return;
      mp->scriptContent.forgeLabels.erase(index);
      this->ui.name->clearTarget();
      this->updateListFromVariant();
   });
   //
   QObject::connect(this->ui.reqFlagTeam, &QCheckBox::stateChanged, [this](int state) {
      if (auto label = this->getLabel()) {
         cobb::modify_bit(label->requirements, Megalo::ReachForgeLabel::requirement_flags::assigned_team, state == Qt::CheckState::Checked);
         this->ui.reqTeam->setDisabled(!label->requires_assigned_team());
      }
   });
   QObject::connect(this->ui.reqFlagObjectType, &QCheckBox::stateChanged, [this](int state) {
      if (auto label = this->getLabel()) {
         cobb::modify_bit(label->requirements, Megalo::ReachForgeLabel::requirement_flags::objects_of_type, state == Qt::CheckState::Checked);
         this->ui.reqObjectType->setDisabled(!label->requires_object_type());
      }
   });
   QObject::connect(this->ui.reqFlagNumber, &QCheckBox::stateChanged, [this](int state) {
      if (auto label = this->getLabel()) {
         cobb::modify_bit(label->requirements, Megalo::ReachForgeLabel::requirement_flags::number, state == Qt::CheckState::Checked);
         this->ui.reqNumber->setDisabled(!label->requires_number());
      }
   });
   QObject::connect(this->ui.reqTeam, &MegaloConstTeamIndexCombobox::currentDataChanged, [this](Megalo::const_team t) {
      if (auto label = this->getLabel())
         label->requiredTeam = t;
   });
   QObject::connect(this->ui.reqObjectType, &MPObjectTypeCombobox::currentDataChanged, [this](uint32_t objectType) {
      if (auto label = this->getLabel())
         label->requiredObjectType = objectType;
   });
   QObject::connect(this->ui.reqNumber, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
      if (auto label = this->getLabel())
         label->requiredNumber = value;
   });
   QObject::connect(this->ui.minCount, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
      if (auto label = this->getLabel())
         label->mapMustHaveAtLeast = value;
   });
   //
   this->updateListFromVariant(nullptr);
}
void ScriptEditorPageForgeLabels::selectLabel(int32_t labelIndex) {
   this->currentForgeLabel = labelIndex;
   //
   const QSignalBlocker blocker(this->ui.list);
   this->ui.list->setCurrentRow(this->currentForgeLabel);
   //
   this->updateLabelFromVariant();
}
Megalo::ReachForgeLabel* ScriptEditorPageForgeLabels::getLabel() const noexcept {
   if (this->currentForgeLabel < 0)
      return nullptr;
   auto mp = ReachEditorState::get().multiplayerData();
   if (!mp)
      return nullptr;
   auto& list = mp->scriptContent.forgeLabels;
   if (this->currentForgeLabel >= list.size())
      return nullptr;
   return &list[this->currentForgeLabel];
}
void ScriptEditorPageForgeLabels::updateListFromVariant(GameVariant* variant) {
   if (!variant) {
      variant = ReachEditorState::get().variant();
      if (!variant)
         return;
   }
   auto mp = variant->get_multiplayer_data();
   if (!mp)
      return;
   auto  container = this->ui.list;
   const QSignalBlocker blocker(container);
   auto& list = mp->scriptContent.forgeLabels;
   int32_t sel = container->currentRow();
   container->clear();
   for (size_t i = 0; i < list.size(); i++) {
      auto  item  = new QListWidgetItem;
      auto& label = list[i];
      if (label.name) {
         item->setText(QString::fromUtf8(label.name->english().c_str()));
      } else {
         item->setText(tr("<unnamed label %1>").arg(i));
      }
      item->setData(Qt::ItemDataRole::UserRole, i);
      container->addItem(item);
   }
   container->setCurrentRow(sel);
}
void ScriptEditorPageForgeLabels::updateLabelFromVariant(GameVariant* variant) {
   this->ui.name->clearTarget();
   if (!variant) {
      variant = ReachEditorState::get().variant();
      if (!variant)
         return;
   }
   auto mp = variant->get_multiplayer_data();
   if (!mp)
      return;
   auto& labels = mp->scriptContent.forgeLabels;
   if (this->currentForgeLabel >= labels.size())
      return;
   auto& label = labels[this->currentForgeLabel];
   //
   const QSignalBlocker blocker1(this->ui.reqFlagNumber);
   const QSignalBlocker blocker2(this->ui.reqFlagObjectType);
   const QSignalBlocker blocker3(this->ui.reqFlagTeam);
   const QSignalBlocker blocker4(this->ui.reqNumber);
   const QSignalBlocker blocker5(this->ui.reqObjectType);
   const QSignalBlocker blocker6(this->ui.reqTeam);
   const QSignalBlocker blocker7(this->ui.minCount);
   this->ui.reqFlagNumber->setChecked(label.requires_number());
   this->ui.reqFlagObjectType->setChecked(label.requires_object_type());
   this->ui.reqFlagTeam->setChecked(label.requires_assigned_team());
   this->ui.reqNumber->setValue(label.requiredNumber);
   this->ui.reqObjectType->setValue(label.requiredObjectType);
   this->ui.reqTeam->setValue(label.requiredTeam);
   this->ui.minCount->setValue(label.mapMustHaveAtLeast);
   //
   this->ui.reqNumber->setDisabled(!label.requires_number());
   this->ui.reqObjectType->setDisabled(!label.requires_object_type());
   this->ui.reqTeam->setDisabled(!label.requires_assigned_team());
   //
   {
      QList<QString> others;
      for (size_t i = 0; i < labels.size(); i++) {
         if (i == this->currentForgeLabel)
            continue;
         auto& other = labels[i];
         if (!other.name)
            continue;
         others.push_back(QString::fromUtf8(other.name->english().c_str()));
      }
      this->ui.name->setBlacklistedStrings(std::move(others));
   }
   this->ui.name->setTarget(label.name);
}