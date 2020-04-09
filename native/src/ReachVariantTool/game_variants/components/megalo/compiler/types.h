#pragma once
#include <string>
#include <vector>

namespace Megalo {
   class OpcodeArgTypeinfo;
   class VariableScopeIndicatorValue;
   //
   namespace Script {
      class Property {
         //
         // This class exists just so that types can specify what properties exist on them. Instances 
         // of this class CAN specify enough information to fully resolve scopes/which/indices/etc., 
         // but it's not super required; their main role is just so that the compiler can identify 
         // references to the properties during parsing.
         //
         // Do not use this for abstract properties.
         //
         public:
            static constexpr VariableScopeIndicatorValue* no_scope = nullptr;
            //
         public:
            std::string name;
            const OpcodeArgTypeinfo& type;
            bool    allow_from_nested  = false; // allow (namespace.var.var.property)? only for biped accessors
            VariableScopeIndicatorValue* scope = nullptr;
            //
            Property(const char* n, const OpcodeArgTypeinfo& t, VariableScopeIndicatorValue* scope = no_scope, bool afn = false) : name(n), type(t), scope(scope), allow_from_nested(afn) {}
      };
   }
}