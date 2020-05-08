#include "page_script_stats.h"
#include <QMessageBox>

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
ScriptEditorPageScriptStats::ScriptEditorPageScriptStats(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   //
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &ScriptEditorPageScriptStats::updateStatsListFromVariant);
   QObject::connect(&editor, &ReachEditorState::variantAbandoned, [this]() {
      this->target = nullptr;
      this->updateStatsListFromVariant();
   });
   {  // Handle option/value name changes
      //
      // The simplest way to handle this is to just refresh both lists whenever any string in the variant changes. 
      // That's also the laziest way to handle things... :<
      //
      QObject::connect(&editor, &ReachEditorState::stringModified, [this]() {
         this->updateStatsListFromVariant();
      });
      QObject::connect(&editor, &ReachEditorState::stringTableModified, [this]() {
         this->updateStatsListFromVariant();
      });
   }
   //
   QObject::connect(this->ui.list, &QListWidget::currentRowChanged, this, &ScriptEditorPageScriptStats::selectStat);
   //
   {  // Option properties
      //
      // Explicit hooks for the name and description aren't needed; once we supply a string ref to the string picker 
      // widgets, they handle everything from there on out.
      //
      QObject::connect(this->ui.name, &ReachStringPicker::selectedStringChanged, [this]() {
         this->updateStatsListFromVariant();
      });
      QObject::connect(this->ui.format, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
         if (!this->target)
            return;
         this->target->format = (ReachMegaloGameStat::Format)index;
      });
      QObject::connect(this->ui.sortOrder, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
         if (!this->target)
            return;
         this->target->sortOrder = (ReachMegaloGameStat::Sort)index;
      });
      QObject::connect(this->ui.groupByTeam, &QCheckBox::stateChanged, [this](int state) {
         if (!this->target)
            return;
         this->target->groupByTeam = state == Qt::Checked;
      });
      //
      QObject::connect(this->ui.buttonNew, &QPushButton::clicked, [this]() {
         auto& editor = ReachEditorState::get();
         auto  mp     = editor.multiplayerData();
         if (!mp)
            return;
         auto& list = mp->scriptContent.stats;
         if (list.size() >= Megalo::Limits::max_script_options) {
            QMessageBox::information(this, tr("Cannot add stat"), tr("Game variants cannot have more than %1 stats.").arg(Megalo::Limits::max_script_stats));
            return;
         }
         this->target = list.emplace_back();
         this->target->is_defined = true;
         if (auto str = mp->scriptData.strings.get_empty_entry()) // If possible, default the new stat's name to an empty string
            this->target->name = str;
         this->updateStatFromVariant();
         this->updateStatsListFromVariant();
      });
      QObject::connect(this->ui.buttonMoveUp, &QPushButton::clicked, [this]() {
         if (!this->target)
            return;
         auto& editor = ReachEditorState::get();
         auto  mp     = editor.multiplayerData();
         if (!mp)
            return;
         auto& list  = mp->scriptContent.stats;
         auto  index = list.index_of(this->target);
         if (index < 0)
            return;
         if (index == 0) // can't move the first item up
            return;
         list.swap_items(index, index - 1);
         this->updateStatsListFromVariant();
      });
      QObject::connect(this->ui.buttonMoveDown, &QPushButton::clicked, [this]() {
         if (!this->target)
            return;
         auto& editor = ReachEditorState::get();
         auto  mp     = editor.multiplayerData();
         if (!mp)
            return;
         auto& list  = mp->scriptContent.stats;
         auto  index = list.index_of(this->target);
         if (index < 0)
            return;
         if (index == list.size() - 1) // can't move the last item down
            return;
         list.swap_items(index, index + 1);
         this->updateStatsListFromVariant();
      });
      QObject::connect(this->ui.buttonDelete, &QPushButton::clicked, [this]() {
         if (!this->target)
            return;
         if (this->target->get_refcount()) {
            QMessageBox::information(this, tr("Cannot remove stat"), tr("This stat is still in use by the gametype's script. It cannot be removed at this time."));
            return;
         }
         auto& editor = ReachEditorState::get();
         auto  mp     = editor.multiplayerData();
         if (!mp)
            return;
         auto& list  = mp->scriptContent.stats;
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
         this->updateStatFromVariant();
         this->updateStatsListFromVariant();
      });
   }
   //
   this->updateStatsListFromVariant(nullptr);
}

void ScriptEditorPageScriptStats::selectStat(int32_t i) {
   if (i < 0) {
      this->target = nullptr;
   } else {
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& list = mp->scriptContent.stats;
      if (i >= list.size())
         return;
      this->target = &list[i];
   }
   this->updateStatFromVariant();
}

void ScriptEditorPageScriptStats::updateStatsListFromVariant(GameVariant* variant) {
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
   auto& list = mp->scriptContent.stats;
   for (size_t i = 0; i < list.size(); i++) {
      auto& option = list[i];
      auto  item   = new QListWidgetItem;
      if (option.name)
         item->setText(QString::fromUtf8(option.name->language(reach::language::english).c_str()));
      else
         item->setText(tr("<unnamed stat %1>", "scripted stat editor").arg(i));
      item->setData(Qt::ItemDataRole::UserRole, QVariant::fromValue((void*)&option));
      this->ui.list->addItem(item);
   }
   if (this->target)
      _selectByPointerData(this->ui.list, this->target);
}
void ScriptEditorPageScriptStats::updateStatFromVariant() {
   const QSignalBlocker blocker0(this->ui.name);
   const QSignalBlocker blocker1(this->ui.format);
   const QSignalBlocker blocker2(this->ui.sortOrder);
   const QSignalBlocker blocker3(this->ui.groupByTeam);
   //
   if (!this->target) {
      this->ui.name->clearTarget();
      //
      // TOOD: reset controls to a blank state
      //
      this->ui.groupByTeam->setChecked(false);
      return;
   }
   auto& s = *this->target;
   this->ui.name->setTarget(s.name);
   this->ui.format->setCurrentIndex((int)s.format);
   this->ui.sortOrder->setCurrentIndex((int)s.sortOrder);
   this->ui.groupByTeam->setChecked(s.groupByTeam);
}