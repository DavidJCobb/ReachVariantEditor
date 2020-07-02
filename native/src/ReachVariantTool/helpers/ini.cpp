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
#include "ini.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include "strings.h"

#include <windows.h>
#include "intrusive_windows_defines.h"

namespace {
   constexpr char c_iniComment       = ';';
   constexpr char c_iniCategoryStart = '[';
   constexpr char c_iniCategoryEnd   = ']';
   constexpr char c_iniKeyValueDelim = '=';
}
namespace cobb {
   namespace ini {
      void file::_pending_category::write(file* const manager, std::fstream& file) {
         if (this->name.empty())
            return;
         file.write(this->header.c_str(), this->header.size());
         {  // Write missing settings.
            try {
               const auto& list = manager->_get_category_contents(this->name);
               for (auto it = list.begin(); it != list.end(); ++it) {
                  const setting* const setting = *it;
                  if (std::find(this->found.begin(), this->found.end(), setting) == this->found.end()) {
                     std::string line = setting->name;
                     line += '=';
                     line += setting->to_string();
                     line += "\n"; // MSVC appears to sometimes treat '\n' in a const char* as "\r\n", automatically, without asking, so don't add a '\r'
                     file.write(line.c_str(), line.size());
                  }
               }
            } catch (std::out_of_range) {};
         }
         file.write(this->body.c_str(), this->body.size());
      }

