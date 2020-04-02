#include "all_indices.h"

namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueRequisitionPalette::typeinfo = OpcodeArgTypeinfo(
      "_requisition_palette",
      "Requisition Palette",
      "A remnant of a scrapped feature.",
      //
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueRequisitionPalette>
   );

   OpcodeArgTypeinfo OpcodeArgValueTrigger::typeinfo = OpcodeArgTypeinfo(
      "_trigger_index",
      "Trigger Index",
      "This type should only be used for running nested triggers; the compiler should manage it invisibly. How did you end up seeing it?",
      //
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueTrigger>
   );

   OpcodeArgTypeinfo OpcodeArgValueIconIndex6Bits::typeinfo = OpcodeArgTypeinfo(
      "_icon_index_6",
      "Icon Index, 6-Bit",
      "An icon.",
      //
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueIconIndex6Bits>
   );

   OpcodeArgTypeinfo OpcodeArgValueIconIndex7Bits::typeinfo = OpcodeArgTypeinfo(
      "_icon_index_7",
      "Icon Index, 7-Bit",
      "An icon.",
      //
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueIconIndex7Bits>
   );
}