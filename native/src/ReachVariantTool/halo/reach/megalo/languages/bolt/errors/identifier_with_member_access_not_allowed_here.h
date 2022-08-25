#pragma once
#include "./base.h"
#include "../syntax_element.h"

namespace halo::reach::megalo::bolt {
   class identifier;
}

namespace halo::reach::megalo::bolt::errors {
   class identifier_with_member_access_not_allowed_here : public base {
      public:
         identifier_with_member_access_not_allowed_here(syntax_element e, const identifier& s) : affected_element(e), subject(&s) {
            this->pos = s.start;
         }
         identifier_with_member_access_not_allowed_here(syntax_element e) : affected_element(e) {} // use if you don't plan on retaining the identifier somewhere

         syntax_element affected_element;
         const identifier* subject = nullptr;
   };
}