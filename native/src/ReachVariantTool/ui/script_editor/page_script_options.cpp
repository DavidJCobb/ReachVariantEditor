#include "page_script_options.h"

namespace {
   bool _selectByPointerData(QListWidget* widget, void* target) {
      auto count = widget->count();
      for (size_t row = 0; row < count; row++) {
         auto item = widget->item(row);
         if (!item)
            continue;
         auto data = item->data(Qt::ItemDataRole::UserRole);
         if (!data.isValid())
            continue;
         auto ptr = data.value<void*>();
         if (ptr == target) {
            widget->setCurrentItem(item);
            return true;
         }
      }
      return false; // not found
   }
}
ScriptEditorPageScriptOptions::ScriptEditorPageScriptOptions(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   //
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &ScriptEditorPageScriptOptions::updateOptionsListFromVariant);
   QObject::connect(&editor, &ReachEditorState::variantAbandoned, [this]() {
      this->targetOption = nullptr;
      this->targetValue  = nullptr;
      this->updateValuesListFromVariant();
   });
   QObject::connect(this->ui.listOptions, &QListWidget::currentRowChanged, this, &ScriptEditorPageScriptOptions::selectOption);
   QObject::connect(this->ui.listValues,  &QListWidget::currentRowChanged, this, &ScriptEditorPageScriptOptions::selectOptionValue);
   //
   QObject::connect(this->ui.optionType, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
      if (!this->targetOption)
         return;
      if (index == 1)
         this->targetOption->make_range();
      this->updateOptionFromVariant();
   });
   //
   // TODO: hooks for all other controls having their states changed
   //
   //
   this->updateOptionsListFromVariant(nullptr);
}

void ScriptEditorPageScriptOptions::selectOption(int32_t i) {
   if (i < 0) {
      this->targetOption = nullptr;
   } else {
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& list = mp->scriptData.options;
      if (i >= list.size())
         return;
      this->targetOption = list[i];
   }
   this->targetValue = nullptr;
   this->updateOptionFromVariant();
   this->updateValueFromVariant();
   this->updateValuesListFromVariant();
}
void ScriptEditorPageScriptOptions::selectOptionValue(int32_t i) {
   this->targetValue = nullptr;
   if (auto o = this->targetOption) {
      if (!o->isRange) {
         auto& list = o->values;
         if (i >= 0 && i < list.size())
            this->targetValue = list[i];
      }
   }
   this->updateValueFromVariant();
}

void ScriptEditorPageScriptOptions::updateOptionsListFromVariant(GameVariant* variant) {
   this->ui.listOptions->clear();
   //
   GameVariantDataMultiplayer* mp = nullptr;
   if (variant)
      mp = variant->get_multiplayer_data();
   else
      mp = ReachEditorState::get().multiplayerData();
   if (!mp)
      return;
   auto& list = mp->scriptData.options;
   for (size_t i = 0; i < list.size(); i++) {
      auto& option = *list[i];
      auto  item   = new QListWidgetItem;
      if (option.name)
         item->setText(QString::fromUtf8(option.name->english().c_str()));
      else
         item->setText(tr("<unnamed option %1>", "scripted option editor").arg(i));
      item->setData(Qt::ItemDataRole::UserRole, QVariant::fromValue((void*)&option));
      this->ui.listOptions->addItem(item);
   }
   if (this->targetOption)
      _selectByPointerData(this->ui.listOptions, this->targetOption);
}
void ScriptEditorPageScriptOptions::updateOptionFromVariant() {
   const QSignalBlocker blocker0(this->ui.rangeDefault);
   const QSignalBlocker blocker1(this->ui.rangeMin);
   const QSignalBlocker blocker2(this->ui.rangeMax);
   const QSignalBlocker blocker3(this->ui.optionName);
   const QSignalBlocker blocker4(this->ui.optionDesc);
   const QSignalBlocker blocker5(this->ui.optionType);
   //
   if (!this->targetOption) {
      //
      // TOOD: reset controls to a blank state
      //
      return;
   }
   auto& o = *this->targetOption;
   this->ui.optionName->setTarget(o.name);
   this->ui.optionDesc->setTarget(o.desc);
   this->ui.optionType->setCurrentIndex(o.isRange ? 1 : 0);
   if (o.rangeMin)
      this->ui.rangeMin->setValue(o.rangeMin->value);
   if (o.rangeMax)
      this->ui.rangeMax->setValue(o.rangeMax->value);
   if (o.rangeDefault)
      this->ui.rangeDefault->setValue(o.rangeDefault->value);
   //
   if (o.isRange) {
      this->ui.stack->setCurrentWidget(this->ui.pageRange);
   } else {
      this->ui.stack->setCurrentWidget(this->ui.pageEnum);
   }
}
void ScriptEditorPageScriptOptions::updateValuesListFromVariant() {
   this->ui.listValues->clear();
   if (!this->targetOption)
      return;
   auto& list = this->targetOption->values;
   for (size_t i = 0; i < list.size(); i++) {
      auto& value = *list[i];
      auto  item  = new QListWidgetItem;
      if (value.name)
         item->setText(QString::fromUtf8(value.name->english().c_str()));
      else
         item->setText(tr("<unnamed value %1>", "scripted option editor").arg(i));
      item->setData(Qt::ItemDataRole::UserRole, QVariant::fromValue((void*)&value));
      this->ui.listValues->addItem(item);
   }
   if (this->targetValue)
      _selectByPointerData(this->ui.listValues, this->targetValue);
}
void ScriptEditorPageScriptOptions::updateValueFromVariant() {
   const QSignalBlocker blocker0(this->ui.valueName);
   const QSignalBlocker blocker1(this->ui.valueDesc);
   const QSignalBlocker blocker2(this->ui.valueValue);
   if (!this->targetValue) {
      //
      // TOOD: reset controls to a blank state
      //
      return;
   }
   this->ui.valueName->setTarget(this->targetValue->name);
   this->ui.valueDesc->setTarget(this->targetValue->desc);
   this->ui.valueValue->setValue(this->targetValue->value);
}