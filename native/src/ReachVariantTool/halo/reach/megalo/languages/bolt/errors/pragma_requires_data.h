#pragma once
#include "./base.h"
#include "../pragma_type.h"

namespace halo::reach::megalo::bolt::errors {
   // Thrown if the script contains a pragma with a name but no data, given a name 
   // that requires data (e.g. `pragma compiler_language` with no language name).
   class pragma_requires_data : public base {
      public:
         pragma_requires_data(pragma_type t) : type(t) {}

         pragma_type type = pragma_type::unspecified;
   };
}