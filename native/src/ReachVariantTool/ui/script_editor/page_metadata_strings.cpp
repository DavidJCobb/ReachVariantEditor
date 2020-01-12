#include "page_metadata_strings.h"
#include "../localized_string_editor.h"

ScriptEditorPageMetaStrings::ScriptEditorPageMetaStrings(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   //
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &ScriptEditorPageMetaStrings::updateFromVariant);
   QObject::connect(&editor, &ReachEditorState::variantAbandoned, [this]() {
      this->updateFromVariant();
   });
   //
   QObject::connect(this->ui.buttonName, &QPushButton::clicked, [this]() {
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& table = mp->localizedName;
      if (!table.size())
         return;
      ReachString* string = table.strings[0];
      if (LocalizedStringEditorModal::editString(this, ReachStringFlags::IsNotInStandardTable, string)) {
         this->ui.labelName->setText(QString::fromUtf8(string->english().c_str()));
      }
   });
   QObject::connect(this->ui.buttonDesc, &QPushButton::clicked, [this]() {
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& table = mp->localizedDesc;
      if (!table.size())
         return;
      ReachString* string = table.strings[0];
      if (LocalizedStringEditorModal::editString(this, ReachStringFlags::IsNotInStandardTable, string)) {
         this->ui.labelDesc->setText(QString::fromUtf8(string->english().c_str()));
      }
   });
   QObject::connect(this->ui.buttonCategory, &QPushButton::clicked, [this]() {
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& table = mp->localizedCategory;
      if (!table.size())
         return;
      ReachString* string = table.strings[0];
      if (LocalizedStringEditorModal::editString(this, ReachStringFlags::IsNotInStandardTable, string)) {
         this->ui.labelCategory->setText(QString::fromUtf8(string->english().c_str()));
      }
   });
   //
   this->updateFromVariant(nullptr);
}
void ScriptEditorPageMetaStrings::updateFromVariant(GameVariant* variant) {
   GameVariantDataMultiplayer* mp = nullptr;
   if (variant)
      mp = variant->get_multiplayer_data();
   else
      mp = ReachEditorState::get().multiplayerData();
   if (!mp) {
      this->ui.labelName->setText("");
      this->ui.labelDesc->setText("");
      this->ui.labelCategory->setText("");
      this->ui.buttonName->setDisabled(true);
      this->ui.buttonDesc->setDisabled(true);
      this->ui.buttonCategory->setDisabled(true);
      return;
   }
   {
      auto  widget = this->ui.labelName;
      auto  button = this->ui.buttonName;
      auto& table  = mp->localizedName;
      if (!table.size()) {
         widget->setText("");
      } else {
         ReachString* string = table.strings[0];
         widget->setText(QString::fromUtf8(string->english().c_str()));
      }
      button->setDisabled(table.size() == 0);
   }
   {
      auto  widget = this->ui.labelDesc;
      auto  button = this->ui.buttonDesc;
      auto& table  = mp->localizedDesc;
      if (!table.size()) {
         widget->setText("");
      } else {
         ReachString* string = table.strings[0];
         widget->setText(QString::fromUtf8(string->english().c_str()));
      }
      button->setDisabled(table.size() == 0);
   }
   {
      auto  widget = this->ui.labelCategory;
      auto  button = this->ui.buttonCategory;
      auto& table  = mp->localizedCategory;
      if (!table.size()) {
         widget->setText("");
      } else {
         ReachString* string = table.strings[0];
         widget->setText(QString::fromUtf8(string->english().c_str()));
      }
      button->setDisabled(table.size() == 0);
   }
}