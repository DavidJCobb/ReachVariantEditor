#pragma once
#include <type_traits>

namespace cobb {
   //
   // A helper for creating a recursive lambda. Usage:
   //
   //    auto lambda = y_combinator{[](auto self, arg, arg){ code; }};
   //    lambda();
   //
   template<typename Lambda> struct y_combinator {
      Lambda lambda;

      template<typename... Args> constexpr decltype(auto) operator()(Args&&... args) const {
         return this->lambda(*this, std::forward<Args>(args)...);
      }
      template<typename... Args> constexpr decltype(auto) operator()(Args&&... args) {
         this->lambda((*this), std::forward<Args>(args)...);
      }
   };
   template<class Lambda> y_combinator(Lambda) -> y_combinator<Lambda>;
}