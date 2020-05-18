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
      //
      ReachString* string;
      bool created = false;
      if (!table.size()) {
         string  = table.add_new();
         created = true;
         if (!string)
            return;
      } else
         string = &table.strings[0];
      //
      auto index = string->index; // should always be zero
      if (LocalizedStringEditorModal::editString(this, ReachStringFlags::IsNotInStandardTable, string)) {
         auto& english = string->get_content(reach::language::english);
         this->ui.labelName->setText(QString::fromUtf8(english.c_str()));
         if (!english.size())
            //
            // I suspect the game would actually support zero-length text, but don't allow that. If the 
            // text is cleared, revert to using the engine default.
            //
            if (index >= 0)
               table.remove(0);
      } else {
         if (created && index >= 0)
            table.remove(index);
      }
   });
   QObject::connect(this->ui.buttonDesc, &QPushButton::clicked, [this]() {
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& table = mp->localizedDesc;
      //
      ReachString* string;
      bool created = false;
      if (!table.size()) {
         string  = table.add_new();
         created = true;
         if (!string)
            return;
      } else
         string = &table.strings[0];
      //
      auto index = string->index; // should always be zero
      if (LocalizedStringEditorModal::editString(this, ReachStringFlags::IsNotInStandardTable, string)) {
         auto& english = string->get_content(reach::language::english);
         this->ui.labelDesc->setText(QString::fromUtf8(english.c_str()));
         if (!english.size())
            //
            // I suspect the game would actually support zero-length text, but don't allow that. If the 
            // text is cleared, revert to using the engine default.
            //
            if (index >= 0)
               table.remove(0);
      } else {
         if (created && index >= 0)
            table.remove(index);
      }
   });
   QObject::connect(this->ui.buttonCategory, &QPushButton::clicked, [this]() {
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      auto& table = mp->localizedCategory;
      //
      ReachString* string;
      bool created = false;
      if (!table.size()) {
         string  = table.add_new();
         created = true;
         if (!string)
            return;
      } else
         string = &table.strings[0];
      //
      auto index = string->index; // should always be zero
      if (LocalizedStringEditorModal::editString(this, ReachStringFlags::IsNotInStandardTable, string)) {
         auto& english = string->get_content(reach::language::english);
         this->ui.labelCategory->setText(QString::fromUtf8(english.c_str()));
         if (!english.size())
            //
            // I suspect the game would actually support zero-length text, but don't allow that. If the 
            // text is cleared, revert to using the engine default.
            //
            if (index >= 0)
               table.remove(0);
      } else {
         if (created && index >= 0)
            table.remove(index);
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
      this->ui.genericName->clearTarget();
      return;
   }
   {
      auto  widget = this->ui.labelName;
      auto  button = this->ui.buttonName;
      auto& table  = mp->localizedName;
      if (!table.size()) {
         widget->setText("");
      } else {
         ReachString& string = table.strings[0];
         widget->setText(QString::fromUtf8(string.get_content(reach::language::english).c_str()));
      }
      button->setDisabled(false);
   }
   {
      auto  widget = this->ui.labelDesc;
      auto  button = this->ui.buttonDesc;
      auto& table  = mp->localizedDesc;
      if (!table.size()) {
         widget->setText("");
      } else {
         ReachString& string = table.strings[0];
         widget->setText(QString::fromUtf8(string.get_content(reach::language::english).c_str()));
      }
      button->setDisabled(false);
   }
   {
      auto  widget = this->ui.labelCategory;
      auto  button = this->ui.buttonCategory;
      auto& table  = mp->localizedCategory;
      if (!table.size()) {
         widget->setText("");
      } else {
         ReachString& string = table.strings[0];
         widget->setText(QString::fromUtf8(string.get_content(reach::language::english).c_str()));
      }
      button->setDisabled(false);
   }
   this->ui.genericName->setTarget(mp->genericName);
}