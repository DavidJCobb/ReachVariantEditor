#pragma once
#include <cstdint>
#include "helpers/polyfills/msvc/consteval_compatible_optional.h"

namespace cobb::reflex3 {
   using member_value = std::intmax_t;

   struct member {
      using optional_value_type = cobb::polyfills::msvc::consteval_compatible_optional<member_value>;

      consteval member() {}
      consteval member(const char* const n) : name(n) {}
      consteval member(const char* const n, member_value v) : name(n), value(v) {}

      const char* name = nullptr;
      optional_value_type value;
   };

   struct member_gap {};
   
   // Define an enum member that is itself another enum, i.e. a nested enum.
   template<class Enumeration> struct member_enum {
      using enumeration = Enumeration;

      consteval member_enum() {}
      consteval member_enum(const char* const n) : name(n) {}
      consteval member_enum(const char* const n, member_value v) : name(n), value(v) {}

      const char* name = nullptr;
      member::optional_value_type value;
   };
   
   template<typename T> concept is_member_enum = requires {
      typename T::enumeration;
      requires std::is_same_v<std::decay_t<T>, member_enum<typename T::enumeration>>;
   };
   
   // Define a named enum member that represents multiple consecutive values.
   struct member_range {
      consteval member_range() {}
      consteval member_range(const char* n, size_t c) : name(n), count(c) {}
      consteval member_range(const char* n, size_t c, member_value v) : name(n), count(c), value(v) {}

      const char* name = nullptr;
      size_t count = 1;
      member::optional_value_type value;
   };
}