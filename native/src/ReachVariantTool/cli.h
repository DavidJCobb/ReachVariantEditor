#pragma once
#include <filesystem>

namespace rvt {
   enum class headless_operation {
      none,
      recompile,
   };

   struct command_line_params {
      command_line_params() {}
      command_line_params(int argc, char* argv[]);

      headless_operation operation = headless_operation::none;
      bool has_error = false;
      bool headless  = false;
      bool help      = false;
      struct {
         std::filesystem::path input;
         std::filesystem::path output; // if not specified, same as input
      } game_variant;
      std::filesystem::path megalo_source;
   };

   extern void print_cli_help();
}