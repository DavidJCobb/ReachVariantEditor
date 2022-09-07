#pragma once

namespace cobb::reflex3::impl::enumeration {
   struct validity_check_results_t {
      bool members_tuple_has_only_relevant_types = true;
      bool names_are_not_blank = true;
      bool names_are_unique    = true;
      bool nested_enums_not_recursive = true;
   };
}