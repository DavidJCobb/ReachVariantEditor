#pragma once
#include "helpers/reflex_enum/reflex_flags.h"
#include "./base_flags_operand.h"

namespace halo::reach::megalo::operands {
   using create_object_flags = base_flags_operand<
      cobb::cs("create_object_flags"),
      cobb::reflex_flags<
         cobb::reflex_enum_member<cobb::cs("never_garbage_collect")>,
         cobb::reflex_enum_member<cobb::cs("suppress_effect")>,
         cobb::reflex_enum_member<cobb::cs("absolute_orientation")>
      >
   >;
   using killer_types = base_flags_operand<
      cobb::cs("killer_types"),
      cobb::reflex_flags<
         cobb::reflex_enum_member<cobb::cs("guardians")>,
         cobb::reflex_enum_member<cobb::cs("suicide")>,
         cobb::reflex_enum_member<cobb::cs("kill")>,
         cobb::reflex_enum_member<cobb::cs("betrayal")>,
         cobb::reflex_enum_member<cobb::cs("quit")>
      >
   >;
   using player_req_purchase_modes = base_flags_operand<
      cobb::cs("player_req_purchase_modes"),
      cobb::reflex_flags<
         cobb::reflex_enum_member<cobb::cs("alive_weapons")>,
         cobb::reflex_enum_member<cobb::cs("alive_equipment")>,
         cobb::reflex_enum_member<cobb::cs("alive_vehicles")>,
         cobb::reflex_enum_member<cobb::cs("dead_weapons")>,
         cobb::reflex_enum_member<cobb::cs("dead_equipment")>
      >
   >;
}
