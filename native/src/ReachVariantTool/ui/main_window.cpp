#include "main_window.h"
#include <cassert>
#include <filesystem>
#include <QDebug>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QProcess>
#include <QSaveFile>
#include <QString>
#include <QTextCodec>
#include <QTextStream>
#include <QTreeWidget>
#include <QWidget>
#include "../editor_state.h"
#include "../game_variants/base.h"
#include "../game_variants/io_process.h"
#include "../game_variants/errors.h"
#include "../game_variants/warnings.h"
#include "../helpers/ini.h"
#include "../helpers/steam.h"
#include "../helpers/stream.h"
#include "../helpers/qt/tree_widget.h"
#include "../services/ini.h"

#include "options_window.h"
#include "script_editor.h"

#include "main_window/debug_helper_functions.h"

namespace {
   ReachVariantTool* _window = nullptr;

   enum class _page {
      __do_not_use_zero = 0,
      //
      welcome, // only shows when no file is loaded
      unknown_variant_type, // only shows when a file of an unknown type is (somehow) loaded
      mp_metadata,
      mp_options_general,
      mp_options_respawn,
      mp_options_social,
      mp_options_map,
      mp_options_team,
      mp_options_loadout,
      mp_options_scripted,
      loadout_palette,
      mp_traits_built_in,
      mp_traits_scripted,
      mp_team_configuration, // specific team
      mp_option_visibility,
      mp_title_update_1,
      forge_options_general,
      //
      ff_metadata,
      ff_respawn_elite,
      ff_scenario,
      ff_base_player_traits,
      ff_base_wave_traits,
      ff_custom_skull,
      ff_round,
      ff_bonus_wave,
      //
      redirect_to_first_child = 0x1000,
   };
   enum class _traits_builtin {
      base,
      respawn,
      powerup_red,
      powerup_blue,
      powerup_yellow,
      forge_editor,
   };

   inline bool _get_mcc_directory(std::wstring& out) {
      return cobb::steam::get_game_directory(976730, out);
   }
}

