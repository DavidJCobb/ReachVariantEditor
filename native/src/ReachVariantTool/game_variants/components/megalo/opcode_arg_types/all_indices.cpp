#include "all_indices.h"

namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueRequisitionPalette::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueRequisitionPalette>
   );

   OpcodeArgTypeinfo OpcodeArgValueTrigger::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueTrigger>
   );

   OpcodeArgTypeinfo OpcodeArgValueIconIndex6Bits::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueIconIndex6Bits>
   );

   OpcodeArgTypeinfo OpcodeArgValueIconIndex7Bits::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueIconIndex7Bits>
   );
}