#pragma once
#include <cstdint>
#include "helpers/polyfills/msvc/consteval_compatible_optional.h"

namespace cobb::reflex2 {
   using member_value = std::intmax_t;

   struct member {
      consteval member() {}
      consteval member(const char* const n) : name(n) {}
      consteval member(const char* const n, member_value v) : name(n), value(v) {}

      const char* name = nullptr;
      cobb::polyfills::msvc::consteval_compatible_optional<member_value> value;
   };

   struct member_gap {};
}