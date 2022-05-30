#pragma once

namespace cobb {
   namespace impl {
      template<auto Value> struct is_constexpr_default_constructible_worker {};
   }
   template<typename T> concept is_constexpr_default_constructible = requires {
      { impl::is_constexpr_default_constructible_worker<T{}>() };
   };
}