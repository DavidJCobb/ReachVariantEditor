#include "page_script_widgets.h"
#include <QImage>
#include <QMessageBox>
#include <QPainter>

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
ScriptEditorPageHUDWidgets::ScriptEditorPageHUDWidgets(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   //
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &ScriptEditorPageHUDWidgets::updateWidgetsListFromVariant);
   QObject::connect(&editor, &ReachEditorState::variantAbandoned, [this]() {
      this->target = nullptr;
      this->updateWidgetsListFromVariant();
   });
   //
   this->ui.preview->addImage(QImage(":/ScriptEditor/hud_widget_preview/base_hud.png"));
   //
   QObject::connect(this->ui.list, &QListWidget::currentRowChanged, this, &ScriptEditorPageHUDWidgets::selectWidget);
   //
   {  // HUD widget properties
      //
      // Explicit hooks for the name and description aren't needed; once we supply a string ref to the string picker 
      // widgets, they handle everything from there on out.
      //
      QObject::connect(this->ui.position, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
         if (!this->target)
            return;
         this->target->position = index;
         this->updateWidgetPreview(index);
      });
      //
      QObject::connect(this->ui.buttonNew, &QPushButton::clicked, [this]() {
         auto& editor = ReachEditorState::get();
         auto  mp     = editor.multiplayerData();
         if (!mp)
            return;
         auto& list = mp->scriptContent.widgets;
         if (list.size() >= Megalo::Limits::max_script_widgets) {
            QMessageBox::information(this, tr("Cannot add HUD widget"), tr("Game variants cannot have more than %1 HUD widgets.").arg(Megalo::Limits::max_script_widgets));
            return;
         }
         this->target = list.emplace_back();
         this->target->is_defined = true;
         this->updateWidgetFromVariant();
         this->updateWidgetsListFromVariant();
      });
      QObject::connect(this->ui.buttonMoveUp, &QPushButton::clicked, [this]() {
         if (!this->target)
            return;
         auto& editor = ReachEditorState::get();
         auto  mp     = editor.multiplayerData();
         if (!mp)
            return;
         auto& list  = mp->scriptContent.widgets;
         auto  index = list.index_of(this->target);
         if (index < 0)
            return;
         if (index == 0) // can't move the first item up
            return;
         list.swap_items(index, index - 1);
         this->updateWidgetsListFromVariant();
      });
      QObject::connect(this->ui.buttonMoveDown, &QPushButton::clicked, [this]() {
         if (!this->target)
            return;
         auto& editor = ReachEditorState::get();
         auto  mp     = editor.multiplayerData();
         if (!mp)
            return;
         auto& list  = mp->scriptContent.widgets;
         auto  index = list.index_of(this->target);
         if (index < 0)
            return;
         if (index == list.size() - 1) // can't move the last item down
            return;
         list.swap_items(index, index + 1);
         this->updateWidgetsListFromVariant();
      });
      QObject::connect(this->ui.buttonDelete, &QPushButton::clicked, [this]() {
         if (!this->target)
            return;
         if (this->target->get_refcount()) {
            QMessageBox::information(this, tr("Cannot remove HUD widget"), tr("This HUD widget is still in use by the gametype's script. It cannot be removed at this time."));
            return;
         }
         auto& editor = ReachEditorState::get();
         auto  mp     = editor.multiplayerData();
         if (!mp)
            return;
         auto& list  = mp->scriptContent.widgets;
         auto  index = list.index_of(this->target);
         if (index < 0)
            return;
         list.erase(index);
         size_t size = list.size();
         if (size) {
            if (index >= size)
               this->target = &list[size - 1];
            else
               this->target = &list[index];
         } else
            this->target = nullptr;
         this->updateWidgetFromVariant();
         this->updateWidgetsListFromVariant();
      });
   }
   //
   this->updateWidgetsListFromVariant(nullptr);
   this->updateWidgetFromVariant();
}

void ScriptEditorPageHUDWidgets::selectWidget(int32_t i) {
   if (i < 0) {
      this->target = nullptr;
   } else {
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& list = mp->scriptContent.widgets;
      if (i >= list.size())
         return;
      this->target = &list[i];
   }
   this->updateWidgetFromVariant();
}

void ScriptEditorPageHUDWidgets::updateWidgetsListFromVariant(GameVariant* variant) {
   const QSignalBlocker blocker(this->ui.list);
   //
   this->ui.list->clear();
   //
   GameVariantDataMultiplayer* mp = nullptr;
   if (variant)
      mp = variant->get_multiplayer_data();
   else
      mp = ReachEditorState::get().multiplayerData();
   if (!mp)
      return;
   auto& list = mp->scriptContent.widgets;
   for (size_t i = 0; i < list.size(); i++) {
      auto& entry = list[i];
      auto  item  = new QListWidgetItem;
      item->setText(tr("HUD Widget #%1", "scripted widget editor").arg(i));
      item->setData(Qt::ItemDataRole::UserRole, QVariant::fromValue((void*)&entry));
      this->ui.list->addItem(item);
   }
   if (this->target)
      _selectByPointerData(this->ui.list, this->target);
}
void ScriptEditorPageHUDWidgets::updateWidgetFromVariant() {
   const QSignalBlocker blocker(this->ui.position);
   //
   if (!this->target) {
      this->updateWidgetPreview(-1);
      //
      // TOOD: reset controls to a blank state
      //
      return;
   }
   auto& s = *this->target;
   this->ui.position->setCurrentIndex(s.position);
   this->updateWidgetPreview(s.position);
}
void ScriptEditorPageHUDWidgets::updateWidgetPreview(int8_t position) {
   auto widget = this->ui.preview;
   widget->removeImage(1);
   if (position < 0) {
      if (!this->target)
         return;
      position = this->target->position;
   }
   widget->addImage(QImage(QString(":/ScriptEditor/hud_widget_preview/%1.png").arg(position)));
}