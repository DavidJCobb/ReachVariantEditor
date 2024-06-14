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
#include <limits>
#include <type_traits>
#include "./file.h"
#include "./setting_type.h"
#include "./setting_value_union.h"

namespace cobb::ini {
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
         setting(manager_getter mg, const char* name, const char* category, bool   v) : owner(mg()), name(name), category(category), initial(v), current(v), pending(v), type(setting_type::boolean) {
            this->owner.insert_setting(this);
         };
         setting(manager_getter mg, const char* name, const char* category, float  v) : owner(mg()), name(name), category(category), initial(v), current(v), pending(v), type(setting_type::float32) {
            this->owner.insert_setting(this);
         };
         setting(manager_getter mg, const char* name, const char* category, int32_t v) : owner(mg()), name(name), category(category), initial(v), current(v), pending(v), type(setting_type::integer_signed) {
            this->owner.insert_setting(this);
         };
         setting(manager_getter mg, const char* name, const char* category, uint32_t v) : owner(mg()), name(name), category(category), initial(v), current(v), pending(v), type(setting_type::integer_unsigned) {
            this->owner.insert_setting(this);
         };
         setting(manager_getter mg, const char* name, const char* category, const char* v) : owner(mg()), name(name), category(category), initialStr(v), currentStr(v), pendingStr(v), type(setting_type::string) {
            this->owner.insert_setting(this);
         };

         file& owner;
         const char* const name;
         const char* const category;
         setting_value_union current; // avoid modifying this directly in favor of calling (modify_and_signal).
         setting_value_union initial; // avoid modifying this at all
         setting_value_union pending; // if you want to edit settings in memory and then commit them when the user presses an "OK" or "Save" button, modify this instead of (current)
         const setting_type type;
         std::string currentStr;
         std::string initialStr;
         std::string pendingStr;
         
         std::string to_string() const;
         
         constexpr void modify(bool v) noexcept { this->current.b = v; }
         constexpr void modify(float v) noexcept { this->current.f = v; }
         constexpr void modify(int32_t v) noexcept { this->current.i = v; }
         constexpr void modify(uint32_t v) noexcept { this->current.u = v; }
         constexpr void modify(const char* v) noexcept { this->currentStr = v; }
         
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
         
         void abandon_pending_changes() noexcept;
         void commit_pending_changes() noexcept;
   };
}