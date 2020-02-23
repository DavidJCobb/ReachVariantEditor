#include "opcode_arg.h"

namespace MegaloEx {
   void OpcodeArgValue::do_index_fixup() {
      auto& list = this->relevant_objects;
      for (size_t i = 0; i < list.count; i++) {
         auto& range = list.ranges[i];
         if (!range.count)
            continue;
         if (range.start > this->data.size)
            continue;
         uint32_t index = -1;
         cobb::indexed_refcountable* object = list.pointers[i];
         if (object) {
            index = object->index;
         } else {
            // TODO: assert if object is missing, since that should never happen?
            #if _DEBUG
               __debugbreak(); // the object is missing; this should never happen
            #endif
         }
         this->data.overwrite(range.start, range.count, index);
      }
   }
}