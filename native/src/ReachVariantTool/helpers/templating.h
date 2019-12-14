#pragma once

namespace cobb {
   //
   // TEMPLATE CASING
   //
   // If you try to use std::enable_if_t on multiple templates of the same function, it'll fail. 
   // The templates have to be substantially different: different conditions within enable_if_t 
   // aren't enough, but different types within enable_if_t are. This macro automates the process: 
   // you can give each case a number, and the macro will use that number to pick a unique enum-
   // class for the case.
   //
   enum class template_case_1 {};
   enum class template_case_2 {};
   enum class template_case_3 {};
   enum class template_case_4 {};
   enum class template_case_5 {};
   //
   #define cobb_enable_case(num, condition) typename std::enable_if_t<(condition), template_case_##num##> = template_case_##num##()
   //
   /*// Example:
   struct foo {
      template<typename T, cobb_enable_case(1, !std::is_bounded_array_v<T>)> void read(T & out) noexcept {
         out = this->read_bits<T>(cobb::bits_in<T>, std::is_signed_v<T> ? read_flags::is_signed : 0);
      };
      template<typename T, cobb_enable_case(2, std::is_bounded_array_v<T>)> void read(T& out) noexcept {
         for (int i = 0; i < std::extent<T>::value; i++)
            out[i] = this->read_bits<decltype(out[i])>(cobb::bits_in<T>, std::is_signed_v<T> ? read_flags::is_signed : 0);
      };
   };
   //*/
};