/*static*/ ReachVariantTool& ReachVariantTool::get() {
   assert(_window && "You shouldn't be calling ReachVariantTool::get before the main window is actually created!");
   return *_window;
}
ReachVariantTool::ReachVariantTool(QWidget *parent) : QMainWindow(parent) {
   ui.setupUi(this);
   _window = this;
   //
   auto& editor = ReachEditorState::get();
   //
   ReachINI::get().register_for_changes([](cobb::ini::setting* setting, cobb::ini::setting_value_union oldValue, cobb::ini::setting_value_union newValue) {
      if (setting == &ReachINI::UIWindowTitle::bShowFullPath || setting == &ReachINI::UIWindowTitle::bShowVariantTitle) {
         ReachVariantTool::get().refreshWindowTitle();
         return;
      }
   });
   {
      std::wstring dir;
      if (_get_mcc_directory(dir)) {
         this->dirBuiltInVariants      = QString::fromWCharArray(dir.c_str());
         this->dirMatchmakingVariants  = this->dirBuiltInVariants;
         this->dirBuiltInVariants     += "/haloreach/game_variants/";
         this->dirMatchmakingVariants += "/haloreach/hopper_game_variants/";
         //
         this->dirBuiltInVariants     = QDir::cleanPath(this->dirBuiltInVariants);
         this->dirMatchmakingVariants = QDir::cleanPath(this->dirMatchmakingVariants);
         //
         auto env = QProcessEnvironment::systemEnvironment();
         auto dir = QDir(env.value("USERPROFILE"));
         dir.cd("AppData/LocalLow/MCC/LocalFiles/");
         if (dir.exists()) {
            this->dirSavedVariants = dir.absolutePath();
         }
      }
   }
   QObject::connect(&editor, &ReachEditorState::scriptTraitsModified, [this](ReachMegaloPlayerTraits* traits) {
      if (!traits) { // traits were added, reordered, or removed
         this->regenerateNavigation();
         return;
      }
      auto& editor = ReachEditorState::get();
      auto  mp     = editor.multiplayerData();
      if (!mp) {
         this->regenerateNavigation();
         return;
      }
      int32_t index = traits->index;
      QTreeWidgetItem* target = this->getNavItemForScriptTraits(traits, index);
      if (!target) {
         this->regenerateNavigation();
         return;
      }
      //
      ReachString* name = traits->name;
      QString text;
      if (name) {
         text = QString::fromUtf8(name->get_content(reach::language::english).c_str());
      } else {
         text = QString("Unnamed Traits %1").arg(index);
      }
      target->setText(0, text);
      ReachString* desc = traits->desc;
      if (desc)
         target->setToolTip(0, QString::fromUtf8(desc->get_content(reach::language::english).c_str()));
      else
         target->setToolTip(0, "");
   });
   QObject::connect(&editor, &ReachEditorState::scriptOptionModified, [this](ReachMegaloOption*) {
      this->ui.optionTogglesScripted->updateModelFromGameVariant();
   });
   QObject::connect(&editor, &ReachEditorState::scriptOptionsModified, [this]() {
      this->ui.optionTogglesScripted->updateModelFromGameVariant();
   });
   //
   QObject::connect(this->ui.actionOpen,    &QAction::triggered, this, QOverload<>::of(&ReachVariantTool::openFile));
   QObject::connect(this->ui.actionSave,    &QAction::triggered, this, &ReachVariantTool::saveFile);
   QObject::connect(this->ui.actionSaveAs,  &QAction::triggered, this, &ReachVariantTool::saveFileAs);
   QObject::connect(this->ui.actionOptions, &QAction::triggered, &ProgramOptionsDialog::get(), &ProgramOptionsDialog::open);
   QObject::connect(this->ui.actionEditScript, &QAction::triggered, [this]() {
      (new MegaloScriptEditorWindow(this))->exec();
   });
   #if _DEBUG
      QObject::connect(this->ui.actionDebugMisc, &QAction::triggered, [this]() {
         //
         // if I need to quickly test something, I can just throw it in here
         //
         QMessageBox::information(this, tr("No test currently set up"), tr("..."));
      });
   #else
      this->ui.actionDebugMisc->setEnabled(false);
      this->ui.actionDebugMisc->setVisible(false);
   #endif
   #if _DEBUG
      QObject::connect(this->ui.actionDebugbreak, &QAction::triggered, DebugHelperFunctions::break_on_variant);
      QObject::connect(this->ui.actionDebugExportTriggersText, &QAction::triggered, [this]() { DebugHelperFunctions::export_variant_triggers_english(this); });
      QObject::connect(this->ui.actionDebugExportStringsText, &QAction::triggered, [this]() { DebugHelperFunctions::export_variant_strings(this); });
   #else
      this->ui.actionDebugbreak->setEnabled(false);
      this->ui.actionDebugbreak->setVisible(false);
      this->ui.actionDebugExportTriggersText->setEnabled(false);
      this->ui.actionDebugExportTriggersText->setVisible(false);
      this->ui.actionDebugExportStringsText->setEnabled(false);
      this->ui.actionDebugExportStringsText->setVisible(false);
   #endif
   {
      auto path = QDir(QApplication::applicationDirPath()).currentPath() + "/help/"; // ughhhhh
      auto dir  = QDir(path);
      if (!dir.exists()) {
         this->ui.actionHelpWeb->setEnabled(false);
         this->ui.actionHelpWeb->setToolTip(tr("The documentation folder is missing!", ""));
         this->ui.actionHelpFolder->setEnabled(false);
         this->ui.actionHelpFolder->setToolTip(tr("The documentation folder is missing!", ""));
      }
      QObject::connect(this->ui.actionHelpWeb, &QAction::triggered, [this]() {
         auto path = QDir(QApplication::applicationDirPath()).currentPath() + "/help/index.html"; // gotta do weird stuff to normalize the application path ughhhhh
         if (!QDesktopServices::openUrl(QString("file:///") + path)) {
            QMessageBox::critical(this, "Error", QString("Unable to open the documentation. We apologize for the inconvenience."));
            return;
         }
      });
      QObject::connect(this->ui.actionHelpFolder, &QAction::triggered, [this]() {
         auto path = QDir(QApplication::applicationDirPath()).currentPath() + "/help/"; // gotta do weird stuff to normalize the application path ughhhhh
         if (!QDesktopServices::openUrl(QString("file:///") + path))
            QMessageBox::critical(this, "Error", QString("Unable to open the documentation. We apologize for the inconvenience."));
      });
   }
   //
   this->ui.MainContentView->setCurrentIndex(0); // Qt Designer makes the last page you were looking at in the editor the default page; let's just switch to the first page here
   QObject::connect(this->ui.MainTreeview, &QTreeWidget::currentItemChanged, this, &ReachVariantTool::onSelectedPageChanged);
   //
   QObject::connect(this->ui.pageContentMetadata, &PageMPMetadata::titleChanged, [this](const char16_t* title) {
      if (ReachINI::UIWindowTitle::bShowVariantTitle.current.b)
         this->refreshWindowTitle();
   });
   this->regenerateNavigation();
   //
   this->setAcceptDrops(true);
}

