#pragma once
#include <optional>
#include <QString>
#include "./_base.h"
#include "../../variable_network_priority.h"

namespace halo::reach::megalo::bolt {
   class expression;
   class identifier;
}

namespace halo::reach::megalo::bolt {
   enum class declaration_type {
      variable,
   };

   class declaration : public item_base {
      public:
         declaration_type type = declaration_type::variable;
         cobb::owned_ptr<identifier> ident;
         cobb::owned_ptr<expression> initial_value;
         std::optional<variable_network_priority> network_priority;
   };
}