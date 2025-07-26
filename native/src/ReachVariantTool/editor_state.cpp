#include "editor_state.h"
#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFontDatabase>
#include <QMessageBox>
#include <QProcess>
#include <QSaveFile>
#include <QTimer>
#include "helpers/steam.h"
#include "game_variants/base.h"
#include "game_variants/io_process.h"
#include "game_variants/components/loadouts.h"
#include "game_variants/components/player_traits.h"
#include "game_variants/components/teams.h"
#include "game_variants/types/firefight.h"
#include "game_variants/types/multiplayer.h"
#include "services/ini.h"

#include "editor_state/ReachStringCentralModel.h"

namespace {
   inline bool _get_mcc_directory(std::wstring& out) {
      return cobb::steam::get_game_directory(976730, out);
   }
}

ReachEditorState::ReachEditorState() {

   bool is_headless = qobject_cast<QGuiApplication*>(QApplication::instance()) == nullptr;

   QObject::connect(this, &ReachEditorState::stringModified, [this](uint32_t index) {
      auto mp = this->multiplayerData();
      if (!mp)
         return;
      ReachString* str = mp->scriptData.strings.get_entry(index);
      if (!str)
         return;
      for (auto& option : mp->scriptData.options)
         if (option.uses_string(str))
            this->scriptOptionModified(&option);
      for (auto& traits : mp->scriptData.traits)
         if (traits.uses_string(str))
            this->scriptTraitsModified(&traits);
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
   if (!is_headless) {
      std::wstring dir;
      if (cobb::steam::get_game_directory(1695793, dir)) {
         QString hrek = QString::fromWCharArray(dir.c_str());
         if (!hrek.isEmpty()) {
            hrek += "/data/ui/fonts/";

            auto id = QFontDatabase::addApplicationFont(hrek + "EurostileLTPro-Demi.otf");
            if (id > 0) {
               auto families = QFontDatabase::applicationFontFamilies(id);
               if (!families.isEmpty())
                  this->reachUIFontFamilyNames.eurostile = families[0];
            }
            id = QFontDatabase::addApplicationFont(hrek + "CHT_TNRGC__+DFYuanBold-B5.ttf");
            if (id > 0) {
               auto families = QFontDatabase::applicationFontFamilies(id);
               if (!families.isEmpty())
                  this->reachUIFontFamilyNames.tv_nord = families[0];
            }
         }
      }
   }

   // Use a single-shot timer because the constructor for ReachStringCentralModel will 
   // try to access us; that happening while we're still being constructed = bad.
   QTimer::singleShot(
      0,
      this,
      [this]() {
         this->_stringModel = new ReachStringCentralModel(this);
      }
   );
}
void ReachEditorState::abandonVariant() noexcept {
   if (this->currentVariantClone) {
      delete this->currentVariantClone;
      this->currentVariantClone = nullptr;
   }
   if (!this->currentVariant)
      return;
   auto v = this->currentVariant;
   this->currentVariant = nullptr;
   this->currentFile    = L"";
   this->currentMPTeam  = -1;
   this->currentFFWaveTraits   = nullptr;
   this->currentLoadoutPalette = nullptr;
   this->currentRespawnOptions = nullptr;
   this->currentTraits         = nullptr;
   emit switchedFFWaveTraits(nullptr);
   emit switchedLoadoutPalette(nullptr);
   emit switchedMultiplayerTeam(nullptr, -1, nullptr);
   emit switchedRespawnOptions(nullptr);
   emit switchedPlayerTraits(nullptr);
   emit variantAbandoned(v);
   delete v;
}
void ReachEditorState::setCurrentFFWaveTraits(ReachFirefightWaveTraits* traits) noexcept {
   this->currentFFWaveTraits = traits;
   emit switchedFFWaveTraits(traits);
}
void ReachEditorState::setCurrentMultiplayerTeam(int8_t index) noexcept {
   this->currentMPTeam = index;
   emit switchedMultiplayerTeam(this->currentVariant, index, this->multiplayerTeam());
}
void ReachEditorState::setCurrentLoadoutPalette(ReachLoadoutPalette* which) noexcept {
   this->currentLoadoutPalette = which;
   emit switchedLoadoutPalette(which);
}
void ReachEditorState::setCurrentPlayerTraits(ReachPlayerTraits* which) noexcept {
   this->currentTraits = which;
   emit switchedPlayerTraits(which);
}
void ReachEditorState::setCurrentRespawnOptions(ReachCGRespawnOptions* which) noexcept {
   this->currentRespawnOptions = which;
   emit switchedRespawnOptions(which);
}
void ReachEditorState::setVariantFilePath(const wchar_t* path) noexcept {
   this->currentFile = path;
   emit variantFilePathChanged(this->currentFile.c_str());
}
void ReachEditorState::takeVariant(GameVariant* other, const wchar_t* path) noexcept {
   this->abandonVariant();
   this->currentVariant = other;
   this->currentFile    = path;
   //
   //if (other)
   //   this->currentVariantClone = other->clone(); // no point until such time as we can implement comparisons quickly
   //
   emit variantAcquired(other);
   emit switchedMultiplayerTeam(this->currentVariant, this->currentMPTeam, this->multiplayerTeam());
}
//
bool ReachEditorState::saveVariant(QWidget* parent, bool saveAs) {
   auto& editor = ReachEditorState::get();
   if (!editor.variant()) {
      QMessageBox::information(parent, tr("No game variant is open"), tr("Can't save a file if there's no actual file open... Wait, what? Why did we even let you try this?"));
      return false;
   }
   bool is_megalo = editor.variant()->multiplayer.type == ReachGameEngine::multiplayer;

   QString fileName;
   if (saveAs) {
      QString allowed_formats;
      if (is_megalo) {
         allowed_formats = tr("Game Variant (*.bin);;Raw Compiled Megalo File (*.mglo);;All Files (*)");
      } else {
         allowed_formats = tr("Game Variant (*.bin);;All Files (*)");
      }

      QString defaultSave;
      this->getDefaultSaveDirectory(defaultSave);
      fileName = QFileDialog::getSaveFileName(
         parent,
         tr("Save Game Variant"), // window title
         defaultSave, // working directory and optionally default-selected file
         allowed_formats // filetype filters
      );
      if (fileName.isEmpty())
         return false;
   } else {
      fileName = QString::fromWCharArray(editor.variantFilePath());
   }
   QSaveFile file(fileName);
   if (!file.open(QIODevice::WriteOnly)) {
      QMessageBox::information(parent, tr("Unable to open file for writing"), file.errorString());
      return false;
   }
   //
   GameVariantSaveProcess save_process;
   if (is_megalo && fileName.endsWith(".mglo", Qt::CaseInsensitive)) {
      save_process.set_flag(GameVariantSaveProcess::flag::save_bare_mglo);
   }
   editor.variant()->write(save_process);
   if (save_process.variant_is_editor_only()) {

      if (save_process.has_flag(GameVariantSaveProcess::flag::save_bare_mglo)) {
         QMessageBox::StandardButton result = QMessageBox::critical(
            parent,
            tr("Error; file not saved"),
            tr(
               "Your game variant exceeds the limits of Halo: Reach's game variant file format. "
               "When saving a normal file, ReachVariantTool can embed non-standard data into the "
               "end of the file, so that you can still save and later load your file and fix "
               "things. This is not possible when saving a bare *.mglo file.\n\n"
               "<b><i>Your file has not been saved.</i></b>\n\n"
               "If you don't have time to get your game variant under the limits right now, then "
               "you should save a normal file (*.bin) so you can come back to it later."
            )
         );
         file.cancelWriting();
         return false;
      }

      auto text = tr("Your game variant exceeds the limits of Halo: Reach's game variant file format. As such, the following data will have to be embedded in a non-standard way to avoid data loss:\n\n");
      if (save_process.has_flag(GameVariantSaveProcess::flag::uses_xrvt_scripts))
         text += tr(" - All script code\n");
      if (save_process.has_flag(GameVariantSaveProcess::flag::uses_xrvt_strings))
         text += tr(" - All script strings\n");
      text += tr("\nThis data is still visible to this editor; you will not lose any of your work. However, the data is invisible to the game; moreover, if you resave this file through Halo: Reach or the Master Chief Collection, the data will then be lost.\n\nDo you still wish to save this file to <%1>?").arg(fileName);
      //
      QMessageBox::StandardButton result = QMessageBox::warning(
         parent,
         tr("Incomplete game variant"),
         text,
         QMessageBox::Yes | QMessageBox::No,
         QMessageBox::Yes // default button
      );
      if (result == QMessageBox::StandardButton::No) {
         file.cancelWriting();
         return false;
      }
   }
   if (saveAs) {
      std::wstring temp = fileName.toStdWString();
      editor.setVariantFilePath(temp.c_str());
   }
   QDataStream out(&file);
   out.setVersion(QDataStream::Qt_4_5);
   out.writeRawData((const char*)save_process.writer.bytes.data(), save_process.writer.bytes.get_bytespan());
   file.commit();
   return true;
}
//
void ReachEditorState::openHelp(QWidget* parent, bool folder) {
   if (folder) {
      auto path = QDir(QApplication::applicationDirPath()).currentPath() + "/help/"; // gotta do weird stuff to normalize the application path ughhhhh
      if (!QDesktopServices::openUrl(QString("file:///") + path))
         QMessageBox::critical(parent, "Error", QString("Unable to open the documentation. We apologize for the inconvenience."));
      return;
   }
   //
   auto path = QDir(QApplication::applicationDirPath()).currentPath() + "/help/index.html"; // gotta do weird stuff to normalize the application path ughhhhh
   if (!QDesktopServices::openUrl(QString("file:///") + path)) {
      QMessageBox::critical(parent, "Error", QString("Unable to open the documentation. We apologize for the inconvenience."));
      return;
   }
}

ReachCustomGameOptions* ReachEditorState::customGameOptions() noexcept {
   if (!this->currentVariant)
      return nullptr;
   return this->currentVariant->get_custom_game_options();
}
GameVariantDataFirefight* ReachEditorState::firefightData() noexcept {
   auto v = this->currentVariant;
   if (!v)
      return nullptr;
   return dynamic_cast<GameVariantDataFirefight*>(v->multiplayer.data);
}
GameVariantDataMultiplayer* ReachEditorState::multiplayerData() noexcept {
   auto v = this->currentVariant;
   if (!v)
      return nullptr;
   auto& m = v->multiplayer;
   if (m.data) {
      switch (m.data->get_type()) {
         case ReachGameEngine::multiplayer:
         case ReachGameEngine::forge:
            return dynamic_cast<GameVariantDataMultiplayer*>(m.data);
      }
   }
   return nullptr;
}
ReachTeamData* ReachEditorState::multiplayerTeam() noexcept {
   if (this->currentMPTeam == -1)
      return nullptr;
   auto data = this->customGameOptions();
   if (data)
      return &(data->team.teams[this->currentMPTeam]);
   return nullptr;
}

void ReachEditorState::getDefaultLoadDirectory(QString& out) const noexcept {
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
void ReachEditorState::getDefaultSaveDirectory(QString& out) const noexcept {
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


std::optional<QString> ReachEditorState::getReachEditingKitWidgetFontFamily() const {
   return this->reachUIFontFamilyNames.eurostile;
}
std::optional<QString> ReachEditorState::getReachEditingKitBodyTextFontFamily() const {
   return this->reachUIFontFamilyNames.tv_nord;
}

ReachStringCentralModel* ReachEditorState::getStringModel() {
   auto* model = this->_stringModel;
   if (!model) {
      model = this->_stringModel = new ReachStringCentralModel;
      model->refresh();
   }
   return model;
}
ReachStringDependentModel* ReachEditorState::makeFilterableStringModel() {
   auto* model = this->getStringModel();
   auto* proxy = new ReachStringDependentModel();
   proxy->setSourceModel(model);
   return proxy;
}