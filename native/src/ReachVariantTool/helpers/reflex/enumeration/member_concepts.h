#pragma once
#include "../member.h"
#include "../member_range.h"
#include "../nested_enum.h"

namespace cobb::reflex::impl::enumeration {
   template<typename T> concept is_underlying_type = std::is_integral_v<T> && std::is_arithmetic_v<T>;

   template<typename T> concept is_valid_member = is_member<T> || is_member_range<T> || is_nested_enum<T>;
   template<typename T> concept is_named_member = requires {
      requires (is_member<T> || is_member_range<T> || is_nested_enum<T>);
      { T::name };
   };
}