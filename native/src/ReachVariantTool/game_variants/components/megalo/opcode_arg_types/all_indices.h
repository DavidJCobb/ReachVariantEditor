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
   
   class OpcodeArgValueIconIndex6Bits : public OpcodeArgValueBaseIndex {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValueIconIndex6Bits() : OpcodeArgValueBaseIndex("Icon", cobb::bitmax(6), index_quirk::presence) {}
   };
   class OpcodeArgValueIconIndex7Bits : public OpcodeArgValueBaseIndex {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValueIconIndex7Bits() : OpcodeArgValueBaseIndex("Icon", cobb::bitmax(7), index_quirk::presence) {}
   };
}