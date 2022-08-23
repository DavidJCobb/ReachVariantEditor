#pragma once
#include <variant>
#include "./base.h"
#include "../syntax_element.h"
#include "../token.h"

namespace halo::reach::megalo::bolt {
   class block;
}

namespace halo::reach::megalo::bolt::errors {
   class missing_syntax_element : public base {
      public:
         using subject_variant_type = std::variant<
            std::nullptr_t,
            block*
         >;

      public:
         missing_syntax_element(syntax_element e) : missing_element(e) {}

         syntax_element missing_element;
         token fault_token;
         subject_variant_type subject = nullptr;
   };
}