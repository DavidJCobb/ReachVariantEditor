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

   template<typename T> concept member_type_supports_metadata = requires {
      typename T::metadata_type;
      { T::metadata } -> std::same_as<const typename T::metadata_type&>;
   };
   template<typename T> concept member_type_has_metadata = requires {
      requires member_type_supports_metadata<T>;
      requires !std::is_same_v<std::decay_t<typename T::metadata_type>, no_member_metadata>;
      requires !std::is_same_v<std::decay_t<typename T::metadata_type>, void>;
   };
}