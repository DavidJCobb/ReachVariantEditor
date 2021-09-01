#include "script_editor.h"
#include "../game_variants/data/object_types.h"
#include "localized_string_editor.h"

MegaloScriptEditorWindow::MegaloScriptEditorWindow(QWidget* parent) : QDialog(parent) {
   ui.setupUi(this);
   //
   this->setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
   QObject::connect(this->ui.navigation, &QListWidget::currentItemChanged, this, [this](QListWidgetItem* current, QListWidgetItem* previous) {
      auto stack = this->ui.stack;
      if (current->text() == "Metadata Strings") {
         stack->setCurrentWidget(this->ui.pageMetadata);
         return;
      }
      if (current->text() == "All Other Strings") {
         stack->setCurrentWidget(this->ui.pageStringTable);
         return;
      }
      if (current->text() == "Forge Labels") {
         stack->setCurrentWidget(this->ui.pageForgeLabels);
         return;
      }
      if (current->text() == "Map Permissions") {
         stack->setCurrentWidget(this->ui.pageMapPerms);
         return;
      }
      if (current->text() == "Player Rating Parameters") {
         stack->setCurrentWidget(this->ui.pageRatingParams);
         return;
      }
      if (current->text() == "Required Object Types") {
         stack->setCurrentWidget(this->ui.pageReqObjectTypes);
         return;
      }
      if (current->text() == "Scripted Options") {
         stack->setCurrentWidget(this->ui.pageScriptOptions);
         return;
      }
      if (current->text() == "Scripted Player Traits") {
         stack->setCurrentWidget(this->ui.pageScriptTraits);
         return;
      }
      if (current->text() == "Scripted Stats") {
         stack->setCurrentWidget(this->ui.pageScriptStats);
         return;
      }
      if (current->text() == "Scripted HUD Widgets") {
         stack->setCurrentWidget(this->ui.pageScriptWidgets);
         return;
      }
      if (current->text() == "Gametype Code") {
         stack->setCurrentWidget(this->ui.pageScriptCode);
         return;
      }
   });
}