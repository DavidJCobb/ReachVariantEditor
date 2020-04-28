/*

This file is provided under the Creative Commons 0 License.
License: <https://creativecommons.org/publicdomain/zero/1.0/legalcode>
Summary: <https://creativecommons.org/publicdomain/zero/1.0/>

One-line summary: This file is public domain or the closest legal equivalent.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
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
   // Like, this doesn't work:
   //
   // class foo : public some_bit_reader {
   //    template<typename T, std::enable_if_t<!std::is_bounded_array_v<T>>> void read(T& out) noexcept {
   //       out = this->read_bits<T>(cobb::bits_in<T>, std::is_signed_v<T> ? read_flags::is_signed : 0);
   //    };
   //    template<typename T, std::enable_if_t<std::is_bounded_array_v<T>>> void read(T& out) noexcept {
   //       using item_type = std::remove_extent_t<T>; // from X[i] to X
   //       for (int i = 0; i < std::extent<T>::value; i++)
   //          out[i] = this->read_bits<item_type>(cobb::bits_in<item_type>, std::is_signed_v<item_type> ? read_flags::is_signed : 0);
   //    };
   // };
   //
   // Overload resolution on that fails because both enable_if_t occurrences can resolve to the 
   // same type; sometimes, that means that one of these templates never instantiates, and other 
   // times, it means that the two function overloads conflict with each other (since they have 
   // the same signature). Fortunately, enable_if_t allows you to optionally specify what type it 
   // resolves to if it succeeds, which is how the helper macro here is able to prevent both of 
   // those problem-cases from arising.
   //
   #define cobb_enable_case(num, condition) typename std::enable_if_t<(condition), cobb::template_case_##num##> = cobb::template_case_##num##()
   //
   enum class template_case_1 {};
   enum class template_case_2 {};
   enum class template_case_3 {};
   enum class template_case_4 {};
   enum class template_case_5 {};
   //
   /*// Example:
   struct foo {
      template<typename T, cobb_enable_case(1, !std::is_bounded_array_v<T>)> void read(T & out) noexcept {
         out = this->read_bits<T>(cobb::bits_in<T>, std::is_signed_v<T> ? read_flags::is_signed : 0);
      };
      template<typename T, cobb_enable_case(2, std::is_bounded_array_v<T>)> void read(T& out) noexcept {
         using item_type = std::remove_extent_t<T>; // from X[i] to X
         for (int i = 0; i < std::extent<T>::value; i++)
            out[i] = this->read_bits<item_type>(cobb::bits_in<item_type>, std::is_signed_v<item_type> ? read_flags::is_signed : 0);
      };
   };
   //*/
};