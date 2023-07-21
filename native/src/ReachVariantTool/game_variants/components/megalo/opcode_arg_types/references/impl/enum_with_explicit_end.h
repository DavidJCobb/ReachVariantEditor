#pragma once

namespace Megalo::references {
   namespace impl {
      template<typename Enum>
      concept enum_with_explicit_end = requires {
         { Enum::__enum_end };
      };
   }
}