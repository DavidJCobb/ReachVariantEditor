#pragma once
#include <array>
#include <string>
#include "./localization_language.h"

namespace halo {
   class loc_string_base {
      public:
         static constexpr size_t translation_count = localization_language_count;

         std::array<std::string, translation_count> translations; // UTF-8

         bool operator==(const loc_string_base&) const = default;
         bool operator!=(const loc_string_base&) const = default;

         bool can_be_forge_label() const;
         bool empty() const;
   };
}