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
#include "reference_tracked_object.h"

namespace cobb {
   void reference_tracked_object::_add_outbound(reference_tracked_object& target) {
      auto& list = this->outbound;
      for (auto it = list.begin(); it != list.end(); ++it) {
         if (it->target != &target)
            continue;
         ++it->count;
         return;
      }
      list.emplace_back(&target, 1);
      //
      target._add_inbound(*this);
   }
   void reference_tracked_object::_add_inbound(reference_tracked_object& target) {
      auto& list = this->inbound;
      for (auto it = list.begin(); it != list.end(); ++it) {
         if (it->target != &target)
            continue;
         ++it->count;
         return;
      }
      list.emplace_back(&target, 1);
   }
   void reference_tracked_object::_remove_outbound(reference_tracked_object& target) {
      auto& list = this->outbound;
      for (auto it = list.begin(); it != list.end(); ++it) {
         if (it->target != &target)
            continue;
         if (--it->count)
            return;
         list.erase(it); // invalidates iterators and we're in a loop, so return immediately
         break;
      }
      target._remove_inbound(*this);
   }
   void reference_tracked_object::_remove_inbound(reference_tracked_object& target) {
      auto& list = this->inbound;
      for (auto it = list.begin(); it != list.end(); ++it) {
         if (it->target != &target)
            continue;
         if (--it->count)
            break;
         list.erase(it); // invalidates iterators and we're in a loop, so return immediately
         return;
      }
   }
   //
   void reference_tracked_object::sever_from_outbound_references() noexcept {
      for (auto& entry : this->outbound)
         entry.target->_remove_inbound(*this);
      this->outbound.clear();
   }
}