#pragma once
#include <array>
#include <type_traits>

//
// MSVC bug as of Sept. 2022: const reference NTTPs are lvalues but all type 
// traits see their decltypes as pointers; compiler error messages in specific 
// situations (involving tons of nearby templates) indicate that MSVC passes a 
// pointer even when you don't use the address-of operator.
//

namespace cobb::polyfills::msvc {
   namespace impl::nttp_decltype {
      template<const auto& Value> constexpr const bool is_broken = []() {
         return std::is_pointer_v<decltype(Value)>;
      }();

      constexpr std::array<int, 5> foo = {};
   }

   template<auto V> using nttp_decltype =
      #if !defined(__INTELLISENSE__) // IntelliSense is not bugged; only the compiler
      std::conditional_t<
         impl::nttp_decltype::is_broken<impl::nttp_decltype::foo>, // only tamper with the types if the bug still occurs
         std::remove_pointer_t<decltype(V)>,
      #endif
         decltype(V)
      #if !defined(__INTELLISENSE__)
      >
      #endif
      ;
}
