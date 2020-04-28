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
#include <unordered_map>
#include <vector>
#include <type_traits>

/*

   The approach I take to defining settings is to do something like this in a header file:

      namespace MyProgram {
         extern cobb::ini::file& get();

         #define MYPROGRAM_MAKE_INI_SETTING(name, category, value) extern cobb::ini::setting name;
         namespace UIWindowTitle {
            MYPROGRAM_MAKE_INI_SETTING(bShowFullPath,     "UIWindowTitle", true);
            MYPROGRAM_MAKE_INI_SETTING(bShowVariantTitle, "UIWindowTitle", true);
         }
         #undef MYPROGRAM_MAKE_INI_SETTING
      }

   And then do this in the CPP file:

      namespace MyProgram {
         extern cobb::ini::file& get() {
            static cobb::ini::file instance = cobb::ini::file(L"MyFile.ini");
            return instance;
         }

         #define MYPROGRAM_MAKE_INI_SETTING(name, category, value) extern cobb::ini::setting name = cobb::ini::setting(get, #name, category, value);
         namespace UIWindowTitle {
            MYPROGRAM_MAKE_INI_SETTING(bShowFullPath,     "UIWindowTitle", true);
            MYPROGRAM_MAKE_INI_SETTING(bShowVariantTitle, "UIWindowTitle", true);
         }
         #undef MYPROGRAM_MAKE_INI_SETTING
      }
*/

namespace cobb {
   namespace ini {
      class setting;
      //
      enum class setting_type {
         boolean,
         float32,
         integer_signed,
         integer_unsigned,
         string,
      };
      union setting_value_union {
         bool     b;
         float    f;
         int32_t  i;
         uint32_t u;
         //
         setting_value_union() : u(0) {};
         setting_value_union(bool v) : b(v) {};
         setting_value_union(float v) : f(v) {};
         setting_value_union(int32_t v) : i(v) {};
         setting_value_union(uint32_t v) : i(v) {};
      };

      template<typename T> struct setting_type_constant_for_type {};
      template<> struct setting_type_constant_for_type<bool> { static constexpr setting_type value = setting_type::boolean; };
      template<> struct setting_type_constant_for_type<float> { static constexpr setting_type value = setting_type::float32; };
      template<> struct setting_type_constant_for_type<int32_t> { static constexpr setting_type value = setting_type::integer_signed; };
      template<> struct setting_type_constant_for_type<uint32_t> { static constexpr setting_type value = setting_type::integer_unsigned; };
      template<typename T> static constexpr setting_type setting_type_constant_for_type_v = setting_type_constant_for_type<T>::value;

      //
      // Type for change-event listeners. Note that as of this writing values are not sent if the changed 
      // setting is a string.
      //
      using change_callback = void(*)(setting* s, setting_value_union oldValue, setting_value_union newValue);

      class file {
         //
         // Class for a single INI file; it retains settings in-memory, both as a flat list and organized by 
         // category.
         //
         friend setting;
         private:
            using setting_list = std::vector<setting*>;
            using setting_map  = std::unordered_map<std::string, setting_list>;
            //
            setting_list settings;
            setting_map  byCategory;
            std::vector<change_callback> changeCallbacks;
            std::wstring filePath;
            std::wstring backupFilePath;
            std::wstring workingFilePath;
            //
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
            //
            setting_list& _get_category_contents(std::string category);
            void _send_change_event(setting*, setting_value_union oldValue, setting_value_union newValue);
            //
         public:
            file(const wchar_t* filepath);
            file(const wchar_t* filepath, const wchar_t* backup, const wchar_t* working);
            //
            void insert_setting(setting* setting);
            void load(); // if the file doesn't exist, calls (save) to generate a new file and then returns (does not redundantly load the new file)
            void save(); // preserves the existing file's whitespace, comments, setting order, etc.; writes all setting values including those not changed from the defaults
            void abandon_pending_changes();
            //
            setting* get_setting(std::string& category, std::string& name) const;
            setting* get_setting(const char* category, const char* name) const;
            void get_categories(std::vector<std::string>& out) const;
            //
            void register_for_changes(change_callback) noexcept;
            void unregister_for_changes(change_callback) noexcept;
      };
      using manager_getter = file&(*)();

      class setting {
         //
         // Class for a single INI setting; multiple value types are supported.
         //
         // SUGGESTED USAGE:
         //
         //  - If you want to instantaneously modify a setting's value, then modify (current).
         //
         //  - If you want to make a tentative change to a setting's value to be committed later, as in the 
         //    case of an options dialog that should only commit changes when the user clicks "Save," then 
         //    modify (pending). When the user clicks "Save," call (save) on the containing file, which 
         //    will commit changes for you before writing any output; alternatively, when the user clicks 
         //    "Cancel," call (abandon_pending_changes) on the containing file.
         //
         private:
            void send_change_event(setting_value_union old) noexcept;
         public:
            setting(manager_getter mg, const char* name, const char* category, bool   v) : owner(mg()), name(name), category(category), default(v), current(v), pending(v), type(setting_type::boolean) {
               this->owner.insert_setting(this);
            };
            setting(manager_getter mg, const char* name, const char* category, float  v) : owner(mg()), name(name), category(category), default(v), current(v), pending(v), type(setting_type::float32) {
               this->owner.insert_setting(this);
            };
            setting(manager_getter mg, const char* name, const char* category, int32_t v) : owner(mg()), name(name), category(category), default(v), current(v), pending(v), type(setting_type::integer_signed) {
               this->owner.insert_setting(this);
            };
            setting(manager_getter mg, const char* name, const char* category, uint32_t v) : owner(mg()), name(name), category(category), default(v), current(v), pending(v), type(setting_type::integer_unsigned) {
               this->owner.insert_setting(this);
            };
            setting(manager_getter mg, const char* name, const char* category, const char* v) : owner(mg()), name(name), category(category), defaultStr(v), currentStr(v), pendingStr(v), type(setting_type::string) {
               this->owner.insert_setting(this);
            };

            file& owner;
            const char* const name;
            const char* const category;
            setting_value_union current; // avoid modifying this directly in favor of calling (modify_and_signal).
            setting_value_union default; // avoid modifying this at all
            setting_value_union pending; // if you want to edit settings in memory and then commit them when the user presses an "OK" or "Save" button, modify this instead of (current)
            const setting_type type;
            std::string currentStr;
            std::string defaultStr;
            std::string pendingStr;
            //
            std::string to_string() const;
            //
            void modify(bool v) noexcept { this->current.b = v; }
            void modify(float v) noexcept { this->current.f = v; }
            void modify(int32_t v) noexcept { this->current.i = v; }
            void modify(uint32_t v) noexcept { this->current.u = v; }
            void modify(const char* v) noexcept { this->currentStr = v; }
            //
            template<typename T> void modify_and_signal(T value) {
               if (std::numeric_limits<T>::is_integer && !std::is_same_v<T, bool>) { // any int
                  if (this->type != setting_type::integer_signed && this->type != setting_type::integer_unsigned)
                     return;
               } else if (std::is_floating_point_v<T>) { // float, double
                  if (this->type != setting_type::float32)
                     return;
               } else {
                  if (setting_type_constant_for_type_v<T> != this->type)
                     return;
               }
               //
               setting_value_union old = this->current;
               this->modify(value);
               this->send_change_event(old);
            }
            void modify_and_signal(const char* s);
            //
            void abandon_pending_changes() noexcept;
            void commit_pending_changes() noexcept;
      };
   }
}