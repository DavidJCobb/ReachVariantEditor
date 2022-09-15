#pragma once
#include <array>
#include <type_traits>

//
// Workaround for: https://developercommunity.visualstudio.com/t/const-auto-template-parameters-NTTP-u/10139694
// 
// In short: the compiler passes const-reference NTTPs as pointers and simply 
// pretends that they're references. This means you don't have to use pointer 
// syntax in your code; however, type traits still treat the value as if it 
// were a pointer.
//

namespace cobb::polyfills::msvc {
   namespace impl::nttp_decltype {
      template<const auto& Value> constexpr const bool is_broken = []() {
         return std::is_pointer_v<decltype(Value)>;
      }();

      constexpr std::array<int, 5> foo = {};
   }

   template<const auto& V> using nttp_decltype =
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
