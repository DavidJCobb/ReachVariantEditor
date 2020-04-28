#include "string.h"
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

namespace cobb {
   namespace qt {
      bool string_has_any_of(const QString& text, const QString& charset) {
         int size_text = text.size();
         int size_set  = charset.size();
         for (int i = 0; i < size_text; i++) {
            auto c = text[i];
            for (int j = 0; j < size_set; j++) {
               auto d = charset[j];
               if (c == d)
                  return true;
            }
         }
         return false;
      }
      bool string_has_any_of(const QString& text, const char* charset) {
         int size_text = text.size();
         int size_set  = strlen(charset);
         for (int i = 0; i < size_text; i++) {
            auto c = text[i];
            if (c > 255) // character isn't representable in Latin-1 and so cannot appear in a (const char*)-type charset
               continue;
            for (int j = 0; j < size_set; j++) {
               auto d = charset[j];
               if (c == d)
                  return true;
            }
         }
         return false;
      }
      bool string_is_integer(const QString& text) {
         bool is_int = false;
         text.toInt(&is_int);
         return is_int;
      }
      int stricmp(const QString& a, const std::string& b) {
         size_t sa = a.size();
         size_t sb = b.size();
         size_t s  = sa < sb ? sa : sb;
         for (size_t i = 0; i < s; ++i) {
            QChar ca = a[(uint)i].toLower();
            char  cb = tolower(b[i]);
            if (ca != cb)
               return ca.unicode() - cb;
         }
         if (sa != sb)
            return sa > sb ? a[(uint)sb].unicode() : b[sa];
         return 0;
      }
   }
}