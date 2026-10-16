#include "ui/main_window.h"
#include <filesystem>
#include <string_view>
#include <QtWidgets/QApplication>

#include "game_variants/base.h"
#include "helpers/endianness.h"
#include "helpers/files.h"
#include "helpers/stream.h"
#include "services/ini.h"
#include "services/RVTThemeEngine.h"
#if _DEBUG
   #include <QDebug>
   #include <QDirIterator>
#endif
#include <QDir>
#include <QStandardPaths>

#include <cstdio>
#include <print>
#include "./cli.h"
#include "./editor_state.h"
#include "./headless.h"
#if defined(Q_OS_WIN)
   #include "./attach_to_parent_console.h"
#endif
#include "./is_console_application.h"
#include "./services/crash_dumper/core.h"

static void load_inis() {
   auto legacy_path_str = QApplication::instance()->applicationDirPath();;
   auto modern_path_str = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppLocalDataLocation);
   if (modern_path_str.isEmpty()) {
      modern_path_str = legacy_path_str;
   } else {
      QDir{}.mkpath(modern_path_str);
   }

   auto legacy_path = std::filesystem::path(legacy_path_str.toStdWString()) / L"ReachVariantTool.ini";
   auto modern_path = std::filesystem::path(modern_path_str.toStdWString()) / L"ReachVariantTool.ini";

   auto& ini = ReachINI::get();
   ini.set_paths(modern_path);

   bool load_from_modern = !std::filesystem::exists(modern_path);
   if (!std::filesystem::exists(modern_path) && std::filesystem::exists(legacy_path)) {
      //
      // This is, technically, vulnerable to a race condition: files can be created or 
      // deleted between us running the checks, and us trying to load. For a quick and 
      // dirty INI update procedure, though, I don't consider that a serious risk.
      //
      ini.load(legacy_path);
      //
      // Force an immediate save, so that the file gets moved.
      //
      if (ini.save()) {
         //
         // Try and delete the old file. Doesn't matter too much if we can't.
         //
         std::error_code ec;
         std::filesystem::remove(legacy_path, ec);
      }
   } else {
      ini.load();
   }
}

static constexpr const bool headless_creates_console_if_none_to_attach_to = false;

int main(int argc, char *argv[]) {
   rvt::subsystems::crash_dumper::core::get_or_create().on_application_start(argc, argv);

   if (argc > 1) {
      rvt::command_line_params params(argc, argv);
      if (params.has_error) {
         return 1;
      }

      if constexpr (!is_console_application) {
         #if defined(Q_OS_WIN)
            //
            // If we were compiled as a Windows Application rather than a Console Application, 
            // then we won't have a console to print to by default. We can try to attach to a 
            // parent process's console; if we were started from the console, then that will 
            // work and allow us to print to that console, though `std::cin` and friends will 
            // not work. (When a console starts a non-console application, it won't wait for 
            // the application to exit before continuing.)
            // 
            switch (attach_to_parent_console()) {
               case attach_to_console_result::success:
                  break;
               case attach_to_console_result::already_have_a_console:
                  break;
               default:
                  #if _DEBUG
                     if (attach_to_new_console())
                        break;
                  #endif
                  return 1;
            }
         #endif
      }

      if (params.help) {
         rvt::print_cli_help();
         return 0;
      }
      if (params.headless) {
         if (params.operation == rvt::headless_operation::none) {
            std::println(stderr, "You didn't specify an operation to perform.");
            return 1;
         }
         if (params.game_variant.input.empty()) {
            std::println(stderr, "You didn't specify a game variant to operate on.");
            return 1;
         }

         QCoreApplication a(argc, argv);
         load_inis();
         
         auto& editor  = ReachEditorState::get();
         if (!rvt::headless::load_game_variant(params.game_variant.input)) {
            return 1;
         }

         switch (params.operation) {
            case rvt::headless_operation::recompile:
               if (!rvt::headless::recompile(params))
                  return 1;
               {
                  auto path = params.game_variant.output;
                  if (path.empty())
                     path = params.game_variant.input;
               }
               break;
         }
         if (!rvt::headless::save_game_variant(params))
            return 1;
         return 0;
      }
   }
   if constexpr (is_console_application) {
      #if defined(Q_OS_WIN)
         //
         // Windows applications must be defined to use a given "subsystem," roughly corresponding 
         // to the kind of application you're making (e.g. GUI versus console). Windows does not 
         // define a "GUI by default but headless if asked for" application type, however:
         // 
         //    https://learn.microsoft.com/en-us/cpp/build/reference/subsystem-specify-subsystem?view=msvc-170
         //    https://devblogs.microsoft.com/oldnewthing/20090101-00/?p=19643
         // 
         // We have to pick one. We can compile a console application that detaches its console 
         // if run standaline and not headlessly, or a GUI application that attaches itself to a 
         // console if it's run from one and headlessly. This codepath handles the former method, 
         // detaching ourselves from a console if we decide to spawn a GUI. (Detaching the console 
         // window will de-spawn it automatically, if nothing else is using it.)
         // 
         // This does have the potential drawback of the console window briefly being visible, if 
         // the program takes too long to get to this point (e.g. system bogged down by something).
         //
         ::FreeConsole();
      #endif
   }

   QApplication a(argc, argv);
   #if _DEBUG
      qDebug() << a.applicationVersion();
   #endif

   // Load INIs. Has to be done after QApplication instantiation so we can get the 
   // program folder path.
   load_inis();
   
   {  // Themes
      auto* engine = new RVTThemeEngine(&a);
      //
      auto& s = ReachINI::UIWindowTitle::sTheme;
      engine->changeStyleFile(QString::fromUtf8(s.currentStr.c_str()));
      {
         using namespace cobb::ini;
         QObject::connect(&ReachINI::getForQt(), &cobb::qt::ini::file::settingChanged, &a, [engine](setting* s, setting_value_union oldValue, setting_value_union newValue) {
            if (s != &ReachINI::UIWindowTitle::sTheme)
               return;
            engine->changeStyleFile(QString::fromUtf8(s->currentStr.c_str()));
         });
      }
   }
   ReachVariantTool w;
   w.show();
   return a.exec();
}
