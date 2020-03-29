#pragma once
#include <string>
#include <vector>

namespace Megalo {
   class OpcodeArgTypeinfo;
   //
   namespace Script {
      class Property {
         public:
            std::string name;
            const OpcodeArgTypeinfo& type;
            bool     allow_from_nested = false; // allow (namespace.var.var.property)? only for biped accessors
            uint32_t hard_max_index    = 0;
            bool     is_read_only      = false;
            int8_t   scope_indicator   = -1; // required for numbers and timers
      };
      class TypeinfoCompileData {
         public:
            OpcodeArgTypeinfo* underlying_type = nullptr;
            uint32_t static_limit = 0; // if > 0, then the limit is constant
      };
   }
}