void ReachVariantTool::getDefaultLoadDirectory(QString& out) const noexcept {
   using dir_type = ReachINI::DefaultPathType;
   out.clear();
   switch ((dir_type)ReachINI::DefaultLoadPath::uPathType.current.u) {
      case dir_type::custom:
         out = QString::fromUtf8(ReachINI::DefaultLoadPath::sCustomPath.currentStr.c_str());
         return;
      case dir_type::mcc_built_in_content:
         out = this->dirBuiltInVariants;
         return;
      case dir_type::current_working_directory:
         out = "";
         return;
      case dir_type::mcc_saved_content:
         out = this->dirSavedVariants;
         return;
      default:
      case dir_type::mcc_matchmaking_content:
         out = this->dirMatchmakingVariants;
         return;
   }
}
void ReachVariantTool::getDefaultSaveDirectory(QString& out) const noexcept {
   using dir_type = ReachINI::DefaultPathType;
   out.clear();
   switch ((dir_type)ReachINI::DefaultSavePath::uPathType.current.u) {
      case dir_type::custom:
         out = QString::fromUtf8(ReachINI::DefaultSavePath::sCustomPath.currentStr.c_str());
         return;
      case dir_type::mcc_built_in_content:
         out = this->dirBuiltInVariants;
         return;
      case dir_type::mcc_matchmaking_content:
         out = this->dirMatchmakingVariants;
         return;
      case dir_type::current_working_directory:
         out = "";
         return;
      default:
      case dir_type::path_of_open_file:
         out = QString::fromWCharArray(ReachEditorState::get().variantFilePath());
         if (!out.isEmpty()) {
            if (!ReachINI::DefaultSavePath::bExcludeMCCBuiltInFolders.current.b)
               return;
            std::wstring path    = out.toStdWString();
            std::wstring prefix = this->dirBuiltInVariants.toStdWString();
            if (!cobb::path_starts_with(path, prefix)) {
               prefix = this->dirMatchmakingVariants.toStdWString();
               if (!cobb::path_starts_with(path, prefix))
                  return;
            }
            out.clear();
         }
      case dir_type::mcc_saved_content:
         out = this->dirSavedVariants;
         return;
   }
}

void ReachVariantTool::dragEnterEvent(QDragEnterEvent* event) {
   if (event->mimeData()->hasUrls())
      event->acceptProposedAction();
}
void ReachVariantTool::dropEvent(QDropEvent* event) {
   auto mime = event->mimeData();
   if (!mime)
      return;
   auto list = mime->urls();
   if (list.size() == 0)
      return;
   if (list.size() > 1) {
      QMessageBox::information(this, tr("Unable to open files"), tr("This program can only open one file at a time."));
      return;
   }
   QString file;
   for (const QUrl& url : list) {
      if (url.isLocalFile()) {
         file = url.toLocalFile();
         break;
      }
   }
   if (file.isEmpty())
      return;
   event->acceptProposedAction();
   this->openFile(file);
}

