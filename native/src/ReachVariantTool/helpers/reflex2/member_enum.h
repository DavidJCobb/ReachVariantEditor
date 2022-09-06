#pragma once
#include "helpers/polyfills/msvc/consteval_compatible_optional.h"
#include "./member.h"

namespace cobb::reflex2 {
   // Define an enum member that is itself another enum, i.e. a nested enum.
   template<class Enumeration> struct member_enum {
      using enumeration = Enumeration;

      consteval member_enum() {}
      consteval member_enum(const char* const n) : name(n) {}
      consteval member_enum(const char* const n, member_value v) : name(n), value(v) {}

      const char* name = nullptr;
      cobb::polyfills::msvc::consteval_compatible_optional<member_value> value;
   };
   
   template<typename T> concept is_member_enum = requires {
      typename T::enumeration;
      requires std::is_same_v<std::decay_t<T>, member_enum<typename T::enumeration>>;
   };
}