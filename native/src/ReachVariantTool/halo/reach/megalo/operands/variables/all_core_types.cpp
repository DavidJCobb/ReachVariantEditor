#include "all_core_types.h"

namespace halo::reach::megalo::operands {
   extern variables::unknown_type* make_variable_of_type(variable_type v) {
      switch (v) {
         case variable_type::number: return new variables::number;
         case variable_type::object: return new variables::object;
         case variable_type::player: return new variables::player;
         case variable_type::team:   return new variables::team;
         case variable_type::timer:  return new variables::timer;
      }
      if (std::is_constant_evaluated()) {
         throw;
      }
      return nullptr;
   }
}