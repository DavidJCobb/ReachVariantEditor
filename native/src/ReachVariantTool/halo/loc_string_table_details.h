#pragma once

namespace halo::loc_string_table_details {
   using offset_type = int16_t; // same as used by the game internally
   using offset_list = std::array<offset_type, localization_language_count>;
   
   enum class optimization_type {
      none,
      bungie,     // only optimize strings when all localizations are identical
      duplicates, // optimize any identical localizations, even across different strings
   };

   constexpr optimization_type optimization = optimization_type::duplicates;
}