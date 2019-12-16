#pragma once
#include "../opcode_arg.h"

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
}