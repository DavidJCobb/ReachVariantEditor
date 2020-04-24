#pragma once
#include "../opcode_arg.h"
#include "../limits.h"

namespace Megalo {
   class OpcodeArgValueRequisitionPalette : public OpcodeArgValueBaseIndex { // development leftover; later used in Halo 4?
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValueRequisitionPalette() : OpcodeArgValueBaseIndex("Requisition Palette", cobb::bitmax(4), index_quirk::presence) {}
   };
   class OpcodeArgValueTrigger : public OpcodeArgValueBaseIndex {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValueTrigger() : OpcodeArgValueBaseIndex("Trigger", Limits::max_triggers, index_quirk::reference) {}
   };
}