#pragma once
#include <cstdint>
#include <string>

namespace cobb {
   namespace steam {
      //
      // Functions to get the install directory for Steam, and the install directory for any game with 
      // a known app ID. These are quick-and-dirty, intended for non-essential uses; they parse Steam 
      // files very lazily and they return paths with both inconsistent and redundant separators (e.g. 
      // doubled slashes and whatnot), in lieu of checking whether the paths listed in the relevant 
      // Steam files have terminating slashes or not.
      //
      bool get_steam_directory(std::wstring& out);
      bool get_game_directory(uint32_t appID, std::wstring& out);
   }
}