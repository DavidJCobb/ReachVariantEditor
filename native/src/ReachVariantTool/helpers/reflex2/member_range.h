#pragma once
#include <cstdint>
#include <optional>
#include "./member.h"

namespace cobb::reflex2 {
   struct member_range {
      consteval member_range() {}
      consteval member_range(const char* n, size_t c) : name(n), count(c) {}
      consteval member_range(const char* n, size_t c, member_value v) : name(n), count(c), value(v) {}

      const char* const name = nullptr;
      size_t count = 1;
      std::optional<member_value> value;
   };
}