#pragma once

namespace halo {
   enum class localization_language {
      english,
      japanese,
      german,
      french,
      spanish,
      mexican,
      italian,
      korean,
      chinese_traditional,
      chinese_simplified,
      portugese,
      polish, // optional
   };

   constexpr size_t localization_language_count = 12;
}