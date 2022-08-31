#pragma once
#include <cstdint>
#include <optional>

namespace cobb::reflex2 {
   using member_value = std::intmax_t;

   struct member {
      consteval member() {}
      consteval member(const char* n) : name(n) {}
      consteval member(const char* n, member_value v) : name(n), value(v) {}

      const char* const name = nullptr;
      std::optional<member_value> value;
   };

   struct member_gap {};
}