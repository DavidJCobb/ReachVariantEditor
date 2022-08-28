#pragma once

namespace halo::reach::megalo::bolt {
   enum class pragma_type {

      unspecified,
      unrecognized,

      compiler_language,
   };

   constexpr bool pragma_type_requires_data(pragma_type t) {
      switch (t) {
         case pragma_type::compiler_language:
            return true;
      }
      return false;
   }
}