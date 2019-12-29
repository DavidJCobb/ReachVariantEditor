#include "ini.h"
#include <algorithm>
#include <fstream>
#include "../helpers/strings.h"

#include <windows.h>
#include "../helpers/intrusive_windows_defines.h"

namespace ReachINI {
   extern cobb::ini::file& get() {
      static cobb::ini::file instance = cobb::ini::file(L"ReachVariantTool.ini");
      return instance;
   }

   // INI SETTING DEFINITIONS.
   //
   // The macro used here intentionally differs from the one used in the H file. Keep the namespaces 
   // synchronized between the two files.
   //
   #define REACHTOOL_MAKE_INI_SETTING(name, category, value) extern cobb::ini::setting name = cobb::ini::setting(get, #name, category, value);
   namespace UIWindowTitle {
      REACHTOOL_MAKE_INI_SETTING(bShowFullPath,     "UIWindowTitle", true);
      REACHTOOL_MAKE_INI_SETTING(bShowVariantTitle, "UIWindowTitle", true);
   }
   #undef REACHTOOL_MAKE_INI_SETTING
}