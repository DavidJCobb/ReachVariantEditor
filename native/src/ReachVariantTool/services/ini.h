#pragma once
#include "../helpers/ini.h"

namespace ReachINI {
   extern cobb::ini::file& get();

   // INI SETTING DEFINITIONS.
   //
   // The macro used here intentionally differs from the one used in the H file. Keep the namespaces 
   // synchronized between the two files.
   //
   #define REACHTOOL_MAKE_INI_SETTING(name, category, value) extern cobb::ini::setting name;
   namespace UIWindowTitle {
      REACHTOOL_MAKE_INI_SETTING(bShowFullPath,     "UIWindowTitle", true);
      REACHTOOL_MAKE_INI_SETTING(bShowVariantTitle, "UIWindowTitle", true);
   }
   #undef REACHTOOL_MAKE_INI_SETTING
}