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
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>
#include "./change_callback.h"
#include "./setting_value_union.h"

namespace cobb::ini {
   class file;
   class setting;
}

namespace cobb::ini {
   using manager_getter = file&(*)();

   class file {
      //
      // Class for a single INI file; it retains settings in-memory, both as a flat list and organized by 
      // category.
      //
      friend setting;
      protected:
         using setting_list = std::vector<setting*>;
         using setting_map  = std::unordered_map<std::string, setting_list>;
         
         setting_list settings;
         setting_map  byCategory;
         std::vector<change_callback> changeCallbacks;
         std::wstring filePath;
         std::wstring backupFilePath;
         std::wstring workingFilePath;
         
         struct _pending_category { // state object used when saving INI settings
            //
            // When saving an INI file that has been modified by the program, we attempt to preserve whitespace, 
            // comments, the order of settings, and so on. This requires that we read the file first, store that 
            // information, and then resave the file; this struct holds the preserved whitespace and other data.
            //
            _pending_category() {};
            _pending_category(std::string name, std::string header) : name(name), header(header) {};
            //
            std::string  name;   // name of the category, used to look up INISetting pointers for found setting names
            std::string  header; // the header line, including whitespace and comments
            std::string  body;   // the body
            setting_list found;  // found settings
            //
            void write(file* const, std::fstream&);
         };
         
         setting_list& _get_category_contents(std::string category);
         virtual void _send_change_event(setting*, setting_value_union oldValue, setting_value_union newValue);
         
      public:
         file();
         file(std::filesystem::path filepath);
         file(std::filesystem::path filepath, std::filesystem::path backup, std::filesystem::path working);

         void set_paths(std::filesystem::path filepath);
         void set_paths(std::filesystem::path filepath, std::filesystem::path backup, std::filesystem::path working);

         void insert_setting(setting* setting);
         void load(); // if the file doesn't exist, calls (save) to generate a new file and then returns (does not redundantly load the new file)
         void save(); // preserves the existing file's whitespace, comments, setting order, etc.; writes all setting values including those not changed from the defaults
         void abandon_pending_changes();
         
         setting* get_setting(std::string& category, std::string& name) const;
         setting* get_setting(const char* category, const char* name) const;
         void get_categories(std::vector<std::string>& out) const;
         
         void register_for_changes(change_callback) noexcept;
         void unregister_for_changes(change_callback) noexcept;
   };
}