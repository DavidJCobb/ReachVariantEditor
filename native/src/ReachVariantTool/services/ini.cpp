#include "ini.h"

namespace ReachINI {
   extern cobb::qt::ini::file& getForQt() {
      static cobb::qt::ini::file instance = cobb::qt::ini::file(L"ReachVariantTool.ini");
      return instance;
   }
   extern cobb::ini::file& get() {
      auto casted = dynamic_cast<cobb::ini::file*>(&(getForQt()));
      assert(casted != nullptr && "cobb::qt::ini::file should be able to cast to cobb::ini::file");
      return *casted;
   }

   // INI SETTING DEFINITIONS.
   //
   // The macro used here intentionally differs from the one used in the H file. Keep the namespaces 
   // synchronized between the two files.
   //
   #define REACHTOOL_MAKE_INI_SETTING(name, category, value) extern cobb::ini::setting name = cobb::ini::setting(get, #name, category, value);
   namespace DefaultLoadPath {
      REACHTOOL_MAKE_INI_SETTING(uPathType,   "DefaultLoadPath", (uint32_t)DefaultPathType::mcc_matchmaking_content);
      REACHTOOL_MAKE_INI_SETTING(sCustomPath, "DefaultLoadPath", "");
   }
   namespace DefaultSavePath {
      REACHTOOL_MAKE_INI_SETTING(uPathType,                 "DefaultSavePath",(uint32_t)DefaultPathType::path_of_open_file);
      REACHTOOL_MAKE_INI_SETTING(bExcludeMCCBuiltInFolders, "DefaultSavePath", true);
      REACHTOOL_MAKE_INI_SETTING(sCustomPath,               "DefaultSavePath", "");
   }
   namespace Editing {
      REACHTOOL_MAKE_INI_SETTING(bHideFirefightNoOps, "Editing", true);
   }
   namespace UIWindowTitle {
      REACHTOOL_MAKE_INI_SETTING(bShowFullPath,     "UIWindowTitle", true);
      REACHTOOL_MAKE_INI_SETTING(bShowVariantTitle, "UIWindowTitle", true);
   }
   #undef REACHTOOL_MAKE_INI_SETTING
}