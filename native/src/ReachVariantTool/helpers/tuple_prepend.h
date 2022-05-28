#pragma once
#include <tuple>

namespace cobb {
   namespace impl::tuple_prepend {
      template<typename, typename> struct prepend;
      template<typename Prepend, typename... Prior> struct prepend<Prepend, std::tuple<Prior...>> {
         using type = std::tuple<Prepend, Prior...>;
      };

      template<typename A, typename B> using prepend_t = typename prepend<A, B>::type;
   }

   template<typename Prepend, typename Prior> using tuple_prepend = impl::tuple_prepend::prepend_t<Prepend, Prior>;
}