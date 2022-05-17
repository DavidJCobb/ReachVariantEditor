#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   //
   // The true meaning of this player trait is unknown. Assembly identifies it as "Double Jump," 
   // but I can't seem to get it to do anything in tests, no matter what I set it to. One thing 
   // to note is that while Assembly lists it as having three distinct values, disassembly of 
   // the game will turn up code to validate player traits, and this code allows up to four 
   // distinct values.
   //
   enum class double_jump : uint8_t {
      unchanged = 0,
      disabled  = 1,
      normal    = 2, // "default"? guessed
      enabled   = 3, // guessed
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::double_jump> {
      using trait = halo::reach::traits::double_jump;
      static constexpr auto data = halo::trait_information({
         .min = (int)trait::unchanged,
         .max = 3,
      });
   };
}