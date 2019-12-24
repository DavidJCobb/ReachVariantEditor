#include "ini.h"
#include <algorithm>
#include <fstream>
#include "../helpers/strings.h"

#include <windows.h>
#include "../helpers/intrusive_windows_defines.h"

namespace {
   const std::wstring& _getINIPath() {
      static std::wstring path = L"ReachVariantTool.ini";
      return path;
   }
   const std::wstring& _getINIBackupPath() {
      static std::wstring path = L"ReachVariantTool.ini.bak";
      return path;
   }
   const std::wstring& _getINIWorkingPath() {
      static std::wstring path = L"ReachVariantTool.ini.tmp";
      return path;
   }
   constexpr char c_iniComment       = ';';
   constexpr char c_iniCategoryStart = '[';
   constexpr char c_iniCategoryEnd   = ']';
   constexpr char c_iniKeyValueDelim = '=';
}
namespace ReachINI {
   //
   // INI SETTING DEFINITIONS.
   //
   // The macro used here intentionally differs from the one used in the H file. Keep the namespaces 
   // synchronized between the two files.
   //
   #define REACHTOOL_MAKE_INI_SETTING(name, category, value) extern setting name = setting(#name, category, value);
   namespace UIWindowTitle {
      REACHTOOL_MAKE_INI_SETTING(bShowFullPath,     "UIWindowTitle", true);
      REACHTOOL_MAKE_INI_SETTING(bShowVariantTitle, "UIWindowTitle", true);
   }
   #undef REACHTOOL_MAKE_INI_SETTING
   //
   // INTERNALS BELOW.
   //
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
      }
      return out;
   };
   void INISettingManager::_CategoryToBeWritten::Write(INISettingManager* const manager, std::fstream& file) {
      if (this->name.empty())
         return;
      file.write(this->header.c_str(), this->header.size());
      {  // Write missing settings.
         try {
            const auto& list = manager->GetCategoryContents(this->name);
            for (auto it = list.begin(); it != list.end(); ++it) {
               const setting* const setting = *it;
               if (std::find(this->found.begin(), this->found.end(), setting) == this->found.end()) {
                  std::string line = setting->name;
                  line += '=';
                  line += setting->to_string();
                  line += "\n"; // MSVC appears to treat '\n' in a const char* as "\r\n", automatically, without asking, so don't add a '\r'
                  file.write(line.c_str(), line.size());
               }
            }
         } catch (std::out_of_range) {};
      }
      file.write(this->body.c_str(), this->body.size());
   };
   //
   INISettingManager& INISettingManager::GetInstance() {
      static INISettingManager instance;
      return instance;
   };
   setting* INISettingManager::Get(const char* category, const char* name) const {
      if (!category || !name)
         return nullptr;
      for (auto it = this->settings.begin(); it != this->settings.end(); ++it) {
         auto& setting = *it;
         if (!_stricmp(setting->category, category) && !_stricmp(setting->name, name))
            return setting;
      }
      return nullptr;
   };
   setting* INISettingManager::Get(std::string& category, std::string& name) const {
      return this->Get(category.c_str(), name.c_str());
   };
   void INISettingManager::ListCategories(std::vector<std::string>& out) const {
      out.reserve(this->byCategory.size());
      for (auto it = this->byCategory.begin(); it != this->byCategory.end(); ++it)
         out.push_back(it->first);
   };
   void INISettingManager::Add(setting* setting) {
      this->settings.push_back(setting);
      //
      std::string category = setting->category;
      std::transform(category.begin(), category.end(), category.begin(), ::tolower);
      this->byCategory[category].push_back(setting);
   };
   INISettingManager::VecSettings& INISettingManager::GetCategoryContents(std::string category) {
      std::transform(category.begin(), category.end(), category.begin(), ::tolower);
      return this->byCategory[category];
   };
   __declspec(noinline) void INISettingManager::Load() {
      std::wstring  path = _getINIPath();
      std::ifstream file;
      file.open(path);
      if (!file) {
         printf("Unable to load our INI file for reading. Calling INISettingManager::Save to generate a default one.\n");
         this->Save(); // generate a new INI file.
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
                  current = this->Get(category, key);
                  i++;
                  break;
               }
               key += c;
            }
         } while (++i < sizeof(buffer) && (c = buffer[i]));
         //
         // Code from here on out assumes that (i) will not be modified -- it will always refer 
         // either to the end of the line or to the first character after the '='.
         //
         if (current) { // handling for setting values
            {  // Allow comments on the same line as a setting (but make sure we change this if we add string settings!)
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
            }
         }
      }
      file.close();
   };
   void INISettingManager::AbandonPendingChanges() {
      for (auto& setting : this->settings)
         setting->abandon_pending_changes();
   }
   __declspec(noinline) void INISettingManager::Save() {
      for (auto& setting : this->settings) {
         setting->commit_pending_changes();
      }
      //
      std::wstring iPath = _getINIPath();
      std::wstring oPath = _getINIWorkingPath();
      std::fstream oFile;
      std::fstream iFile;
      oFile.open(oPath, std::ios_base::out | std::ios_base::trunc);
      if (!oFile) {
         printf("Unable to open working INI file for writing.\n");
         return;
      }
      //
      _CategoryToBeWritten current;
      std::vector<std::string> missingCategories;
      this->ListCategories(missingCategories);
      iFile.open(iPath, std::ios_base::in);
      if (iFile) { // original file is optional; we'll generate a new one after this if it's not present
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
               char c = line[i];
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
                     current.Write(this, oFile);
                     current = _CategoryToBeWritten(token, line);
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
                     auto ptr = this->Get(current.name.c_str(), token.c_str());
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
      current.Write(this, oFile); // write the last category, if necessary
      {  // Write missing categories.
         auto& list = missingCategories;
         for (auto it = list.begin(); it != list.end(); ++it) {
            oFile.put('\n'); // MSVC treats std::fstream.put('\n') as .write("\r\n", 2) automatically, without asking, so don't put('\r')
            std::string cat = *it;
            cat += "]\n"; // MSVC appears to treat '\n' in a const char* as "\r\n", automatically, without asking, so don't add a '\r'
            cat.insert(cat.begin(), '[');
            current = _CategoryToBeWritten(*it, cat);
            current.Write(this, oFile);
         }
      }
      oFile.close();
      bool success = ReplaceFile(_getINIPath().c_str(), _getINIWorkingPath().c_str(), _getINIBackupPath().c_str(), 0, 0, 0);
      if (!success) {
         //if (!PathFileExists(_getINIPath().c_str())) {
            success = MoveFileEx(_getINIWorkingPath().c_str(), _getINIPath().c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
         //}
      }
   };

   std::vector<ChangeCallback> _changeCallbacks;
   void RegisterForChanges(ChangeCallback c) {
      if (!c)
         return;
      if (std::find(_changeCallbacks.begin(), _changeCallbacks.end(), c) != _changeCallbacks.end())
         return;
      _changeCallbacks.push_back(c);
   };
   void UnregisterForChanges(ChangeCallback c) {
      if (!c)
         return;
      _changeCallbacks.erase(
         std::remove(_changeCallbacks.begin(), _changeCallbacks.end(), c)
      );
   };
   void SendChangeEvent() {
      for (auto it = _changeCallbacks.begin(); it != _changeCallbacks.end(); ++it) {
         if (*it)
            (*it)(nullptr, 0, 0);
      }
   };
   void SendChangeEvent(setting* setting, setting_value_union oldValue, setting_value_union newValue) {
      for (auto it = _changeCallbacks.begin(); it != _changeCallbacks.end(); ++it) {
         if (*it)
            (*it)(setting, oldValue, newValue);
      }
   };
}