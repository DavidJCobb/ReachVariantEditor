#include "script_editor.h"
#include <QMenuBar>
#include <QResource>
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
   //
   auto* menubar = new QMenuBar(this);
   this->layout()->setMenuBar(menubar);
   {
      auto* root = new QMenu(tr("File"), menubar);
      {
         auto* action = this->menu_actions.save = new QAction(tr("Save"), root);
         QObject::connect(action, &QAction::triggered, this, [this]() { ReachEditorState::get().saveVariant(this, false); });
      }
      {
         auto* action = this->menu_actions.saveAs = new QAction(tr("Save As..."), root);
         QObject::connect(action, &QAction::triggered, this, [this]() { ReachEditorState::get().saveVariant(this, true); });
      }
      root->addAction(this->menu_actions.save);
      root->addAction(this->menu_actions.saveAs);
      menubar->addAction(root->menuAction());
   }
   {
      auto* root = new QMenu(tr("Help"), menubar);
      {
         auto* action = this->menu_actions.help.web = new QAction(tr("View Help (in browser)"), root);
         QObject::connect(action, &QAction::triggered, this, [this]() { ReachEditorState::get().openHelp(this, false); });
      }
      {
         auto* action = this->menu_actions.help.folder = new QAction(tr("View Help (in folder)"), root);
         QObject::connect(action, &QAction::triggered, this, [this]() { ReachEditorState::get().openHelp(this, true); });
      }
      root->addAction(this->menu_actions.help.web);
      root->addAction(this->menu_actions.help.folder);
      menubar->addAction(root->menuAction());
   }
   {
      this->menu_actions.save->setShortcut(QKeySequence::Save);
      this->menu_actions.saveAs->setShortcut(QKeySequence::SaveAs);
      this->menu_actions.help.folder->setShortcut(QKeySequence::HelpContents);
   }
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantFilePathChanged, this, &MegaloScriptEditorWindow::updateSaveMenuItems);
   QObject::connect(&editor, &ReachEditorState::variantAbandoned,       this, &MegaloScriptEditorWindow::updateSaveMenuItems);
   QObject::connect(&editor, &ReachEditorState::variantAcquired,        this, &MegaloScriptEditorWindow::updateSaveMenuItems);
   this->updateSaveMenuItems();
}

void MegaloScriptEditorWindow::keyPressEvent(QKeyEvent* event) {
   if (event->matches(QKeySequence::Cancel)) {
      event->ignore();
      return;
   }
   QDialog::keyPressEvent(event);
}

void MegaloScriptEditorWindow::updateSaveMenuItems() {
   auto& editor = ReachEditorState::get();
   if (!editor.variant()) {
      this->menu_actions.save->setEnabled(false);
      this->menu_actions.saveAs->setEnabled(false);
      return;
   }
   //
   bool is_resource = false;
   {
      std::wstring file = ReachEditorState::get().variantFilePath();
      if (file[0] == ':') { // Qt resource?
         is_resource = QResource(QString::fromStdWString(file)).isValid();
      }
   }
   this->menu_actions.save->setEnabled(!is_resource);
   this->menu_actions.saveAs->setEnabled(true);
}