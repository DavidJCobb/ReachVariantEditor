#pragma once
#include "limits.h"
#include "../../helpers/bitnumber.h"

namespace Megalo {
   using condition_index = cobb::bitnumber<cobb::bitcount(Limits::max_conditions - 1), uint16_t>;
   using condition_count = cobb::bitnumber<cobb::bitcount(Limits::max_conditions), uint16_t>;
   using action_index    = cobb::bitnumber<cobb::bitcount(Limits::max_actions - 1), uint16_t>;
   using action_count    = cobb::bitnumber<cobb::bitcount(Limits::max_actions), uint16_t>;
   //
   using const_team_index = cobb::bitnumber<cobb::bitcount((int)const_team::_count - 1), const_team, true>;
   //
   using object_type_index_optional = cobb::bitnumber<cobb::bitcount(Megalo::Limits::max_object_types), int32_t, true>;
   using forge_label_index = cobb::bitnumber<cobb::bitcount(Limits::max_script_labels), int32_t, true>;
}