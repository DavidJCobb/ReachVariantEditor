#pragma once
#include "../opcode_arg.h"

namespace Megalo {
   class MegaloValueLabelIndex : public OpcodeArgValueBaseIndex {
      public:
         MegaloValueLabelIndex() : OpcodeArgValueBaseIndex("Forge Label", 16, index_quirk::presence) {};
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new MegaloValueLabelIndex();
         }
   };
   class MegaloValueMPObjectTypeIndex : public OpcodeArgValueBaseIndex {
      public:
         MegaloValueMPObjectTypeIndex() : OpcodeArgValueBaseIndex("MP Object Type", 2048, index_quirk::presence) {};
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new MegaloValueMPObjectTypeIndex();
         }
   };
}