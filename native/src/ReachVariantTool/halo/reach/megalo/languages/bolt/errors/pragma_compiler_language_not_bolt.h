#pragma once
#include "./base.h"
#include "../pragma_data/compiler_language.h"

namespace halo::reach::megalo::bolt::errors {
   class pragma_compiler_language_not_bolt : public base {
      public:
         pragma_data::compiler_language specified = pragma_data::compiler_language::unrecognized;
   };
}