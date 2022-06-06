#pragma once
#include <type_traits>
#include "../member.h"

namespace cobb::reflex::member_concepts {
   template<typename T> concept type_supports_metadata = requires {
      typename T::metadata_type;
      { T::metadata } -> std::same_as<const typename T::metadata_type&>;
   };
   template<typename T> concept type_has_metadata = requires {
      requires type_supports_metadata<T>;
      requires !std::is_same_v<std::decay_t<typename T::metadata_type>, no_member_metadata>;
      requires !std::is_same_v<std::decay_t<typename T::metadata_type>, void>;
   };
}