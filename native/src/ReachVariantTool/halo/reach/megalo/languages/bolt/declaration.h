#pragma once
#include <optional>
#include <QString>
#include "./_base.h"

namespace halo::reach::megalo::bolt {
   enum class declaration_type {
      variable,
   };

   enum class variable_network_priority {
      local,
      low,
      high,
   };

   class declaration : public item_base {
      public:
         declaration_type type = declaration_type::variable;
         QString name;
         std::optional<variable_network_priority> network_priority;
         std::optional<int32_t> initial_value;
   };
}