#include "page_multiplayer_settings_scripted.h"
#include "../../game_variants/types/multiplayer.h"
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>

PageMPSettingsScripted::PageMPSettingsScripted(QWidget* parent) : QWidget(parent) {
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPSettingsScripted::updateFromVariant);
   QObject::connect(&editor, &ReachEditorState::scriptOptionModified, this, &PageMPSettingsScripted::updateFromOption);
   QObject::connect(&editor, &ReachEditorState::scriptOptionsModified, [this]() {
      this->updateFromVariant(nullptr);
   });
}

namespace {
   void _onMegaloComboboxChange(QComboBox* widget, int index) {
      auto data = ReachEditorState::get().multiplayerData();
      if (!data)
         return;
      auto v = widget->property("MegaloOptionIndex");
      if (!v.isValid())
         return;
      int32_t i = v.toInt();
      if (i < 0)
         return;
      auto& list   = data->scriptData.options;
      if (i >= list.size())
         return;
      auto& option = data->scriptData.options[i];
      option.currentValueIndex = index;
   }
   void _onMegaloSliderChange(QWidget* widget, int value) {
      auto data = ReachEditorState::get().multiplayerData();
      if (!data)
         return;
      auto v = widget->property("MegaloOptionIndex");
      if (!v.isValid())
         return;
      int32_t i = v.toInt();
      if (i < 0)
         return;
      auto& list = data->scriptData.options;
      if (i >= list.size())
         return;
      auto& option = data->scriptData.options[i];
      option.rangeCurrent = value;
   }
}
void PageMPSettingsScripted::_renderOptionRow(uint32_t i, QGridLayout* layout, const ReachMegaloOption& option) {
   ReachString* desc   = option.desc;
   //
   auto label = new QLabel(this);
   label->setProperty("MegaloOptionIndex", i);
   if (option.name) {
      label->setText(QString::fromUtf8(option.name->get_content(reach::language::english).c_str()));
   } else {
      label->setText(QString("Unnamed Option #%1").arg(i + 1));
   }
   layout->addWidget(label, i, 0);
   if (desc)
      label->setToolTip(QString::fromUtf8(desc->get_content(reach::language::english).c_str()));
   //
   if (option.isRange) {
      auto container = new QWidget(this);
      auto horizontal = new QBoxLayout(QBoxLayout::Direction::LeftToRight);
      horizontal->setContentsMargins(0, 0, 0, 0);
      container->setLayout(horizontal);
      layout->addWidget(container, i, 1);
      //
      auto widget = new QSpinBox(container);
      widget->setProperty("MegaloOptionIndex", i);
      widget->setMinimum(option.rangeMin->value);
      widget->setMaximum(option.rangeMax->value);
      widget->setSingleStep(1);
      widget->setValue(option.rangeCurrent);
      widget->setAlignment(Qt::AlignRight);
      QObject::connect(widget, QOverload<int>::of(&QSpinBox::valueChanged), [widget](int v) { _onMegaloSliderChange(widget, v); });
      if (desc)
         widget->setToolTip(QString::fromUtf8(desc->get_content(reach::language::english).c_str()));
      //
      auto labelMin = new QLabel(container);
      labelMin->setText(QString::fromUtf8(u8"%1 \u2264 ").arg(option.rangeMin->value));
      labelMin->setBuddy(widget);
      labelMin->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
      auto labelMax = new QLabel(container);
      labelMax->setText(QString::fromUtf8(u8" \u2264 %1").arg(option.rangeMax->value));
      labelMax->setBuddy(widget);
      //
      horizontal->addWidget(labelMin, 2);
      horizontal->addWidget(widget, 1);
      horizontal->addWidget(labelMax, 0);
      //
      label->setBuddy(widget);
   } else {
      auto combo = new QComboBox(this);
      combo->setProperty("MegaloOptionIndex", i);
      for (uint32_t j = 0; j < option.values.size(); j++) {
         auto& value = *option.values[j];
         if (value.name) {
            combo->addItem(QString::fromUtf8(value.name->get_content(reach::language::english).c_str()), (int16_t)value.value);
         } else {
            combo->addItem(QString(value.value), (int16_t)value.value);
         }
      }
      combo->setCurrentIndex(option.currentValueIndex);
      layout->addWidget(combo, i, 1);
      //
      // TODO: display default value?
      //
      QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), [combo](int v) { _onMegaloComboboxChange(combo, v); });
      if (desc)
         combo->setToolTip(QString::fromUtf8(desc->get_content(reach::language::english).c_str()));
      label->setBuddy(combo);
   }
}
void PageMPSettingsScripted::updateFromVariant(GameVariant* variant) {
   auto page   = this;
   auto layout = dynamic_cast<QGridLayout*>(page->layout());
   if (!layout) {
      layout = new QGridLayout;
      page->setLayout(layout);
      layout->setContentsMargins(0, 0, 0, 0);
   }
   {  // Clear layout
      QLayoutItem* child;
      while ((child = layout->takeAt(0)) != nullptr) {
         auto widget = child->widget();
         if (widget)
            delete widget;
         delete child;
      }
   }
   auto data = ReachEditorState::get().multiplayerData();
   if (!data)
      return;
   const auto& options = data->scriptData.options;
   for (uint32_t i = 0; i < options.size(); i++) {
      auto& option = options[i];
      this->_renderOptionRow(i, layout, option);
   }
   auto spacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
   layout->addItem(spacer, options.size(), 0, 1, 2);
}
void PageMPSettingsScripted::updateFromOption(ReachMegaloOption* option) {
   if (!option) // should never happen
      return;
   auto mp = ReachEditorState::get().multiplayerData();
   if (!mp)
      return;
   int8_t index  = option->index;
   auto   page   = this;
   auto   layout = dynamic_cast<QGridLayout*>(page->layout());
   if (!layout)
      return;
   std::vector<QWidget*> widgets;
   for (int c = 0; c < layout->columnCount(); ++c) {
      QLayoutItem* item   = layout->itemAtPosition(index, c);
      QWidget*     widget = item->widget();
      if (widget)
         widgets.push_back(widget);
   }
   for (auto widget : widgets) {
      layout->removeWidget(widget);
      delete widget;
   }
   this->_renderOptionRow(index, layout, *option);
}