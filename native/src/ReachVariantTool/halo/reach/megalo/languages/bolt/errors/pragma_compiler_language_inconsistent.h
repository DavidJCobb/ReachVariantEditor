#pragma once
#include "./base.h"
#include "../pragma_data/compiler_language.h"

namespace halo::reach::megalo::bolt::errors {
   class pragma_compiler_language_inconsistent : public base {
      public:
         pragma_data::compiler_language prior;
         pragma_data::compiler_language here;
   };
}