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
#include "./setting.h"

namespace cobb::ini {
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