      file::file(const wchar_t* filepath) {
         this->filePath = filepath;
         std::filesystem::path w = filepath;
         std::filesystem::path b = w;
         w.replace_extension(".ini.tmp");
         b.replace_extension(".ini.bak");
         this->backupFilePath  = w;
         this->workingFilePath = b;
      }
      file::file(const wchar_t* filepath, const wchar_t* backup, const wchar_t* working) {
         this->filePath = filepath;
         this->backupFilePath = backup;
         this->workingFilePath = working;
      }
      //
      file::setting_list& file::_get_category_contents(std::string category) {
         std::transform(category.begin(), category.end(), category.begin(), ::tolower);
         return this->byCategory[category];
      }
      void file::_send_change_event(setting* s, setting_value_union oldValue, setting_value_union newValue) {
         auto copy = this->changeCallbacks; // in case a callback unregisters itself
         for (auto cb : copy)
            cb(s, oldValue, newValue);
      }
      setting* file::get_setting(const char* category, const char* name) const {
         if (!category || !name)
            return nullptr;
         for (auto it = this->settings.begin(); it != this->settings.end(); ++it) {
            auto& setting = *it;
            if (!_stricmp(setting->category, category) && !_stricmp(setting->name, name))
               return setting;
         }
         return nullptr;
      }
      setting* file::get_setting(std::string& category, std::string& name) const {
         return this->get_setting(category.c_str(), name.c_str());
      }
      void file::get_categories(std::vector<std::string>& out) const {
         out.reserve(this->byCategory.size());
         for (auto it = this->byCategory.begin(); it != this->byCategory.end(); ++it)
            out.push_back(it->first);
      }
      void file::insert_setting(setting* setting) {
         this->settings.push_back(setting);
         //
         std::string category = setting->category;
         std::transform(category.begin(), category.end(), category.begin(), ::tolower);
         this->byCategory[category].push_back(setting);
      }
      //
      __declspec(noinline) void file::load() {
         std::ifstream file;
         file.open(this->filePath);
         if (!file) {
            printf("Unable to load our INI file for reading. Calling cobb::ini::file::save to generate a default one.\n");
            this->save(); // generate a new INI file.
            return;
         }
         std::string category = "";
         std::string key      = "";
         while (!file.bad() && !file.eof()) {
            char buffer[1024];
            file.getline(buffer, sizeof(buffer));
            buffer[1023] = '\0';
            //
            bool     foundAny   = false; // found anything that isn't whitespace?
            bool     isCategory = false;
            setting* current    = nullptr;
            size_t   i = 0;
            char     c = buffer[0];
            #pragma region Category headers and INI setting names
            if (!c)
               continue;
            do {
               if (!foundAny) {
                  if (c == ' ' || c == '\t')
                     continue;
                  if (c == c_iniComment) // lines starting with semicolons are comments
                     break;
                  foundAny = true;
                  if (c == c_iniCategoryStart) {
                     isCategory = true;
                     category   = "";
                     continue;
                  }
                  key = "";
               }
               if (isCategory) {
                  if (c == c_iniCategoryEnd)
                     break;
                  category += c;
                  continue;
               }
               //
               // handling for setting names:
               //
               if (!current) {
                  if (c == c_iniKeyValueDelim) {
                     current = this->get_setting(category, key);
                     i++;
                     break;
                  }
                  key += c;
               }
            } while (++i < sizeof(buffer) && (c = buffer[i]));
            #pragma endregion
            //
            // Code from here on out assumes that (i) will not be modified -- it will always refer 
            // either to the end of the line or to the first character after the '='.
            //
            #pragma region INI setting value
            if (current) { // handling for setting values
               if (current->type != setting_type::string) {  // Allow comments on the same line as a setting (unless the setting value is a string)
                  size_t j = i;
                  do {
                     if (buffer[j] == '\0')
                        break;
                     if (buffer[j] == ';') {
                        buffer[j] = '\0';
                        break;
                     }
                  } while (++j < sizeof(buffer));
               }
               switch (current->type) {
                  case setting_type::boolean:
                     {
                        if (cobb::string_says_false(buffer + i)) {
                           current->current.b = false;
                           break;
                        }
                        //
                        // treat numbers that compute to zero as "false:"
                        //
                        float value;
                        bool  final = true;
                        if (cobb::string_to_float(buffer + i, value)) {
                           final = (value != 0.0);
                        }
                        current->current.b = final;
                     }
                     break;
                  case setting_type::float32:
                     {
                        float value;
                        if (cobb::string_to_float(buffer + i, value))
                           current->current.f = value;
                     }
                     break;
                  case setting_type::integer_signed:
                     {
                        int32_t value;
                        if (cobb::string_to_int(buffer + i, value))
                           current->current.i = value;
                     }
                     break;
                  case setting_type::integer_unsigned:
                     {
                        uint32_t value;
                        if (cobb::string_to_int(buffer + i, value))
                           current->current.u = value;
                     }
                     break;
                  case setting_type::string:
                     current->currentStr.clear();
                     do {
                        char c = buffer[i];
                        if (c == '\0')
                           break;
                        current->currentStr += c;
                     } while (++i < sizeof(buffer));
                     break;
               }
            }
            #pragma endregion
         }
         file.close();
      };
      __declspec(noinline) void file::save() {
         for (auto& setting : this->settings) {
            setting->commit_pending_changes();
         }
         //
         std::fstream oFile;
         std::fstream iFile;
         oFile.open(this->workingFilePath, std::ios_base::out | std::ios_base::trunc);
         if (!oFile) {
            printf("Unable to open working INI file for writing.\n");
            return;
         }
         //
         _pending_category current;
         std::vector<std::string> missingCategories;
         this->get_categories(missingCategories);
         iFile.open(this->filePath, std::ios_base::in);
         if (iFile) {
            //
            // If the INI file already exists, read its contents: we want to preserve whitespace, comments, 
            // the order of settings, and so on. This loop reads the original file and then writes them (with 
            // any needed changes to INI setting values) to the working file.
            //
            bool beforeCategories = true;
            while (!iFile.bad() && !iFile.eof()) {
               std::string line;
               getline(iFile, line);
               line += '\n'; // add the terminator, since it (but not '\r') will be omitted from getline's result.
               size_t i    = 0;
               size_t size = line.size();
               bool   foundAny   = false; // found any non-whitespace chars
               bool   isCategory = false;
               bool   isValue    = false;
               std::string token;
               do {
                  unsigned char c = line[i];
                  if (!c)
                     break;
                  if (c == c_iniComment) { // comment
                     if (isValue) {
                        size_t j = i; // we want to preserve any whitespace that followed the original value
                        do {
                           char d = line[--j];
                           if (!isspace(d))
                              break;
                        } while (j);
                        current.body += (line.c_str() + j + 1); // at this point, j will include one non-whitespace character BEFORE the trailing whitespace
                        break;
                     }
                     current.body += line;
                     break;
                  }
                  if (c == '\r' || c == '\n') { // end of line; make sure we get the whole terminator (we need this to catch the case of a setting value with no trailing comment)
                     current.body += (line.c_str() + i);
                     break;
                  }
                  if (!foundAny) {
                     if (c == c_iniCategoryStart) {
                        isCategory = true;
                        beforeCategories = false;
                        continue;
                     }
                     if (isspace(c))
                        continue;
                     foundAny = true;
                  }
                  if (isCategory) {
                     if (c == c_iniCategoryEnd) {
                        current.write(this, oFile);
                        current = _pending_category(token, line);
                        {
                           std::transform(token.begin(), token.end(), token.begin(), ::tolower);
                           missingCategories.erase(std::remove(missingCategories.begin(), missingCategories.end(), token), missingCategories.end());
                        }
                        break;
                     }
                     token += c;
                  } else {
                     if (c == c_iniKeyValueDelim) {
                        current.body.append(line, 0, i + 1);
                        auto ptr = this->get_setting(current.name.c_str(), token.c_str());
                        if (!ptr) {
                           current.body.append(line, i);
                           break;
                        }
                        current.body += ptr->to_string();
                        current.found.push_back(ptr);
                        isValue = true;
                        continue;
                     }
                     token += c;
                  }
               } while (++i < size);
               if (beforeCategories) { // content before all categories
                  oFile.write(line.c_str(), size);
               }
            }
            iFile.close();
         }
         current.write(this, oFile); // write the last category, if necessary
         {  // Write missing categories.
            //
            // Write any setting categories that weren't present in the existing file. (If there was 
            // no existing file, then this writes all categories, creating the file from scratch.)
            //
            for (auto& cat : missingCategories) {
               oFile.put('\n'); // MSVC treats std::fstream.put('\n') as .write("\r\n", 2) automatically, without asking, so don't put('\r')
               std::string header;
               cobb::sprintf(header, "[%s]\n", cat.c_str());
               current = _pending_category(cat, header);
               current.write(this, oFile);
            }
         }
         oFile.close();
         bool success = ReplaceFileW(this->filePath.c_str(), this->workingFilePath.c_str(), this->backupFilePath.c_str(), 0, 0, 0);
         if (!success) {
            //if (!PathFileExists(_getINIPath().c_str())) {
               success = MoveFileExW(this->workingFilePath.c_str(), this->filePath.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
            //}
         }
      }
      //
      void file::abandon_pending_changes() {
         for (auto& setting : this->settings)
            setting->abandon_pending_changes();
      }
      //
      void file::register_for_changes(change_callback cb) noexcept {
         if (!cb)
            return;
         auto& list = this->changeCallbacks;
         if (std::find(list.begin(), list.end(), cb) != list.end())
            return;
         list.push_back(cb);
      }
      void file::unregister_for_changes(change_callback cb) noexcept {
         if (!cb)
            return;
         auto& list = this->changeCallbacks;
         list.erase(std::remove(list.begin(), list.end(), cb));
      }


      void setting::send_change_event(setting_value_union old) noexcept {
         this->owner._send_change_event(this, old, this->current);
      }
      std::string setting::to_string() const {
         std::string out;
         char working[20];
         switch (this->type) {
            case setting_type::boolean:
               out = this->current.b ? "TRUE" : "FALSE";
               break;
            case setting_type::float32:
               sprintf_s(working, "%f", this->current.f);
               working[19] = '\0';
               out = working;
               break;
            case setting_type::integer_signed:
               sprintf_s(working, "%i", this->current.i);
               working[19] = '\0';
               out = working;
               break;
            case setting_type::integer_unsigned:
               sprintf_s(working, "%u", this->current.u);
               working[19] = '\0';
               out = working;
               break;
            case setting_type::string:
               out = this->currentStr;
               break;
         }
         return out;
      }
      void setting::modify_and_signal(const char* s) {
         if (this->type != setting_type::string)
            return;
         this->currentStr = s;
         this->send_change_event(0);
      }
      void setting::abandon_pending_changes() noexcept {
         switch (this->type) {
            case setting_type::boolean:
               this->pending.b = this->current.b;
               return;
            case setting_type::float32:
               this->pending.f = this->current.f;
               return;
            case setting_type::integer_signed:
               this->pending.i = this->current.i;
               return;
            case setting_type::integer_unsigned:
               this->pending.u = this->current.u;
               return;
            case setting_type::string:
               this->pendingStr = this->currentStr;
               return;
         }
      }
      void setting::commit_pending_changes() noexcept {
         switch (this->type) {
            case setting_type::boolean:
               if (this->pending.b != this->current.b)
                  this->modify_and_signal(this->pending.b);
               return;
            case setting_type::float32:
               if (this->pending.f != this->current.f)
                  this->modify_and_signal(this->pending.f);
               return;
            case setting_type::integer_signed:
               if (this->pending.i != this->current.i)
                  this->modify_and_signal(this->pending.i);
               return;
            case setting_type::integer_unsigned:
               if (this->pending.u != this->current.u)
                  this->modify_and_signal(this->pending.u);
               return;
            case setting_type::string:
               if (this->pendingStr != this->currentStr)
                  this->modify_and_signal(this->pendingStr.c_str());
               return;
         }
      }
   }
}