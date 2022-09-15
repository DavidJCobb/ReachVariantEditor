#pragma once
#include <cstdint>
#include "helpers/polyfills/msvc/consteval_compatible_optional.h"
#include "helpers/tuple_prepend.h"
#include "helpers/tuple_unpack.h"

namespace cobb::reflex4 {
   using member_value = std::intmax_t;

   struct member {
      using optional_value_type = cobb::polyfills::msvc::consteval_compatible_optional<member_value>;

      consteval member() {}
      consteval member(const char* const n) : name(n) {}
      consteval member(const char* const n, member_value v) : name(n), value(v) {}

      constexpr bool operator==(const member&) const noexcept = default;

      const char* name = nullptr;
      optional_value_type value;
   };

   // Creates a gap in an enum, without having to manually set values to do it.
   struct member_gap {};
   
   // Define a named enum member that represents multiple consecutive values.
   struct member_range {
      consteval member_range() {}
      consteval member_range(const char* n, size_t c) : name(n), count(c) {}
      consteval member_range(const char* n, size_t c, member_value v) : name(n), count(c), value(v) {}

      constexpr bool operator==(const member_range&) const noexcept = default;

      const char* name = nullptr;
      size_t count = 1;
      member::optional_value_type value;
   };

   // Define an enum member that is itself another enum, i.e. a nested enum.
   template<class Enumeration> struct member_enum {
      using enumeration = Enumeration;

      consteval member_enum() {}
      consteval member_enum(const char* const n) : name(n) {}
      consteval member_enum(const char* const n, member_value v) : name(n), value(v) {}

      constexpr bool operator==(const member_enum&) const noexcept = default;

      const char* name = nullptr;
      member::optional_value_type value;
   };
   template<typename T> concept is_member_enum = requires {
      typename T::enumeration;
         requires std::is_same_v<std::decay_t<T>, member_enum<typename T::enumeration>>;
   };

   // Define an enum member that is itself a nested enumeration, included inline.
   template<typename Underlying, typename... Types> struct member_enum_inline {
      using underlying_type       = Underlying;
      using definition_tuple_type = std::tuple<Types...>;

      consteval member_enum_inline() {}
      consteval member_enum_inline(const char* const n, const definition_tuple_type& m) : name(n), members(m) {}
      consteval member_enum_inline(const char* const n, member_value v, const definition_tuple_type& m) : name(n), value(v), members(m) {}

      constexpr bool operator==(const member_enum_inline&) const noexcept = default;

      const char* name = nullptr;
      member::optional_value_type value;
      definition_tuple_type members;
   };
   template<typename T> concept is_member_enum_inline = requires {
      typename T::underlying_type;
      typename T::definition_tuple_type;
      requires std::is_same_v<
         std::decay<T>,
         cobb::tuple_unpack_t<member_enum_inline, cobb::tuple_prepend<typename T::underlying_type, typename T::definition_tuple_type>>
      >;
   };
}