void ReachVariantTool::openFile() {
   //
   // TODO: warn on unsaved changes
   //
   QString targetDir = this->lastFileDirectory;
   if (targetDir.isEmpty())
      this->getDefaultLoadDirectory(targetDir);
   QString fileName = QFileDialog::getOpenFileName(this, tr("Open Game Variant"), targetDir, tr("Game Variant (*.bin);;All Files (*)"));
   if (!fileName.isEmpty()) {
      QDir dir;
      this->lastFileDirectory = dir.absoluteFilePath(fileName);
   }
   this->openFile(fileName);
}
void ReachVariantTool::openFile(QString fileName) {
   auto& editor = ReachEditorState::get();
   //
   // TODO: warn on unsaved changes (if not going through the open file dialog)
   //
   if (fileName.isEmpty())
      return;
   std::wstring s = fileName.toStdWString();
   auto file    = cobb::mapped_file(s.c_str());
   if (!file) {
      QString text = tr("Failed to open the file. %1");
      //
      LPVOID message;
      uint32_t size = FormatMessage(
         FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
         nullptr,
         file.get_error(),
         LANG_USER_DEFAULT,
         (LPTSTR)&message,
         0,
         nullptr
      );
      text = text.arg((LPCTSTR)message);
      LocalFree(message);
      //
      QMessageBox::information(this, tr("Unable to open file"), text);
      return;
   }
   auto variant = new GameVariant();
   if (!variant->read(file)) {
      auto& error_report = GameEngineVariantLoadError::get();
      if (error_report.state == GameEngineVariantLoadError::load_state::failure) {
         QMessageBox::information(this, tr("Unable to open file"), error_report.to_qstring());
      } else {
         QMessageBox::information(this, tr("Unable to open file"), tr("Failed to read the game variant data. The code that failed didn't signal a reason; that would be a programming mistake on my part, so let me know."));
      }
      return;
   }
   {
      auto& log   = GameEngineVariantLoadWarningLog::get();
      auto  count = log.size();
      if (count) {
         if (count > 1) {
            //
            // TODO: There were %n warnings. View them?
            //
            for (size_t i = 0; i < count; i++) {
               QMessageBox::information(this, tr("Warning"), log.warnings[i]);
            }
         } else {
            QMessageBox::information(this, tr("Warning"), log.warnings[0]);
         }
      }
   }
   editor.takeVariant(variant, s.c_str());
   {
      auto i = this->ui.MainTreeview->currentIndex();
      this->ui.MainTreeview->setCurrentItem(nullptr);
      this->ui.MainTreeview->setCurrentIndex(i); // force update of team, trait, etc., pages
   }
   this->refreshWindowTitle();
   this->ui.optionTogglesScripted->updateModelFromGameVariant();
   this->regenerateNavigation();
}
void ReachVariantTool::_saveFileImpl(bool saveAs) {
   auto& editor = ReachEditorState::get();
   if (!editor.variant()) {
      QMessageBox::information(this, tr("No game variant is open"), tr("We do not currently support creating game variants from scratch. Open a variant and then you can save an edited copy of it."));
      return;
   }
   QString fileName;
   if (saveAs) {
      QString defaultSave;
      this->getDefaultSaveDirectory(defaultSave);
      fileName = QFileDialog::getSaveFileName(
         this,
         tr("Save Game Variant"), // window title
         defaultSave, // working directory and optionally default-selected file
         tr("Game Variant (*.bin);;All Files (*)") // filetype filters
      );
      if (fileName.isEmpty())
         return;
   } else {
      fileName = QString::fromWCharArray(editor.variantFilePath());
   }
   QSaveFile file(fileName);
   if (!file.open(QIODevice::WriteOnly)) {
      QMessageBox::information(this, tr("Unable to open file for writing"), file.errorString());
      return;
   }
   //
   GameVariantSaveProcess save_process;
   editor.variant()->write(save_process);
   if (save_process.variant_is_editor_only()) {
      auto text = tr("Your game variant exceeds the limits of Halo: Reach's game variant file format. As such, the following data will have to be embedded in a non-standard way to avoid data loss:\n\n");
      if (save_process.has_flag(GameVariantSaveProcess::flag::uses_xrvt_scripts))
         text += tr(" - All script code\n");
      if (save_process.has_flag(GameVariantSaveProcess::flag::uses_xrvt_strings))
         text += tr(" - All script strings\n");
      text += tr("\nThis data is still visible to this editor; you will not lose any of your work. However, the data is invisible to the game; moreover, if you resave this file through Halo: Reach or the Master Chief Collection, the data will then be lost.\n\nDo you still wish to save this file to <%1>?").arg(fileName);
      //
      QMessageBox::StandardButton result = QMessageBox::warning(
         this,
         tr("Incomplete game variant"),
         text,
         QMessageBox::Yes | QMessageBox::No,
         QMessageBox::Yes // default button
      );
      if (result == QMessageBox::StandardButton::No) {
         file.cancelWriting();
         return;
      }
   }
   if (saveAs) {
      std::wstring temp = fileName.toStdWString();
      editor.setVariantFilePath(temp.c_str());
      this->refreshWindowTitle();
   }
   QDataStream out(&file);
   out.setVersion(QDataStream::Qt_4_5);
   out.writeRawData((const char*)save_process.writer.bytes.data(), save_process.writer.bytes.get_bytespan());
   file.commit();
}

