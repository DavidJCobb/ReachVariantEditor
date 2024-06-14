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
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <vector>
#include <Windows.h>
#include "intrusive_windows_defines.h"
#include "./string/strieq_ascii.h"

namespace {
   void _file_to_string(std::ifstream& file, std::string& dst) {
      file.seekg(0, std::ios::end);
      size_t size = file.tellg();
      dst.resize(size);
      file.seekg(0);
      file.read(&dst[0], size);
   }

   class valve_reader {
      public:
         struct node {
            std::string name;
            std::optional<std::string> value;

            node* parent = nullptr;
            std::vector<node*> children;

            constexpr ~node() {
               for (auto* child : this->children)
                  delete child;
               this->children.clear();
            }
         };

      protected:
         size_t offset = 0;
         node*  target = nullptr;

      public:
         node root;
         std::string data;

      protected:
         static constexpr bool is_space(char c) {
            switch (c) {
               case ' ':
               case '\t':
               case '\r':
               case '\n':
                  return true;
            }
            return false;
         }

         constexpr bool _is_in_bounds() {
            return this->offset < this->data.size();
         }

         constexpr void _skip_whitespace() {
            while (_is_in_bounds() && is_space(this->data[this->offset]))
               ++this->offset;
         }

         constexpr std::optional<std::string> _consume_quoted() {
            _skip_whitespace();
            if (!_is_in_bounds())
               return {};
            char delim = this->data[this->offset];
            switch (delim) {
               case '"':
               case '\'':
                  break;
               default:
                  return {};
            }
            size_t end = this->data.find_first_of(delim, this->offset + 1);
            if (end == std::string::npos)
               return {};
            size_t start = this->offset;
            this->offset = end + 1;

            std::string raw = this->data.substr(start + 1, end - start - 1);
            std::string out;
            for (size_t i = 0; i < raw.size(); ++i) {
               char c = raw[i];
               if (c != '\\') {
                  out += c;
                  continue;
               }
               if (i + 1 < raw.size()) {
                  c = raw[i + 1];
                  out += c;
                  ++i; // skip
               }
            }
            return out;
         }

      public:
         constexpr bool parse() {
            this->offset = 0;
            this->root   = {};
            this->target = &this->root;

            while (_is_in_bounds()) {
               _skip_whitespace();
               if (!_is_in_bounds())
                  break;

               auto key = this->_consume_quoted();
               if (!key.has_value()) {

                  // Could be closing a struct?
                  if (this->data[this->offset] == '}') {
                     ++this->offset;
                     this->target = this->target->parent;
                     if (this->target == nullptr)
                        return true;
                     continue;
                  }

                  // Unexpected absence of a key.
                  return false;
               }

               auto* next = new node;
               this->target->children.push_back(next);
               next->parent = this->target;
               next->name = key.value();

               auto value = this->_consume_quoted();
               if (value.has_value()) {
                  next->value = value.value();
                  continue;
               }

               //_skip_whitespace(); // already done by _consume_quoted()
               if (this->data[this->offset] == '{') {
                  ++this->offset;
                  this->target = next;
                  continue;
               }
               return false;
            }
            return this->target == &this->root;
         }
         
         constexpr std::optional<std::string> vdf_library_folder_for(unsigned int appID) const {
            if (this->root.children.empty())
               return {};
            const auto* base = this->root.children[0];
            if (!cobb::strieq_ascii(base->name, "libraryfolders"))
               return {};

            std::string app_id_string;
            if (appID == 0)
               app_id_string = "0";
            else {
               while (appID > 0) {
                  uint8_t digit = appID % 10;
                  app_id_string = (char)('0' + digit) + app_id_string;
                  appID /= 10;
               }
            }

            for (const auto* indexed : base->children) {
               {  // Name is numeric?
                  bool integer = true;
                  for (char c : indexed->name) {
                     if (c < '0' || c > '9') {
                        integer = false;
                        break;
                     }
                  }
                  if (!integer)
                     continue;
               }

               const char* path = nullptr;
               const node* apps = nullptr;

               for (const auto* child : indexed->children) {
                  if (cobb::strieq_ascii(child->name, "path")) {
                     if (child->value.has_value())
                        path = child->value.value().c_str();
                     continue;
                  }
                  if (cobb::strieq_ascii(child->name, "apps")) {
                     apps = child;
                  }
               }
               if (!path || !apps)
                  continue;

               for (auto* app : apps->children)
                  if (app->name == app_id_string)
                     return path;
            }
            return {};
         }

         constexpr std::optional<std::string> acf_install_directory_name() const {
            if (this->root.children.empty())
               return {};
            const auto* base = this->root.children[0];
            if (!cobb::strieq_ascii(base->name, "AppState"))
               return {};

            for (auto* child : base->children) {
               if (cobb::strieq_ascii(child->name, "installdir")) {
                  if (child->value.has_value())
                     return child->value.value();
               }
            }
            return {};
         }
   };
}
namespace cobb {
   namespace steam {
      bool get_steam_directory(std::wstring& out) {
         HKEY    key;
         LSTATUS status = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam\\", 0, KEY_WOW64_32KEY | KEY_QUERY_VALUE, &key);
         if (status == ERROR_SUCCESS) {
            DWORD valueType = REG_SZ;
            //
            wchar_t buffer[1024];
            DWORD   length = sizeof(buffer);
            status = RegQueryValueExW(key, L"InstallPath", 0, &valueType, (LPBYTE)buffer, &length);
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
         steam += L"/steamapps/libraryfolders.vdf";

         std::string library_path;
         std::string acf_path;
         std::string directory;
         {
            std::ifstream file(steam);
            if (!file)
               return false;

            valve_reader vdf;
            _file_to_string(file, vdf.data);
            if (!vdf.parse())
               return false;

            auto opt_library_path = vdf.vdf_library_folder_for(appID);
            if (!opt_library_path.has_value())
               return false;

            acf_path = opt_library_path.value() + "/steamapps/appmanifest_" + std::to_string(appID) + ".acf";
            library_path = std::move(opt_library_path.value());
         }
         {
            std::ifstream file(acf_path);
            if (!file)
               return false;

            valve_reader acf;
            _file_to_string(file, acf.data);
            if (!acf.parse())
               return false;

            auto opt_directory = acf.acf_install_directory_name();
            if (!opt_directory.has_value())
               return false;
            directory = std::move(opt_directory.value());
         }
         std::string result = library_path;
         if (!library_path.ends_with('/') && !library_path.ends_with('\\'))
            result += '/';
         result += "steamapps/common/";
         result += directory;
         if (!directory.ends_with('/') && !directory.ends_with('\\'))
            result += '/';
         
         out.clear();
         out.reserve(result.size());
         for (char c : result)
            out += c;
         return true;
      }
   }
}