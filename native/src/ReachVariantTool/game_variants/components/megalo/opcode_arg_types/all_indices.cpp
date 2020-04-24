#include "all_indices.h"

namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueRequisitionPalette::typeinfo = OpcodeArgTypeinfo(
      "_requisition_palette",
      "Requisition Palette",
      "A remnant of a scrapped feature.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueRequisitionPalette>
   );

   OpcodeArgTypeinfo OpcodeArgValueTrigger::typeinfo = OpcodeArgTypeinfo(
      "_trigger_index",
      "Trigger Index",
      "This type should only be used for running nested triggers; the compiler should manage it invisibly. How did you end up seeing it?",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueTrigger>
   );
}