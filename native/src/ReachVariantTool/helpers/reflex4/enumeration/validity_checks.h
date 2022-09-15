#pragma once

namespace cobb::reflex4::impl::enumeration {
   struct validity_check_results_t {
      bool members_tuple_has_only_relevant_types = true;
      bool names_are_not_blank = true;
      bool names_are_unique    = true;
   };
}