#include "page_string_table.h"
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QMessageBox>
#include "../localized_string_editor.h"
#include "../../helpers/string_scanner.h"
#include "../../editor_state.h"
#include "../../game_variants/types/multiplayer.h"

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
ScriptEditorPageStringTable::ScriptEditorPageStringTable(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   //
   QObject::connect(&editor, &ReachEditorState::stringModified, [this](uint32_t index) {
      this->updateFromVariant(nullptr);
      if (!_selectByPointerData(this->ui.list, this->_selected))
         //
         // We failed to select anything, which means that the previously-selected string was 
         // likely removed from the table altogether.
         //
         this->_selected = nullptr;
   });
   QObject::connect(&editor, &ReachEditorState::stringTableModified, [this]() {
      this->updateFromVariant(nullptr);
      if (!_selectByPointerData(this->ui.list, this->_selected))
         //
         // We failed to select anything, which means that the previously-selected string was 
         // likely removed from the table altogether.
         //
         this->_selected = nullptr;
   });
   QObject::connect(&editor, &ReachEditorState::variantRecompiled, [this]() {
      this->updateFromVariant(nullptr);
      if (!_selectByPointerData(this->ui.list, this->_selected))
         //
         // We failed to select anything, which means that the previously-selected string was 
         // likely removed from the table altogether.
         //
         this->_selected = nullptr;
   });
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &ScriptEditorPageStringTable::updateFromVariant);
   //
   QObject::connect(this->ui.list, &QListWidget::currentItemChanged, [this](QListWidgetItem* current, QListWidgetItem* previous) {
      this->_selected = nullptr;
      if (!current)
         return;
      auto data = current->data(Qt::ItemDataRole::UserRole);
      if (!data.isValid())
         return;
      this->_selected = (ReachString*)data.value<void*>();
      this->ui.buttonDelete->setDisabled(false);
      this->ui.buttonEdit->setDisabled(false);
      //
      auto index = this->ui.list->row(current);
      if (index < 0)
         return;
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& table = mp->scriptData.strings;
      if (index >= table.size())
         return;
      auto& string = table.strings[index];
      if (string.get_refcount())
         this->ui.buttonDelete->setDisabled(true);
   });
   QObject::connect(this->ui.list, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* target) {
      auto widget = this->ui.list;
      widget->setCurrentItem(target);
      this->ui.buttonEdit->clicked();
   });
   QObject::connect(this->ui.buttonEdit, &QPushButton::clicked, [this]() {
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto  list  = this->ui.list;
      auto& table = mp->scriptData.strings;
      auto  index = list->currentRow();
      if (index < 0 || index > table.strings.size())
         return;
      auto& string = table.strings[index];
      uint32_t flags = 0;
      if (table.is_at_count_limit())
         flags |= ReachStringFlags::DisallowSaveAsNew;
      if (LocalizedStringEditorModal::editString(this, flags, &string)) {
         //
         // The string had changes saved, which will cause us to receive a signal from ReachEditorState 
         // and redraw the list. We need to reselect what we had selected before.
         //
         if (index < list->count()) // sanity check; should always be true
            list->setCurrentRow(index);
      }
   });
   QObject::connect(this->ui.buttonNew, &QPushButton::clicked, [this]() {
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& table = mp->scriptData.strings;
      if (table.is_at_count_limit()) {
         QMessageBox::information(this, tr("Cannot add string"), tr("The string table contains the maximum number of strings. No more can be added."));
         return;
      }
      if (LocalizedStringEditorModal::editString(this, 0, nullptr)) {
         this->ui.list->setCurrentRow(this->ui.list->count() - 1); // select the newly-added string
      }
   });
   QObject::connect(this->ui.buttonDelete, &QPushButton::clicked, [this]() {
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto   list  = this->ui.list;
      auto   index = list->currentRow();
      auto&  table = mp->scriptData.strings;
      size_t count = table.strings.size();
      if (index < 0 || index > count)
         return;
      if (table.strings[index].get_refcount()) {
         QMessageBox::information(this, tr("Cannot remove string"), tr("This string is still in use by the gametype or its script. It cannot be removed at this time."));
         return;
      }
      table.remove(index);
      --count;
      ReachEditorState::get().stringTableModified();
      if (count) {
         if (index >= count)
            list->setCurrentRow(count - 1);
         else
            list->setCurrentRow(index);
      }
   });
   //
   {  // Set up the "copy" button.
      auto button = this->ui.buttonCopy;
      auto menu   = new QMenu(this);
      button->setMenu(menu);
      //
      auto act_copy_text  = menu->addAction(tr("Copy Text",  "string table copy option"));
      auto act_copy_code  = menu->addAction(tr("Copy Code",  "string table copy option"));
      auto act_copy_index = menu->addAction(tr("Copy Index", "string table copy option"));
      QObject::connect(act_copy_text, &QAction::triggered, [this, act_copy_text]() {
         auto str = this->getSelected();
         if (str) {
            auto english = str->get_content(reach::language::english).c_str();
            QApplication::clipboard()->setText(english, QClipboard::Clipboard);
         }
      });
      QObject::connect(act_copy_code, &QAction::triggered, [this, act_copy_code]() {
         auto str = this->getSelected();
         if (str) {
            QString english = str->get_content(reach::language::english).c_str();
            english = cobb::string_scanner::escape(english, '"');
            english.prepend('"');
            english.append('"');
            QApplication::clipboard()->setText(english, QClipboard::Clipboard);
         }
      });
      QObject::connect(act_copy_index, &QAction::triggered, [this, act_copy_index]() {
         auto str = this->getSelected();
         if (str) {
            auto text = QString("%1").arg(str->index);
            QApplication::clipboard()->setText(text, QClipboard::Clipboard);
         }
      });
   }
   //
   this->updateFromVariant(nullptr);
}
ReachString* ScriptEditorPageStringTable::getSelected() {
   auto mp = ReachEditorState::get().multiplayerData();
   if (!mp)
      return nullptr;
   auto   list  = this->ui.list;
   auto   index = list->currentRow();
   auto&  table = mp->scriptData.strings;
   return table.get_entry(index);
}
void ScriptEditorPageStringTable::updateFromVariant(GameVariant* variant) {
   GameVariantDataMultiplayer* mp = nullptr;
   if (variant)
      mp = variant->get_multiplayer_data();
   else
      mp = ReachEditorState::get().multiplayerData();
   if (!mp) {
      this->ui.statsReadout->setText(tr("No multiplayer game variant loaded.", "string table editor"));
      return;
   }
   auto  list = this->ui.list;
   list->clear();
   //
   auto& table = mp->scriptData.strings;
   auto  count = table.strings.size();
   for (size_t i = 0; i < count; i++) {
      auto& string = table.strings[i];
      auto  item   = new QListWidgetItem();
      item->setText(QString::fromUtf8(string.get_content(reach::language::english).c_str()));
      item->setData(Qt::ItemDataRole::UserRole, QVariant::fromValue((void*)&string));
      list->addItem(item);
   }
   this->ui.statsReadout->setText(tr("Total string table size: %1 / %2 bytes", "string table editor").arg(table.total_bytecount()).arg(table.max_buffer_size));
}