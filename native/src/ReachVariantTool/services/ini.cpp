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
   namespace CodeEditor {
      REACHTOOL_MAKE_INI_SETTING(bOverrideBackColor,  "CodeEditor", true);
      REACHTOOL_MAKE_INI_SETTING(bOverrideTextColor,  "CodeEditor", true);
      REACHTOOL_MAKE_INI_SETTING(sBackColor,          "CodeEditor", "rgb(255, 255, 255)");
      REACHTOOL_MAKE_INI_SETTING(sTextColor,          "CodeEditor", "rgb(  0,   0,   0)");
      REACHTOOL_MAKE_INI_SETTING(sFontFamily,         "CodeEditor", "Courier New");
      REACHTOOL_MAKE_INI_SETTING(sFormatCommentLine,  "CodeEditor", "rgb( 32, 160,   8) italic");
      REACHTOOL_MAKE_INI_SETTING(sFormatCommentBlock, "CodeEditor", "rgb( 32, 160,   8) italic");
      REACHTOOL_MAKE_INI_SETTING(sFormatKeyword,      "CodeEditor", "rgb(  0,  16, 255) bold");
      REACHTOOL_MAKE_INI_SETTING(sFormatSubkeyword,   "CodeEditor", "rgb(  0, 120, 190) bold");
      REACHTOOL_MAKE_INI_SETTING(sFormatNumber,       "CodeEditor", "rgb(200, 100,   0)");
      REACHTOOL_MAKE_INI_SETTING(sFormatOperator,     "CodeEditor", "rgb(  0,   0, 128) bold");
      REACHTOOL_MAKE_INI_SETTING(sFormatStringSimple, "CodeEditor", "rgb(140, 140, 140)");
      REACHTOOL_MAKE_INI_SETTING(sFormatStringBlock,  "CodeEditor", "rgb(160,   0,  80)");
   }
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
      REACHTOOL_MAKE_INI_SETTING(sTheme, "UIWindowTitle", "themes/default.qss");
   }
   #undef REACHTOOL_MAKE_INI_SETTING
}