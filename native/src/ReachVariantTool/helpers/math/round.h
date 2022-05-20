#pragma once
#include <cstdint>
#include <limits>
#include <type_traits>
#include "./sign.h"

namespace cobb {
   namespace impl::round {
      // constant defined here in an attempt to reduce constexpr steps and avoid hitting compiler safety limits
      template<typename T> static constexpr T near_half = T(0.5) - std::numeric_limits<T>::epsilon();
   }

   namespace ct {
      template<typename T> requires std::is_floating_point_v<T> constexpr int64_t round(T v) {
         return v + ::cobb::impl::round::near_half<T> *sign(v); // return value is int, so we truncate after this
      }
   }

   template<typename T> requires std::is_floating_point_v<T> constexpr int64_t round(T v) {
      if (!std::is_constant_evaluated()) {
         return ::round(v);
      }
      return ct::round(v);
   }
}