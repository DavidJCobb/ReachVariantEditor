#pragma once
#include <limits>

namespace cobb {
   template<typename First, typename... Rest> requires (sizeof...(Rest) > 0)
   constexpr First min(First x, Rest... args) {
      auto v = std::numeric_limits<First>::max();
      ((v = args < v ? args : v), ...);
      return v;
   }
   template<typename First, typename... Rest> requires (sizeof...(Rest) > 0)
   constexpr First max(First x, Rest... args) {
      auto v = std::numeric_limits<First>::lowest();
      ((v = args > v ? args : v), ...);
      return v;
   }

   namespace impl::numeric_min_max {
      template<typename T> concept collection = requires(const T& list) {
      typename T::value_type;
         { list.begin() };
         { list.end() };
         { ++(list.begin()) };
      };
   }

   template<impl::numeric_min_max::collection Collection>
   constexpr typename Collection::value_type min(const Collection& list) {
      auto v = std::numeric_limits<typename Collection::value_type>::max();
      for (auto item : list)
         if (item < v)
            v = item;
      return v;
   }

   template<impl::numeric_min_max::collection Collection>
   constexpr typename Collection::value_type max(const Collection& list) {
      auto v = std::numeric_limits<typename Collection::value_type>::lowest();
      for (auto item : list)
         if (item > v)
            v = item;
      return v;
   }
}