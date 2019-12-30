#include "ReachVariantTool.h"
#include <cassert>
#include <filesystem>
#include <QColorDialog>
#include <QDateTime>
#include <QFileDialog>
#include <QGridLayout>
#include <QMessageBox>
#include <QSlider>
#include <QString>
#include <QTreeWidget>
#include <QWidget>
#include "editor_state.h"
#include "game_variants/base.h"
#include "game_variants/errors.h"
#include "helpers/ini.h"
#include "helpers/stream.h"
#include "services/ini.h"
#include "ui/generic/QXBLGamertagValidator.h"

#include "ProgramOptionsDialog.h"

namespace {
   ReachVariantTool* _window = nullptr;

   QColor _colorFromReach(uint32_t c) {
      return QColor((c >> 0x10) & 0xFF, (c >> 0x08) & 0xFF, c & 0xFF);
   }
   uint32_t _colorToReach(const QColor& c) {
      return ((c.red() & 0xFF) << 0x10) | ((c.green() & 0xFF) << 0x08) | (c.blue() & 0xFF);
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
   ReachINI::get().register_for_changes([](cobb::ini::setting* setting, cobb::ini::setting_value_union oldValue, cobb::ini::setting_value_union newValue) {
      if (setting == &ReachINI::UIWindowTitle::bShowFullPath || setting == &ReachINI::UIWindowTitle::bShowVariantTitle) {
         ReachVariantTool::get().refreshWindowTitle();
         return;
      }
   });
   //
   QObject::connect(this->ui.actionOpen,    &QAction::triggered, this, &ReachVariantTool::openFile);
   QObject::connect(this->ui.actionSave,    &QAction::triggered, this, &ReachVariantTool::saveFile);
   QObject::connect(this->ui.actionSaveAs,  &QAction::triggered, this, &ReachVariantTool::saveFileAs);
   QObject::connect(this->ui.actionOptions, &QAction::triggered, &ProgramOptionsDialog::get(), &ProgramOptionsDialog::open);
   #if _DEBUG
      QObject::connect(this->ui.actionDebugbreak, &QAction::triggered, []() {
         auto variant = ReachEditorState::get().variant();
         __debugbreak();
      });
   #else
      this->ui.actionDebugbreak->setEnabled(false);
      this->ui.actionDebugbreak->setVisible(false);
   #endif
   //
   this->ui.MainContentView->setCurrentIndex(0); // Qt Designer makes the last page you were looking at in the editor the default page; let's just switch to the first page here
   QObject::connect(this->ui.MainTreeview, &QTreeWidget::itemSelectionChanged, this, &ReachVariantTool::onSelectedPageChanged);
   //
   QObject::connect(this->ui.pageContentMetadata, &PageMPMetadata::titleChanged, [this](const char16_t* title) {
      if (ReachINI::UIWindowTitle::bShowVariantTitle.current.b)
         this->refreshWindowTitle();
   });
   {  // Specific Team Settings: Add each team to the navigation
      auto tree = this->ui.MainTreeview;
      QTreeWidgetItem* branch;
      {
         auto list = tree->findItems(tr("Team Settings", "MainTreeview"), Qt::MatchRecursive, 0);
         if (!list.size())
            return;
         branch = list[0];
      }
      for (QTreeWidgetItem* child : branch->takeChildren())
         delete child;
      for (size_t i = 0; i < 8; i++) {
         auto item = new QTreeWidgetItem(branch, QTreeWidgetItem::UserType + i);
         item->setText(0, QString("Team %1").arg(i + 1));
      }
   }
}

void ReachVariantTool::openFile() {
   auto& editor = ReachEditorState::get();
   //
   // TODO: warn on unsaved changes
   //
   QString fileName = QFileDialog::getOpenFileName(this, tr("Open Game Variant"), "", tr("Game Variant (*.bin);;All Files (*)"));
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
   editor.takeVariant(variant, s.c_str());
   this->refreshScriptedPlayerTraitList();
   this->setupWidgetsForScriptedOptions();
   {
      auto i = this->ui.MainTreeview->currentIndex();
      this->ui.MainTreeview->setCurrentItem(nullptr);
      this->ui.MainTreeview->setCurrentIndex(i); // force update of team, trait, etc., pages
   }
   this->refreshWindowTitle();
   this->ui.optionTogglesScripted->updateModelFromGameVariant();
}
void ReachVariantTool::_saveFileImpl(bool saveAs) {
   auto& editor = ReachEditorState::get();
   if (!editor.variant()) {
      QMessageBox::information(this, tr("No game variant is open"), tr("We do not currently support creating game variants from scratch. Open a variant and then you can save an edited copy of it."));
      return;
   }
   QString fileName;
   if (saveAs) {
      fileName = QFileDialog::getSaveFileName(
         this,
         tr("Save Game Variant"), // window title
         QString::fromWCharArray(editor.variantFilePath()), // working directory and optionally default-selected file
         tr("Game Variant (*.bin);;All Files (*)") // filetype filters
      );
      if (fileName.isEmpty())
         return;
   } else {
      fileName = QString::fromWCharArray(editor.variantFilePath());
   }
   QFile file(fileName);
   if (!file.open(QIODevice::WriteOnly)) {
      QMessageBox::information(this, tr("Unable to open file for writing"), file.errorString());
      return;
   }
   if (saveAs) {
      std::wstring temp = fileName.toStdWString();
      editor.setVariantFilePath(temp.c_str());
      this->refreshWindowTitle();
   }
   QDataStream out(&file);
   out.setVersion(QDataStream::Qt_4_5);
   //
   cobb::bit_or_byte_writer writer;
   editor.variant()->write(writer);
   out.writeRawData((const char*)writer.bytes.data(), writer.bytes.get_bytespan());
}
void ReachVariantTool::onSelectedPageChanged() {
   auto widget     = this->ui.MainTreeview;
   auto selections = widget->selectedItems();
   if (!selections.size())
      return;
   auto sel     = selections[0];
   auto text    = sel->text(0);
   auto stack   = this->ui.MainContentView;
   auto variant = ReachEditorState::get().variant();
   auto mp_data = variant ? variant->multiplayer.data->as_multiplayer() : nullptr;
   if (text == tr("Metadata", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageGameVariantHeader);
      return;
   }
   if (text == tr("General Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsGeneral);
      return;
   }
   if (text == tr("Respawn Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsRespawn);
      return;
   }
   if (text == tr("Social Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsSocial);
      return;
   }
   if (text == tr("Map and Game Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsMap);
      return;
   }
   if (text == tr("Team Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsTeam);
      return;
   }
   if (text == tr("Script-Specific Options", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsScripted);
      return;
   }
   if (text == tr("Loadout Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionsLoadout);
      return;
   }
   if (variant && mp_data) { // traits; loadout palettes; scripted traits; teams
      if (auto p = sel->parent()) {
         const auto text = p->text(0);
         if (text == tr("Script-Specific Options", "MainTreeview")) {
            auto t = sel->type();
            if (t >= QTreeWidgetItem::UserType) {
               size_t index = t - QTreeWidgetItem::UserType;
               this->switchToPlayerTraits(&mp_data->scriptData.traits[index]);
               return;
            }
         }
      }
      if (auto p = sel->parent()) {
         const auto text = p->text(0);
         if (text == tr("Team Settings", "MainTreeview")) {
            auto t = sel->type();
            if (t >= QTreeWidgetItem::UserType) {
               size_t index = t - QTreeWidgetItem::UserType;
               ReachEditorState::get().setCurrentMultiplayerTeam(index);
               this->ui.MainContentView->setCurrentWidget(this->ui.PageSpecificTeamConfig);
               return;
            }
         }
      }
      //
      if (text == tr("Respawn Traits", "MainTreeview")) {
         this->switchToPlayerTraits(&mp_data->options.respawn.traits);
         return;
      }
      if (text == tr("Base Player Traits", "MainTreeview")) {
         this->switchToPlayerTraits(&mp_data->options.map.baseTraits);
         return;
      }
      if (text == tr("Red Powerup Traits", "MainTreeview")) {
         this->switchToPlayerTraits(&mp_data->options.map.powerups.red.traits);
         return;
      }
      if (text == tr("Blue Powerup Traits", "MainTreeview")) {
         this->switchToPlayerTraits(&mp_data->options.map.powerups.blue.traits);
         return;
      }
      if (text == tr("Custom Powerup Traits", "MainTreeview")) {
         this->switchToPlayerTraits(&mp_data->options.map.powerups.yellow.traits);
         return;
      }
      if (text == tr("Editor Traits", "MainTreeview")) {
         this->switchToPlayerTraits(&mp_data->forgeData.editorTraits);
         return;
      }
      //
      const QString paletteNames[] = {
         tr("Spartan Tier 1", "MainTreeview"),
         tr("Spartan Tier 2", "MainTreeview"),
         tr("Spartan Tier 3", "MainTreeview"),
         tr("Elite Tier 1", "MainTreeview"),
         tr("Elite Tier 2", "MainTreeview"),
         tr("Elite Tier 3", "MainTreeview"),
      };
      for (uint8_t i = 0; i < std::extent<decltype(paletteNames)>::value; i++) {
         if (text == paletteNames[i]) {
            this->switchToLoadoutPalette(&mp_data->options.loadouts.palettes[i]);
            return;
         }
      }
   }
   if (text == tr("Option Visibility", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageOptionToggles);
      return;
   }
   if (text == tr("Title Update Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageTitleUpdateConfig);
      return;
   }
   if (text == tr("Forge Settings", "MainTreeview")) {
      stack->setCurrentWidget(this->ui.PageForge);
      return;
   }
}
void ReachVariantTool::switchToLoadoutPalette(ReachLoadoutPalette* palette) {
   ReachEditorState::get().setCurrentLoadoutPalette(palette);
   this->refreshWidgetsForLoadoutPalette();
   this->ui.MainContentView->setCurrentWidget(this->ui.PageLoadoutPalette);
}
void ReachVariantTool::switchToPlayerTraits(ReachPlayerTraits* traits) {
   ReachEditorState::get().setCurrentPlayerTraits(traits);
   this->ui.MainContentView->setCurrentWidget(this->ui.PageEditPlayerTraits);
}
void ReachVariantTool::refreshWidgetsForLoadoutPalette() {
   this->ui.loadout1Content->pullFromGameVariant();
   this->ui.loadout2Content->pullFromGameVariant();
   this->ui.loadout3Content->pullFromGameVariant();
   this->ui.loadout4Content->pullFromGameVariant();
   this->ui.loadout5Content->pullFromGameVariant();
}
void ReachVariantTool::refreshScriptedPlayerTraitList() {
   auto tree   = this->ui.MainTreeview;
   QTreeWidgetItem* branch;
   {
      auto list = tree->findItems(tr("Script-Specific Options", "MainTreeview"), Qt::MatchRecursive, 0);
      if (!list.size())
         return;
      branch = list[0];
   }
   for (QTreeWidgetItem* child : branch->takeChildren())
      delete child;
   auto data = ReachEditorState::get().multiplayerData();
   if (!data)
      return;
   auto& t = data->scriptData.traits;
   for (size_t i = 0; i < t.size(); i++) {
      auto item = new QTreeWidgetItem(branch, QTreeWidgetItem::UserType + i);
      auto name = t[i].name;
      QString text;
      if (name) {
         text = QString::fromUtf8(name->english().c_str());
      } else {
         text = QString("Unnamed Traits %1").arg(i);
      }
      item->setText(0, text);
   }
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
//
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
   void _onMegaloSliderChange(QSlider* widget, int value) {
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
void ReachVariantTool::setupWidgetsForScriptedOptions() {
   auto page   = this->ui.PageOptionsScripted;
   auto layout = dynamic_cast<QGridLayout*>(page->layout());
   if (!layout) {
      layout = new QGridLayout;
      page->setLayout(layout);
   }
   {  // Clear layout
      QLayoutItem* child;
      while ((child = layout->takeAt(0)) != nullptr)
         delete child;
   }
   auto data = ReachEditorState::get().multiplayerData();
   if (!data)
      return;
   const auto& options = data->scriptData.options;
   for (uint32_t i = 0; i < options.size(); i++) {
      auto& option = options[i];
      auto  desc   = option.desc;
      //
      auto label = new QLabel(this);
      label->setProperty("MegaloOptionIndex", i);
      if (option.name) {
         label->setText(QString::fromUtf8(option.name->english().c_str()));
      } else {
         label->setText(QString("Unnamed Option #%1").arg(i + 1));
      }
      layout->addWidget(label, i, 0);
      if (desc)
         label->setToolTip(QString::fromUtf8(desc->english().c_str()));
      //
      if (option.isRange) {
         auto slider = new QSlider(Qt::Horizontal, this);
         slider->setProperty("MegaloOptionIndex", i);
         slider->setMinimum(option.rangeMin.value);
         slider->setMaximum(option.rangeMax.value);
         slider->setValue(option.rangeCurrent);
         layout->addWidget(slider, i, 1);
         //
         // TODO: display default value?
         //
         QObject::connect(slider, QOverload<int>::of(&QSlider::valueChanged), [slider](int v) { _onMegaloSliderChange(slider, v); });
         if (desc)
            slider->setToolTip(QString::fromUtf8(desc->english().c_str()));
         label->setBuddy(slider);
      } else {
         auto combo = new QComboBox(this);
         combo->setProperty("MegaloOptionIndex", i);
         for (uint32_t j = 0; j < option.values.size(); j++) {
            auto& value = option.values[j];
            if (value.name) {
               combo->addItem(QString::fromUtf8(value.name->english().c_str()), (int16_t)value.value);
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
            combo->setToolTip(QString::fromUtf8(desc->english().c_str()));
         label->setBuddy(combo);
      }
   }
   auto spacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
   layout->addItem(spacer, options.size(), 0, 1, 2);
}
