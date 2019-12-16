#pragma once
#include "../opcode_arg.h"
#include "../limits.h"

namespace Megalo {
   class OpcodeArgValueLabelIndex : public OpcodeArgValueBaseIndex {
      public:
         OpcodeArgValueLabelIndex() : OpcodeArgValueBaseIndex("Forge Label", 16, index_quirk::presence) {};
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueLabelIndex();
         }
   };
   class OpcodeArgValueMPObjectTypeIndex : public OpcodeArgValueBaseIndex {
      public:
         OpcodeArgValueMPObjectTypeIndex() : OpcodeArgValueBaseIndex("MP Object Type", 2048, index_quirk::presence) {};
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueMPObjectTypeIndex();
         }
   };
   class OpcodeArgValueNameIndex : public OpcodeArgValueBaseIndex {
      public:
         OpcodeArgValueNameIndex() : OpcodeArgValueBaseIndex("Name", Limits::max_variant_strings, index_quirk::offset) {};
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueNameIndex();
         }
   };
}