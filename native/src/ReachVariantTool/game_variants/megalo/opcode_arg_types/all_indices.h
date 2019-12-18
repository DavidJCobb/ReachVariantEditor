#pragma once
#include "../opcode_arg.h"
#include "../limits.h"

namespace Megalo {
   class OpcodeArgValueIncidentID : public OpcodeArgValueBaseIndex { // development leftover; later used in Halo 4?
      public:
         OpcodeArgValueIncidentID() : OpcodeArgValueBaseIndex("Incident ID", Limits::max_incident_types, index_quirk::offset) {};
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueIncidentID();
         }
   };
   class OpcodeArgValueLabelIndex : public OpcodeArgValueBaseIndex {
      public:
         OpcodeArgValueLabelIndex() : OpcodeArgValueBaseIndex("Forge Label", Limits::max_script_labels, index_quirk::presence) {};
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueLabelIndex();
         }
   };
   class OpcodeArgValueLoadoutPalette : public OpcodeArgValueBaseIndex {
      public:
         OpcodeArgValueLoadoutPalette() : OpcodeArgValueBaseIndex("Loadout Palette", 6, index_quirk::reference) {};
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueLoadoutPalette();
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
         OpcodeArgValueNameIndex() : OpcodeArgValueBaseIndex("Name", Limits::max_string_ids, index_quirk::offset) {};
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueNameIndex();
         }
   };
   class OpcodeArgValuePlayerTraits : public OpcodeArgValueBaseIndex {
      public:
         OpcodeArgValuePlayerTraits() : OpcodeArgValueBaseIndex("Scripted Player Traits", Limits::max_script_traits, index_quirk::reference) {};
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValuePlayerTraits();
         }
   };
   class OpcodeArgValueRequisitionPalette : public OpcodeArgValueBaseIndex { // development leftover; later used in Halo 4?
      public:
         OpcodeArgValueRequisitionPalette() : OpcodeArgValueBaseIndex("Requisition Palette", cobb::bitmax(4), index_quirk::presence) {};
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueRequisitionPalette();
         }
   };
   class OpcodeArgValueSound : public OpcodeArgValueBaseIndex {
      public:
         OpcodeArgValueSound() : OpcodeArgValueBaseIndex("Sound", Limits::max_engine_sounds, index_quirk::offset) {};
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueSound();
         }
   };
   class OpcodeArgValueTrigger : public OpcodeArgValueBaseIndex {
      public:
         OpcodeArgValueTrigger() : OpcodeArgValueBaseIndex("Trigger", Limits::max_triggers, index_quirk::reference) {};
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueTrigger();
         }
   };

   static_assert(cobb::bitcount(Limits::max_script_widgets - 1) == 2, "Incorrect bitlength for Widget Index opcode arg values!"); // KSoft.Tool specifies this explicitly for some reason
   class OpcodeArgValueWidget : public OpcodeArgValueBaseIndex {
      public:
         OpcodeArgValueWidget() : OpcodeArgValueBaseIndex("HUD Widget", Limits::max_script_widgets, index_quirk::presence) {};
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueWidget();
         }
   };
   
   class OpcodeArgValueIconIndex6Bits : public OpcodeArgValueBaseIndex {
      public:
         OpcodeArgValueIconIndex6Bits() : OpcodeArgValueBaseIndex("Icon", cobb::bitmax(6), index_quirk::presence) {};
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueIconIndex6Bits();
         }
   };
   class OpcodeArgValueIconIndex7Bits : public OpcodeArgValueBaseIndex {
      public:
         OpcodeArgValueIconIndex7Bits() : OpcodeArgValueBaseIndex("Icon", cobb::bitmax(7), index_quirk::presence) {};
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueIconIndex7Bits();
         }
   };
}