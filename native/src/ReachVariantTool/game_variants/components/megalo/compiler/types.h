#pragma once
#include <string>
#include <vector>

namespace Megalo {
   class OpcodeArgTypeinfo;
   //
   namespace Script {
      class Property {
         //
         // This class exists just so that types can specify what properties exist on them. Instances 
         // of this class CAN specify enough information to fully resolve scopes/which/indices/etc., 
         // but it's not super required; their main role is just so that the compiler can identify 
         // references to the properties during parsing.
         //
         public:
            std::string name;
            const OpcodeArgTypeinfo& type;
            bool     allow_from_nested = false; // allow (namespace.var.var.property)? only for biped accessors
            uint32_t hard_max_index    = 0;
            bool     is_read_only      = false;
            const char* scope_indicator_id = nullptr; // use nullptr if the scope is to be determined dynamically
            //
            Property(const char* n, const OpcodeArgTypeinfo& t, const char* scope = nullptr, bool afn = false) : name(n), type(t), scope_indicator_id(scope), allow_from_nested(afn) {}
      };
      class TypeinfoToScriptMapping {
         public:
            OpcodeArgTypeinfo* underlying_type = nullptr;
            uint32_t static_limit = 0; // if > 0, then the limit is constant
      };
   }
}