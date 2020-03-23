#pragma once
#include "../opcode_arg.h"
#include "../limits.h"
#include "../../../data/mp_incidents.h"
#include "../../../data/mp_object_types.h"
#include "../../../data/mp_object_names.h"

namespace Megalo {
   class OpcodeArgValueRequisitionPalette : public OpcodeArgValueBaseIndex { // development leftover; later used in Halo 4?
      public:
         OpcodeArgValueRequisitionPalette() : OpcodeArgValueBaseIndex("Requisition Palette", cobb::bitmax(4), index_quirk::presence) {};
         static OpcodeArgValue* factory(cobb::ibitreader&) {
            return new OpcodeArgValueRequisitionPalette();
         }
   };
   class OpcodeArgValueTrigger : public OpcodeArgValueBaseIndex {
      public:
         OpcodeArgValueTrigger() : OpcodeArgValueBaseIndex("Trigger", Limits::max_triggers, index_quirk::reference) {};
         static OpcodeArgValue* factory(cobb::ibitreader&) {
            return new OpcodeArgValueTrigger();
         }
   };
   
   class OpcodeArgValueIconIndex6Bits : public OpcodeArgValueBaseIndex {
      public:
         OpcodeArgValueIconIndex6Bits() : OpcodeArgValueBaseIndex("Icon", cobb::bitmax(6), index_quirk::presence) {};
         static OpcodeArgValue* factory(cobb::ibitreader&) {
            return new OpcodeArgValueIconIndex6Bits();
         }
   };
   class OpcodeArgValueIconIndex7Bits : public OpcodeArgValueBaseIndex {
      public:
         OpcodeArgValueIconIndex7Bits() : OpcodeArgValueBaseIndex("Icon", cobb::bitmax(7), index_quirk::presence) {};
         static OpcodeArgValue* factory(cobb::ibitreader&) {
            return new OpcodeArgValueIconIndex7Bits();
         }
   };
}