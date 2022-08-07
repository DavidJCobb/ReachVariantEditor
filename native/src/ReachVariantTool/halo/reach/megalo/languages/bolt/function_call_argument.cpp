#include "function_call_argument.h"

namespace halo::reach::megalo::bolt {
   function_call_argument::function_call_argument(function_call_argument&& other) noexcept {
      *this = std::move(other);
   }
   function_call_argument& function_call_argument::operator=(function_call_argument&& other) noexcept {
      item_base::operator=(std::move(other));
      std::swap(this->name,  other.name);
      std::swap(this->value, other.value);
      return *this;
   }
}