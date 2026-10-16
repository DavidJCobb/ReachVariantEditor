#pragma once

constexpr const bool is_console_application =
   //
   // In MSBuild, the Linker "Subsystem" option controls whether you're compiling 
   // a console application or a GUI application.
   // 
   // This option cannot be directly tested from source code. When you create a 
   // project in Visual Studio, you have to select the subsystem, and based on the 
   // choice you make at that time, VS will configure the project to define the 
   // appropriate macro (_CONSOLE or _WINDOWS). However, these macros may not be 
   // updated when you manually change the subsystem option. (Worse, it seems like 
   // `_WINDOWS` may be defined implicitly somewhere in Visual Studio or Qt VS 
   // Tools' supplied defaults, even if your project isn't set to define it.)
   // 
   // If neither macro is present, then we assume that the project is a console 
   // application and the macros just aren't set up.
   //
   #if defined(RVT_FORCE_CONSOLENESS_TO)
      (bool)(RVT_FORCE_CONSOLENESS_TO)
   #else
      #if _CONSOLE
         true
      #else
         #if _WINDOWS
            false
         #else
            true
         #endif
      #endif
   #endif
;