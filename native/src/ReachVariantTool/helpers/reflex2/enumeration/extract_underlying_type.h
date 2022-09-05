#pragma once
#include "./enumeration_data_forward_declare.h"

namespace cobb::reflex2::impl::enumeration {
   template<class Enumeration> concept has_explicit_underlying_type = requires {
      typename enumeration_data<Enumeration>;
      typename enumeration_data<Enumeration>::underlying_type;
   };

   template<class Enumeration> struct extract_underlying_type;
   template<class Enumeration> requires (has_explicit_underlying_type<Enumeration>) struct extract_underlying_type<Enumeration> {
      using type = typename enumeration_data<Enumeration>::underlying_type;
   };
   template<class Enumeration> requires (!has_explicit_underlying_type<Enumeration>) struct extract_underlying_type<Enumeration> {
      using type = int;
   };
}