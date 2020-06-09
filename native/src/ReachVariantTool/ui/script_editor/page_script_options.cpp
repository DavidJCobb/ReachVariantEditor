#include "page_script_options.h"
#include <QMessageBox>

namespace {
   enum _range_property_to_modify {
      min,
      max,
      default,
   };

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
   void _modifyOptionRangeProperty(ReachMegaloOption* option, _range_property_to_modify what, int16_t value) {
      if (!option)
         return;
      ReachMegaloOptionValueEntry* p = nullptr;
      auto& current = option->rangeCurrent;
      switch (what) {
         case _range_property_to_modify::min:
            p = option->rangeMin;
            if (p && current < value)
               current = value;
            break;
         case _range_property_to_modify::max:
            p = option->rangeMax;
            if (p && current > value)
               current = value;
            break;
         case _range_property_to_modify::default:
            p = option->rangeDefault;
            break;
         default:
            return;
      }
      if (!p)
         return;
      p->value = value;
      ReachEditorState::get().scriptOptionModified(option);
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
   QObject::connect(&editor, &ReachEditorState::stringModified, [this](uint32_t index) { // Handle option/value name changes from outside
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto str = mp->scriptData.strings.get_entry(index);
      if (!str || str->get_refcount() == 0)
         return;
      bool is_option  = false;
      bool is_current = false;
      for (auto& option : mp->scriptData.options) {
         if (option.uses_string(str)) {
            is_option = true;
            if (&option == this->targetOption) {
               is_current = true;
               break;
            }
         }
      }
      if (is_option) {
         this->updateOptionsListFromVariant();
         if (is_current)
            this->updateValuesListFromVariant();
      }
   });
   //
   QObject::connect(this->ui.listOptions, &QListWidget::currentRowChanged, this, &ScriptEditorPageScriptOptions::selectOption);
   QObject::connect(this->ui.listValues,  &QListWidget::currentRowChanged, this, &ScriptEditorPageScriptOptions::selectOptionValue);
   //
   {  // Option properties
      //
      // Explicit hooks for the name and description aren't needed; once we supply a string ref to the string picker 
      // widgets, they handle everything from there on out.
      //
      QObject::connect(this->ui.optionName, &ReachStringPicker::selectedStringChanged, [this]() {
         this->updateOptionsListFromVariant();
      });
      QObject::connect(this->ui.optionName, &ReachStringPicker::selectedStringSwitched, [this]() {
         ReachEditorState::get().scriptOptionModified(this->targetOption); // ReachEditorState sends this for us if the string is edited, but if we select a different string, we need to send it
      });
      QObject::connect(this->ui.optionDesc, &ReachStringPicker::selectedStringSwitched, [this]() {
         ReachEditorState::get().scriptOptionModified(this->targetOption); // ReachEditorState sends this for us if the string is edited, but if we select a different string, we need to send it
      });
      //
      QObject::connect(this->ui.optionType, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
         if (!this->targetOption)
            return;
         if (index == 1)
            this->targetOption->make_range();
         else
            this->targetOption->isRange = false;
         this->updateOptionFromVariant();
         ReachEditorState::get().scriptOptionModified(this->targetOption);
      });
      QObject::connect(this->ui.rangeMin, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
         _modifyOptionRangeProperty(this->targetOption, _range_property_to_modify::min, value);
      });
      QObject::connect(this->ui.rangeMax, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
         _modifyOptionRangeProperty(this->targetOption, _range_property_to_modify::max, value);
      });
      QObject::connect(this->ui.rangeDefault, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
         _modifyOptionRangeProperty(this->targetOption, _range_property_to_modify::default, value);
      });
      //
      QObject::connect(this->ui.buttonOptionsNew, &QPushButton::clicked, [this]() {
         auto& editor = ReachEditorState::get();
         auto  mp     = editor.multiplayerData();
         if (!mp)
            return;
         auto& list = mp->scriptData.options;
         if (list.size() >= Megalo::Limits::max_script_options) {
            QMessageBox::information(this, tr("Cannot add option"), tr("Game variants cannot have more than %1 options.").arg(Megalo::Limits::max_script_options));
            return;
         }
         this->targetOption = mp->create_script_option();
         this->selectOption(this->targetOption ? this->targetOption->index : -1);
         this->updateOptionsListFromVariant();
         ReachEditorState::get().scriptOptionsModified();
      });
      QObject::connect(this->ui.buttonOptionsMoveUp, &QPushButton::clicked, [this]() {
         if (!this->targetOption)
            return;
         auto& editor = ReachEditorState::get();
         auto  mp     = editor.multiplayerData();
         if (!mp)
            return;
         auto index = this->targetOption->index;
         #if _DEBUG
            assert(index >= 0 && "Script Editor: Options: Move Up: Target option doesn't know its index?");
         #else
            if (index < 0)
               return;
         #endif
         if (mp->swap_script_options(index, index - 1)) {
            this->updateOptionsListFromVariant();
            ReachEditorState::get().scriptOptionsModified();
         }
      });
      QObject::connect(this->ui.buttonOptionsMoveDown, &QPushButton::clicked, [this]() {
         if (!this->targetOption)
            return;
         auto& editor = ReachEditorState::get();
         auto  mp     = editor.multiplayerData();
         if (!mp)
            return;
         auto index = this->targetOption->index;
         #if _DEBUG
            assert(index >= 0 && "Script Editor: Options: Move Up: Target option doesn't know its index?");
         #else
            if (index < 0)
               return;
         #endif
         if (mp->swap_script_options(index, index + 1)) {
            this->updateOptionsListFromVariant();
            ReachEditorState::get().scriptOptionsModified();
         }
      });
      QObject::connect(this->ui.buttonOptionsDelete, &QPushButton::clicked, [this]() {
         if (!this->targetOption)
            return;
         if (this->targetOption->get_refcount()) {
            QMessageBox::information(this, tr("Cannot remove option"), tr("This option is still in use by the gametype's script. It cannot be removed at this time."));
            return;
         }
         auto& editor = ReachEditorState::get();
         auto  mp     = editor.multiplayerData();
         if (!mp)
            return;
         auto& list  = mp->scriptData.options;
         auto  index = list.index_of(this->targetOption);
         if (index < 0)
            return;
         mp->delete_script_option(this->targetOption);
         size_t size = list.size();
         if (size) {
            if (index >= size)
               index = size - 1;
            this->selectOption(index);
         } else {
            this->selectOption(-1);
         }
         this->updateOptionsListFromVariant();
         ReachEditorState::get().scriptOptionsModified();
      });
   }
   {  // Value properties
      QObject::connect(this->ui.defaultEnumValue, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
         if (!this->targetOption)
            return;
         if (index < 0 || index >= this->targetOption->values.size())
            return;
         this->targetOption->defaultValueIndex = index;
      });
      //
      QObject::connect(this->ui.valueName, &ReachStringPicker::selectedStringChanged, [this]() {
         this->updateValuesListFromVariant();
      });
      QObject::connect(this->ui.valueName, &ReachStringPicker::selectedStringSwitched, [this]() {
         ReachEditorState::get().scriptOptionModified(this->targetOption); // ReachEditorState sends this for us if the string is edited, but if we select a different string, we need to send it
      });
      QObject::connect(this->ui.valueDesc, &ReachStringPicker::selectedStringSwitched, [this]() {
         ReachEditorState::get().scriptOptionModified(this->targetOption); // ReachEditorState sends this for us if the string is edited, but if we select a different string, we need to send it
      });
      QObject::connect(this->ui.valueValue, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
         if (!this->targetValue)
            return;
         this->targetValue->value = value;
         ReachEditorState::get().scriptOptionModified(this->targetOption);
      });
      //
      QObject::connect(this->ui.buttonValuesNew, &QPushButton::clicked, [this]() {
         if (!this->targetOption)
            return;
         auto& option = *this->targetOption;
         if (option.isRange)
            return;
         if (option.values.size() >= Megalo::Limits::max_script_option_values) {
            QMessageBox::information(this, tr("Cannot add option value"), tr("Options cannot have more than %1 values.").arg(Megalo::Limits::max_script_option_values));
            return;
         }
         auto* value = option.add_value();
         if (!value)
            return;
         this->targetValue = value;
         {  // If possible, default the new value's name and description to an empty string
            auto& editor = ReachEditorState::get();
            auto  mp     = editor.multiplayerData();
            if (mp) {
               auto str = mp->scriptData.strings.get_empty_entry();
               if (str) {
                  value->name = str;
                  value->desc = str;
               }
            }
         }
         this->updateValueFromVariant();
         this->updateValuesListFromVariant();
         if (this->targetValue) {
            const QSignalBlocker blocker(this->ui.listValues);
            this->ui.listValues->setCurrentRow(option.values.size());
         }
         ReachEditorState::get().scriptOptionModified(this->targetOption);
      });
      QObject::connect(this->ui.buttonValuesMoveUp, &QPushButton::clicked, [this]() {
         if (!this->targetOption || !this->targetValue)
            return;
         auto& option = *this->targetOption;
         auto  value  = this->targetValue;
         auto  index  = option.values.index_of(value);
         if (index < 0)
            return;
         if (index == 0) // can't move the first item up
            return;
         option.values.swap_items(index, index - 1);
         {
            auto c = option.currentValueIndex;
            if (c == index)
               c = index - 1;
            else if (c == index - 1)
               c = index;
         }
         if (index == this->targetOption->defaultValueIndex)
            this->targetOption->defaultValueIndex -= 1;
         this->updateValuesListFromVariant();
         ReachEditorState::get().scriptOptionModified(this->targetOption);
      });
      QObject::connect(this->ui.buttonValuesMoveDown, &QPushButton::clicked, [this]() {
         if (!this->targetOption || !this->targetValue)
            return;
         auto& option = *this->targetOption;
         auto  value  = this->targetValue;
         auto  index  = option.values.index_of(value);
         if (index < 0)
            return;
         if (index == option.values.size() - 1) // can't move the last item down
            return;
         option.values.swap_items(index, index + 1);
         {
            auto c = option.currentValueIndex;
            if (c == index)
               c = index + 1;
            else if (c == index + 1)
               c = index;
         }
         if (index == this->targetOption->defaultValueIndex)
            this->targetOption->defaultValueIndex += 1;
         this->updateValuesListFromVariant();
         ReachEditorState::get().scriptOptionModified(this->targetOption);
      });
      QObject::connect(this->ui.buttonValuesDelete, &QPushButton::clicked, [this]() {
         if (!this->targetOption || !this->targetValue)
            return;
         auto& option = *this->targetOption;
         auto  value  = this->targetValue;
         auto& list   = option.values;
         if (list.size() == 1) {
            QMessageBox::information(this, tr("Cannot remove option value"), tr("If a game variant contains enum-options with no values, that game variant won't be considered valid; it will not appear in the MCC's menus."));
            return;
         }
         auto  index  = list.index_of(value);
         option.delete_value(value);
         if (index > 0)
            this->targetValue = list[index - 1];
         else if (option.values.size())
            this->targetValue = list[0];
         else
            this->targetValue = nullptr;
         if (index == this->targetOption->defaultValueIndex) {
            int diff = (list.size() - 1) - index;
            this->targetOption->defaultValueIndex -= diff;
         }
         this->updateValueFromVariant();
         this->updateValuesListFromVariant();
         ReachEditorState::get().scriptOptionModified(this->targetOption);
      });
   }
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
      this->targetOption = &list[i];
   }
   this->updateOptionFromVariant();
   this->updateValuesListFromVariant();
   this->selectOptionValue(0);
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
   const QSignalBlocker blocker(this->ui.listOptions);
   //
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
      auto& option = list[i];
      auto  item   = new QListWidgetItem;
      if (option.name)
         item->setText(QString::fromUtf8(option.name->get_content(reach::language::english).c_str()));
      else
         item->setText(tr("<unnamed option %1>", "scripted option editor").arg(i));
      item->setData(Qt::ItemDataRole::UserRole, QVariant::fromValue((void*)&option));
      this->ui.listOptions->addItem(item);
   }
   if (this->targetOption)
      _selectByPointerData(this->ui.listOptions, this->targetOption);
   else
      this->updateOptionFromVariant();
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
      this->ui.optionName->clearTarget();
      this->ui.optionDesc->clearTarget();
      this->updateValuesListFromVariant();
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
   const QSignalBlocker blocker0(this->ui.listValues);
   const QSignalBlocker blocker1(this->ui.defaultEnumValue);
   //
   this->ui.listValues->clear();
   this->ui.defaultEnumValue->clear();
   if (!this->targetOption) {
      this->ui.buttonValuesDelete->setDisabled(true);
      return;
   }
   auto& list = this->targetOption->values;
   for (size_t i = 0; i < list.size(); i++) {
      auto&   value = *list[i];
      auto    item  = new QListWidgetItem;
      QString name;
      if (value.name)
         name = QString::fromUtf8(value.name->get_content(reach::language::english).c_str());
      else
         name = tr("<unnamed value %1>", "scripted option editor").arg(i);
      item->setText(name);
      item->setData(Qt::ItemDataRole::UserRole, QVariant::fromValue((void*)&value));
      this->ui.listValues->addItem(item);
      //
      this->ui.defaultEnumValue->addItem(name);
   }
   if (this->targetValue)
      _selectByPointerData(this->ui.listValues, this->targetValue);
   this->ui.defaultEnumValue->setCurrentIndex(this->targetOption->defaultValueIndex);
   this->ui.buttonValuesDelete->setDisabled(list.size() <= 1);
}
void ScriptEditorPageScriptOptions::updateValueFromVariant() {
   const QSignalBlocker blocker0(this->ui.valueName);
   const QSignalBlocker blocker1(this->ui.valueDesc);
   const QSignalBlocker blocker2(this->ui.valueValue);
   if (!this->targetValue) {
      this->ui.valueName->clearTarget();
      this->ui.valueDesc->clearTarget();
      this->ui.valueValue->setValue(0);
      return;
   }
   this->ui.valueName->setTarget(this->targetValue->name);
   this->ui.valueDesc->setTarget(this->targetValue->desc);
   this->ui.valueValue->setValue(this->targetValue->value);
}