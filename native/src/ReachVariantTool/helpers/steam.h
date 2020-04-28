/*

This file is provided under the Creative Commons 0 License.
License: <https://creativecommons.org/publicdomain/zero/1.0/legalcode>
Summary: <https://creativecommons.org/publicdomain/zero/1.0/>

One-line summary: This file is public domain or the closest legal equivalent.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
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