namespace {
   QTreeWidgetItem* _makeNavItem(QTreeWidget* parent, QString s, _page p) {
      auto item = new QTreeWidgetItem(parent);
      item->setText(0, s);
      item->setData(0, Qt::ItemDataRole::UserRole, (uint)p);
      item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
      item->setExpanded(true);
      return item;
   }
   QTreeWidgetItem* _makeNavItem(QTreeWidgetItem* parent, QString s, _page p) {
      auto item = new QTreeWidgetItem(parent);
      item->setText(0, s);
      item->setData(0, Qt::ItemDataRole::UserRole, (uint)p);
      item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
      item->setExpanded(true);
      return item;
   }
   QTreeWidgetItem* _makeNavItemMPTraits(QTreeWidgetItem* parent, QString s, _traits_builtin t) {
      auto item = new QTreeWidgetItem(parent);
      item->setText(0, s);
      item->setData(0, Qt::ItemDataRole::UserRole + 0, (uint)_page::mp_traits_built_in);
      item->setData(0, Qt::ItemDataRole::UserRole + 1, (uint)t);
      item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
      return item;
   }
   QTreeWidgetItem* _makeNavItemMegaloTraits(QTreeWidgetItem* parent, QString s, uint index) {
      auto item = new QTreeWidgetItem(parent);
      item->setText(0, s);
      item->setData(0, Qt::ItemDataRole::UserRole + 0, (uint)_page::mp_traits_scripted);
      item->setData(0, Qt::ItemDataRole::UserRole + 1, index);
      item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
      return item;
   }
   QTreeWidgetItem* _makeNavItemTeamConfig(QTreeWidgetItem* parent, QString s, uint8_t teamIndex) {
      auto item = new QTreeWidgetItem(parent);
      item->setText(0, s);
      item->setData(0, Qt::ItemDataRole::UserRole + 0, (uint)_page::mp_team_configuration);
      item->setData(0, Qt::ItemDataRole::UserRole + 1, teamIndex);
      item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
      return item;
   }
}
void ReachVariantTool::regenerateNavigation() {
   auto& editor = ReachEditorState::get();
   auto  widget = this->ui.MainTreeview;
   //
   int32_t priorScriptTraits = -1; // TODO: This doesn't work so well for catching when we reorder traits
   QString priorSelection;
   if (auto item = widget->currentItem()) {
      priorSelection = item->text(0);
      auto data = item->data(0, Qt::ItemDataRole::UserRole);
      if (data.isValid() && (_page)data.toInt() == _page::mp_traits_scripted) {
         data = item->data(0, Qt::ItemDataRole::UserRole + 1);
         if (data.isValid())
            priorScriptTraits = data.toInt();
      }
   }
   QTreeWidgetItem* defaultFallback = nullptr; // if the previously-selected item is gone after we rebuild, select this instead; if this is nullptr, select the first item
   //
   {
      const QSignalBlocker blocker(widget);
      widget->clear();
      auto variant = editor.variant();
      if (!variant) {
         auto item = new QTreeWidgetItem(widget);
         item->setText(0, tr("Welcome", "main window navigation pane"));
         item->setData(0, Qt::ItemDataRole::UserRole, (uint)_page::welcome);
         widget->setCurrentItem(item);
         return;
      }
      auto mp = variant->get_multiplayer_data();
      auto ff = variant->get_firefight_data();
      if (mp) {
         defaultFallback = _makeNavItem(widget, tr("Metadata", "main window navigation pane"), _page::mp_metadata);
         {  // Options
            auto options = _makeNavItem(widget, tr("Options", "main window navigation pane"), _page::redirect_to_first_child);
            {
               _makeNavItem(options, tr("General Settings", "main window navigation pane"), _page::mp_options_general);
               //
               auto respawn = _makeNavItem(options, tr("Respawn Settings", "main window navigation pane"), _page::mp_options_respawn);
               _makeNavItemMPTraits(respawn, tr("Respawn Traits", "main window navigation pane"), _traits_builtin::respawn);
               //
               _makeNavItem(options, tr("Social Settings", "main window navigation pane"), _page::mp_options_social);
               //
               auto map = _makeNavItem(options, tr("Map and Game Settings", "main window navigation pane"), _page::mp_options_map);
               _makeNavItemMPTraits(map, tr("Base Player Traits", "main window navigation pane"), _traits_builtin::base);
               _makeNavItemMPTraits(map, tr("Red Powerup Traits", "main window navigation pane"), _traits_builtin::powerup_red);
               _makeNavItemMPTraits(map, tr("Blue Powerup Traits", "main window navigation pane"), _traits_builtin::powerup_blue);
               _makeNavItemMPTraits(map, tr("Yellow Powerup Traits", "main window navigation pane"), _traits_builtin::powerup_yellow);
               //
               auto team = _makeNavItem(options, tr("Team Settings", "main window navigation pane"), _page::mp_options_team);
               for (uint8_t i = 0; i < 8; i++) {
                  _makeNavItemTeamConfig(team, tr("Team %1", "main window navigation pane").arg(i + 1), i);
               }
               //
               auto loadout = _makeNavItem(options, tr("Loadout Settings", "main window navigation pane"), _page::mp_options_loadout);
               for (uint8_t i = 0; i < 3; i++) {
                  auto t = new QTreeWidgetItem(loadout);
                  t->setText(0, tr("Spartan Tier %1", "main window navigation pane").arg(i + 1));
                  t->setData(0, Qt::ItemDataRole::UserRole + 0, (uint)_page::loadout_palette);
                  t->setData(0, Qt::ItemDataRole::UserRole + 1, i * 2);
               }
               for (uint8_t i = 0; i < 3; i++) {
                  auto t = new QTreeWidgetItem(loadout);
                  t->setText(0, tr("Elite Tier %1", "main window navigation pane").arg(i + 1));
                  t->setData(0, Qt::ItemDataRole::UserRole + 0, (uint)_page::loadout_palette);
                  t->setData(0, Qt::ItemDataRole::UserRole + 1, i * 2 + 1);
               }
               //
               auto script = _makeNavItem(options, tr("Script-Specific Settings", "main window navigation pane"), _page::mp_options_scripted);
               if (priorScriptTraits >= 0)
                  defaultFallback = script;
               auto& t = mp->scriptData.traits;
               for (size_t i = 0; i < t.size(); i++) {
                  auto& traits = t[i];
                  ReachString* name = traits.name;
                  QString text;
                  if (name) {
                     text = QString::fromUtf8(name->get_content(reach::language::english).c_str());
                  } else {
                     text = QString("Unnamed Traits %1").arg(i);
                  }
                  auto item = _makeNavItemMegaloTraits(script, text, i);
                  ReachString* desc = traits.desc;
                  if (desc)
                     item->setToolTip(0, QString::fromUtf8(desc->get_content(reach::language::english).c_str()));
               }
            }
         }
         _makeNavItem(widget, tr("Option Visibility", "main window navigation pane"), _page::mp_option_visibility);
         _makeNavItem(widget, tr("Title Update Settings", "main window navigation pane"), _page::mp_title_update_1);
         if (mp->get_type() == ReachGameEngine::forge) {
            auto item = _makeNavItem(widget, tr("Forge Settings", "main window navigation pane"), _page::forge_options_general);
            _makeNavItemMPTraits(item, tr("Editor Traits", "main window navigation pane"), _traits_builtin::forge_editor);
         }
      } else if (ff) {
         defaultFallback = _makeNavItem(widget, tr("Metadata", "main window navigation pane"), _page::ff_metadata);
         _makeNavItem(widget, tr("Scenario Settings", "main window navigation pane"), _page::ff_scenario);
         //
         // TODO
         //
      } else {
         auto item = _makeNavItem(widget, tr("Welcome", "main window navigation pane"), _page::unknown_variant_type);
         widget->setCurrentItem(item);
         return;
      }
   }
   //
   // If the item we had selected before still exists, select it again.
   //
   bool reselected = false;
   if (priorScriptTraits >= 0) {
      auto item = this->getNavItemForScriptTraits(nullptr, priorScriptTraits);
      if (item) {
         widget->setCurrentItem(item);
         reselected = true;
      }
   } else if (!priorSelection.isEmpty()) {
      auto list = widget->findItems(priorSelection, Qt::MatchFixedString | Qt::MatchCaseSensitive | Qt::MatchRecursive);
      if (list.size()) {
         widget->setCurrentItem(list[0]);
         reselected = true;
      }
   }
   if (!reselected) {
      if (defaultFallback)
         widget->setCurrentItem(defaultFallback);
      else {
         auto root = widget->invisibleRootItem();
         if (root->childCount())
            widget->setCurrentItem(root->child(0));
      }
   }
}

