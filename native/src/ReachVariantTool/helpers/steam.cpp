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
#include "steam.h"
#include "strings.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <Windows.h>
#include "intrusive_windows_defines.h"

namespace {
   constexpr char* install_directory_key = "installdir";
   bool _install_directory_from_acf(uint32_t appID, const std::wstring& acf_folder_path, std::wstring& out) {
      std::wstring acf = acf_folder_path + L"/appmanifest_";
      acf += std::to_wstring(appID);
      acf += L".acf";
      //
      std::ifstream file(acf);
      if (!file)
         return false;
      std::string line;
      while (std::getline(file, line)) {
         //
         // Very quick-and-dirty code for reading the ACF files; doesn't pay attention to 
         // hierarchy or such.
         //
         size_t size = line.size();
         size_t i    = 0;
         for (; i < size; i++)
            if (!isspace(line[i]))
               break;
         if (i >= size)
            continue;
         bool hasQuote = false;
         if (line[i] == '"') {
            hasQuote = true;
            ++i;
            if (i >= size)
               continue;
         }
         if (_strnicmp(line.data() + i, install_directory_key, cobb::strlen(install_directory_key)) != 0)
            continue;
         i += cobb::strlen(install_directory_key);
         if (hasQuote)
            ++i;
         for (; i < size; i++)
            if (!isspace(line[i]))
               break;
         if (i >= size)
            continue;
         hasQuote = line[i] == '"';
         if (hasQuote)
            ++i;
         out = acf_folder_path;
         out += L"/common/";
         for (; i < size; i++) {
            if (line[i] == '"')
               break;
            out += line[i];
         }
         return true;
      }
      return false;
   }
}
namespace cobb {
   namespace steam {
      bool get_steam_directory(std::wstring& out) {
         HKEY    key;
         LSTATUS status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam\\", 0, KEY_WOW64_32KEY | KEY_QUERY_VALUE, &key);
         if (status == ERROR_SUCCESS) {
            DWORD valueType = REG_SZ;
            //
            TCHAR buffer[1024];
            DWORD length = sizeof(buffer);
            status = RegQueryValueEx(key, L"InstallPath", 0, &valueType, (LPBYTE)buffer, &length);
            //while (status == ERROR_MORE_DATA) { // buffer not long enough
            //}
            if (status == ERROR_SUCCESS) {
               if (valueType != REG_SZ)
                  status = ERROR_FILE_NOT_FOUND;
               else
                  out = buffer;
            }
         }
         RegCloseKey(key);
         return status == ERROR_SUCCESS;
      }
      bool get_game_directory(uint32_t appID, std::wstring& out) {
         std::wstring steam;
         if (!get_steam_directory(steam))
            return false;
         steam += L"/steamapps/";
         if (_install_directory_from_acf(appID, steam, out))
            return true;
         steam += L"/libraryfolders.vdf";
         std::ifstream file(steam);
         if (!file)
            return false;
         std::vector<std::wstring> libraries;
         std::string line;
         bool found = false;
         while (std::getline(file, line)) {
            //
            // Very quick-and-dirty code for reading the VDF file; doesn't pay attention to 
            // hierarchy or such.
            //
            size_t size = line.size();
            size_t i = 0;
            for (; i < size; i++)
               if (!isspace(line[i]))
                  break;
            if (i >= size)
               continue;
            bool hasQuote = false;
            if (line[i] == '"') {
               hasQuote = true;
               ++i;
               if (i >= size)
                  continue;
            }
            char* end = nullptr;
            auto index = strtoul(line.data() + i, &end, 10);
            if ((index == 0 && errno) || index == UINT_MAX)
               continue;
            if (!end)
               continue;
            i = end - line.data();
            if (hasQuote)
               ++i;
            for (; i < size; i++)
               if (!isspace(line[i]))
                  break;
            if (i >= size)
               continue;
            hasQuote = line[i] == '"';
            if (hasQuote)
               ++i;
            std::wstring working = L"";
            for (; i < size; i++) {
               if (line[i] == '"')
                  break;
               working += line[i];
            }
            working += L"/steamapps/";
            libraries.push_back(working);
            found = true;
            break;
         }
         if (!found)
            return false;
         for (auto& path : libraries) {
            if (_install_directory_from_acf(appID, path, out)) {
               std::filesystem::directory_entry dir(out);
               return dir.exists();
            }
         }
         return false;
      }
   }
}