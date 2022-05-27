#pragma once
#include <optional>
#include <type_traits>

namespace cobb {
   template<typename T> concept is_std_optional_type = requires {
      typename T::value_type;
      requires std::is_same_v<T, std::optional<typename T::value_type>>;
   };
}