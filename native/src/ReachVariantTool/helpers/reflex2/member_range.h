#pragma once
#include <cstdint>
#include "helpers/polyfills/msvc/consteval_compatible_optional.h"
#include "./member.h"

namespace cobb::reflex2 {
   // Define a named enum member that represents multiple consecutive values.
   struct member_range {
      consteval member_range() {}
      consteval member_range(const char* n, size_t c) : name(n), count(c) {}
      consteval member_range(const char* n, size_t c, member_value v) : name(n), count(c), value(v) {}

      const char* name = nullptr;
      size_t count = 1;
      cobb::polyfills::msvc::consteval_compatible_optional<member_value> value;
   };
}