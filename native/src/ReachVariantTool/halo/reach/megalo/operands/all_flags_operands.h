#pragma once
#include "helpers/reflex/flags_mask.h"
#include "./base_flags_operand.h"

namespace halo::reach::megalo::operands {
   using create_object_flags = base_flags_operand<
      cobb::cs("create_object_flags"),
      cobb::reflex::flags_mask<
         cobb::reflex::member<cobb::cs("never_garbage_collect")>,
         cobb::reflex::member<cobb::cs("suppress_effect")>,
         cobb::reflex::member<cobb::cs("absolute_orientation")>
      >
   >;
   using killer_types = base_flags_operand<
      cobb::cs("killer_types"),
      cobb::reflex::flags_mask <
         cobb::reflex::member<cobb::cs("guardians")>,
         cobb::reflex::member<cobb::cs("suicide")>,
         cobb::reflex::member<cobb::cs("kill")>,
         cobb::reflex::member<cobb::cs("betrayal")>,
         cobb::reflex::member<cobb::cs("quit")>
      >
   >;
   using player_req_purchase_modes = base_flags_operand<
      cobb::cs("player_req_purchase_modes"),
      cobb::reflex::flags_mask <
         cobb::reflex::member<cobb::cs("alive_weapons")>,
         cobb::reflex::member<cobb::cs("alive_equipment")>,
         cobb::reflex::member<cobb::cs("alive_vehicles")>,
         cobb::reflex::member<cobb::cs("dead_weapons")>,
         cobb::reflex::member<cobb::cs("dead_equipment")>
      >
   >;
}
