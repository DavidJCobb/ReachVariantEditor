
// test for compile

#include "bitnumber.h"

namespace halo::util::test {
   f_bitnumber<bitnumber_params<uint16_t>{
      .bitcount = 2,
   }> xyz;


   using xyz_t = decltype(xyz);
   using xyz_under = xyz_t::underlying_type;
   using xyz_underi = xyz_t::underlying_int;

   constexpr auto xyz_params = bitnumber_params<uint16_t>{
      .bitcount = 2,
   };
   constexpr auto presence = xyz_params.if_absent.has_value();
   constexpr auto foo = xyz_params.if_absent.value();
}