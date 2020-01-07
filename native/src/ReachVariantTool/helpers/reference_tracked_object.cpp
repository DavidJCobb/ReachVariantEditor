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