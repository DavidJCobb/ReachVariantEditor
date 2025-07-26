#pragma once
#include <filesystem>

namespace rvt {
   struct command_line_params;
}

namespace rvt::headless {
   // Attempt to load a game variant, and give it to ReachEditorState if successful.
   // Returns true if successful.
   extern bool load_game_variant(const std::filesystem::path&);

   // Assumes the game variant is already loaded.
   extern bool recompile(const command_line_params&);

   extern bool save_game_variant(const command_line_params&);
}