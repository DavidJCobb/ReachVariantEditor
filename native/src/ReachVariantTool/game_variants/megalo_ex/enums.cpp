#include "enums.h"
#include "../../helpers/strings.h"

namespace Megalo {
   void SmartEnum::to_string(std::string& out, uint32_t value) const noexcept {
      if (value >= this->count) {
         cobb::sprintf(out, "unknown:%d", value);
         return;
      }
      out = this->values[value];
   }
   void SmartFlags::to_string(std::string& out, uint32_t value) const noexcept {
      out.clear();
      size_t i = 0;
      for (; i < this->count; i++) {
         if (value & (1 << i)) {
            if (!out.empty())
               out += ", ";
            out += this->values[i];
         }
      }
      for (; i < 32; i++) {
         std::string temp;
         if (value & (1 << i)) {
            if (!out.empty())
               out += ", ";
            cobb::sprintf(temp, "unknown:%d", i);
            out += temp;
         }
      }
   }
}