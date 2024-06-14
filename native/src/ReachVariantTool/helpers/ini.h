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
#include "./ini/change_callback.h"
#include "./ini/file.h"
#include "./ini/setting.h"
#include "./ini/setting_type.h"
#include "./ini/setting_value_union.h"

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