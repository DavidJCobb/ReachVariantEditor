#include "./cli.h"
#include <cstdio>
#include <print>
#include <string_view>
#include "./rvt_version_macros.h"

namespace rvt {
   command_line_params::command_line_params(int argc, char* argv[]) {
      if (argc <= 1)
         return;

      std::string_view operation = argv[1];
      if (operation == "--help" || operation == "/?") {
         this->help = true;
         return;
      }

      if (operation == "--headless") {
         this->headless = true;

         //
         // Currently supported commands:
         // 
         // <rvt-exe> --headless <in-variant> --recompile <in-source>
         // <rvt-exe> --headless <in-variant> --recompile <in-source> --dst <out-variant>
         //

         if (argc < 3) {
            this->has_error = true;
            std::println(stderr, "To run RVT headlessly, you must specify a game variant file to modify.");
            return;
         }
         this->game_variant.input = argv[2];

         for (int i = 3; i < argc; ++i) {
            std::string_view arg      = argv[i];
            bool             has_next = i + 1 < argc;
            if (arg == "--recompile") {
               this->operation = headless_operation::recompile;
               if (!has_next) {
                  this->has_error = true;
                  std::println(stderr, "Expected a file path (to a file with Megalo code) after `--recompile`.");
                  return;
               }
               this->megalo_source = argv[i + 1];
               ++i;
               continue;
            }
            if (arg == "--dst") {
               if (!has_next) {
                  this->has_error = true;
                  std::println(stderr, "Expected a file path (to save the modified game variant to) after `--dst`.");
                  return;
               }
               this->game_variant.output = argv[i + 1];
               ++i;
               continue;
            }
            this->has_error = true;
            std::println(stderr, "Unrecognized command line parameter; stopping. Parameter was: `{}`", arg);
            return;
         }
      }
   }

   extern void print_cli_help() {
      std::println("ReachVariantTool {}.{}.{}.{}", VER_MAJOR, VER_MINOR, VER_PATCH, VER_BUILD);
      std::println("Available commands:");
      std::println("--headless <in-variant> --recompile <in-source>");
      std::println(
         "   Loads the game variant at the <in-variant> file path. Loads the Megalo \n"
         "   script source code at the <in-source> file path. Attempts to recompile \n"
         "   the game variant's script; if successful, overwrites the game variant.\n"
         "\n"
      );
      std::println("--headless <in-variant> --recompile <in-source> --dst <out-variant>");
      std::println(
         "   Same as above, but the recompiled game variant is saved to <out-variant> \n"
         "   (if and only if recompilation succeeds).\n"
         "\n"
      );
   }
}