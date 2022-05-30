#pragma once
#include <cstdint>
#include <limits>

namespace cobb::reflex {
   using compile_time_value_type = intmax_t;

   constexpr compile_time_value_type undefined = std::numeric_limits<compile_time_value_type>::lowest();

   using default_underlying_enum_type = int;
}
