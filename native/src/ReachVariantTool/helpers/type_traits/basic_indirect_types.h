#pragma once
#include <type_traits>

namespace cobb {
   template<typename T> struct basic_indirect_types {
      using type            = void;
      using reference       = void;
      using const_reference = void;
      using move_reference  = void;
   };

   template<typename T> requires (!std::is_same_v<T, void>) struct basic_indirect_types<T> {
      using type            = T;
      using reference       = T&;
      using const_reference = const T&;
      using move_reference  = T&&;
   };
}