void ReachVariantTool::onSelectedPageChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous) {
   auto widget     = this->ui.MainTreeview;
   auto selections = widget->selectedItems();
   if (!selections.size())
      return;
   auto sel     = current;
   auto stack   = this->ui.MainContentView;
   auto variant = ReachEditorState::get().variant();
   auto mp_data = variant ? variant->multiplayer.data->as_multiplayer() : nullptr;
   if (uint d = sel->data(0, Qt::ItemDataRole::UserRole).toInt()) {
      uint extra = sel->data(0, Qt::ItemDataRole::UserRole + 1).toInt();
      switch ((_page)d) {
         case _page::redirect_to_first_child:
            if (!sel->childCount()) {
               if (auto parent = sel->parent()) {
                  widget->setCurrentItem(parent);
                  return;
               }
               return;
            }
            widget->setCurrentItem(sel->child(0));
            return;
         case _page::welcome: // only shows when no file is loaded
            stack->setCurrentWidget(this->ui.PageWelcome);
            return;
         case _page::unknown_variant_type: // only shows when a file of an unknown type is (somehow) loaded
            stack->setCurrentWidget(this->ui.PageMessageForUnknownType);
            return;
         case _page::mp_metadata:
            stack->setCurrentWidget(this->ui.PageGameVariantMPMetadata);
            return;
         case _page::mp_options_general:
            stack->setCurrentWidget(this->ui.PageOptionsGeneral);
            return;
         case _page::mp_options_respawn:
            stack->setCurrentWidget(this->ui.PageOptionsRespawn);
            return;
         case _page::mp_options_social:
            stack->setCurrentWidget(this->ui.PageOptionsSocial);
            return;
         case _page::mp_options_map:
            stack->setCurrentWidget(this->ui.PageOptionsMap);
            return;
         case _page::mp_options_team:
            stack->setCurrentWidget(this->ui.PageOptionsTeam);
            return;
         case _page::mp_options_loadout:
            stack->setCurrentWidget(this->ui.PageOptionsLoadout);
            return;
         case _page::mp_options_scripted:
            stack->setCurrentWidget(this->ui.PageOptionsScripted);
            return;
         case _page::loadout_palette:
            if (!mp_data || extra >= mp_data->options.loadouts.palettes.size()) {
               const QSignalBlocker blocker(widget);
               widget->setCurrentItem(previous);
               return;
            }
            this->switchToLoadoutPalette(&mp_data->options.loadouts.palettes[extra]);
            return;
         case _page::mp_traits_built_in:
            if (!mp_data) {
               const QSignalBlocker blocker(widget);
               widget->setCurrentItem(previous);
               return;
            }
            switch ((_traits_builtin)extra) {
               case _traits_builtin::base:
                  this->switchToPlayerTraits(&mp_data->options.map.baseTraits);
                  return;
               case _traits_builtin::respawn:
                  this->switchToPlayerTraits(&mp_data->options.respawn.traits);
                  return;
               case _traits_builtin::powerup_red:
                  this->switchToPlayerTraits(&mp_data->options.map.powerups.red.traits);
                  return;
               case _traits_builtin::powerup_blue:
                  this->switchToPlayerTraits(&mp_data->options.map.powerups.blue.traits);
                  return;
               case _traits_builtin::powerup_yellow:
                  this->switchToPlayerTraits(&mp_data->options.map.powerups.yellow.traits);
                  return;
               case _traits_builtin::forge_editor:
                  this->switchToPlayerTraits(&mp_data->forgeData.editorTraits);
                  return;
            }
            {  // Unknown trait set; disallow selection.
               const QSignalBlocker blocker(widget);
               widget->setCurrentItem(previous);
               return;
            }
            break;
         case _page::mp_traits_scripted:
            if (!mp_data || extra >= mp_data->scriptData.traits.size()) {
               const QSignalBlocker blocker(widget);
               widget->setCurrentItem(previous);
               return;
            }
            this->switchToPlayerTraits(&mp_data->scriptData.traits[extra]);
            return;
         case _page::mp_team_configuration: // specific team
            ReachEditorState::get().setCurrentMultiplayerTeam(extra);
            this->ui.MainContentView->setCurrentWidget(this->ui.PageSpecificTeamConfig);
            return;
         case _page::mp_option_visibility:
            stack->setCurrentWidget(this->ui.PageOptionToggles);
            return;
         case _page::mp_title_update_1:
            stack->setCurrentWidget(this->ui.PageTitleUpdateConfig);
            return;
         case _page::forge_options_general:
            stack->setCurrentWidget(this->ui.PageForge);
            return;
         case _page::ff_metadata:
            stack->setCurrentWidget(this->ui.PageGameVariantFFMetadata);
            return;
         case _page::ff_scenario:
            stack->setCurrentWidget(this->ui.PageFirefightScenario);
            return;
      }
   }
}
void ReachVariantTool::switchToLoadoutPalette(ReachLoadoutPalette* palette) {
   ReachEditorState::get().setCurrentLoadoutPalette(palette);
   this->ui.MainContentView->setCurrentWidget(this->ui.PageLoadoutPalette);
}
void ReachVariantTool::switchToPlayerTraits(ReachPlayerTraits* traits) {
   ReachEditorState::get().setCurrentPlayerTraits(traits);
   this->ui.MainContentView->setCurrentWidget(this->ui.PageEditPlayerTraits);
}
void ReachVariantTool::refreshWindowTitle() {
   auto& editor = ReachEditorState::get();
   if (!editor.variant()) {
      this->setWindowTitle("ReachVariantTool");
      return;
   }
   std::wstring file = editor.variantFilePath();
   if (ReachINI::UIWindowTitle::bShowFullPath.current.b == false) {
      file = std::filesystem::path(file).filename().wstring();
   }
   if (ReachINI::UIWindowTitle::bShowVariantTitle.current.b == true) {
      QString variantTitle;
      //
      auto mp = editor.multiplayerData();
      if (mp) {
         //
         // Prefer getting the title from the mpvr block when possible, because it's always 
         // big-endian there. The endianness is inconsistent in chdr (old modded variants 
         // packaged for 360 may use little-endian) and the block header doesn't have any 
         // data that could clue us into when it's little-endian (i.e. the version and flags 
         // are the same as in big-endian MCC-era blocks).
         //
         variantTitle = QString::fromUtf16(mp->variantHeader.title);
      } else {
         variantTitle = QString::fromUtf16(editor.variant()->contentHeader.data.title);
      }
      this->setWindowTitle(
         QString("%1 <%2> - ReachVariantTool").arg(variantTitle).arg(file)
      );
   } else {
      this->setWindowTitle(
         QString("%1 - ReachVariantTool").arg(file)
      );
   }
}

QTreeWidgetItem* ReachVariantTool::getNavItemForScriptTraits(ReachMegaloPlayerTraits* traits, int32_t index) {
   if (index < 0) {
      if (!traits)
         return nullptr;
      index = traits->index;
   }
   //
   auto widget = this->ui.MainTreeview;
   QTreeWidgetItem* container = cobb::qt::tree_widget::find_item_by_data(widget, [](QVariant data) { return (_page)data.toInt() == _page::mp_options_scripted; }, Qt::ItemDataRole::UserRole);
   if (!container)
      return nullptr;
   auto size = container->childCount();
   for (size_t i = 0; i < size; i++) {
      auto item = container->child(i);
      if (!item)
         continue;
      auto data = item->data(0, Qt::ItemDataRole::UserRole + 0);
      if (!data.isValid())
         continue;
      if ((_page)data.toInt() != _page::mp_traits_scripted)
         continue;
      data = item->data(0, Qt::ItemDataRole::UserRole + 1);
      if (!data.isValid())
         continue;
      if (data.toInt() == index) {
         return item;
      }
   }
   return nullptr;
}