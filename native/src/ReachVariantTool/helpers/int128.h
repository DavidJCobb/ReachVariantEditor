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
#include <cstdint>
#include <string>
#include <intrin.h>

namespace cobb {
   #ifdef __SIZEOF_INT128__
      using uint128_t = __int128;
   #else
      #ifndef _M_X64
         #error This implementation of 128-bit integers is only compatible with x64.
      #endif
      class uint128_t {
         //
         // 128-bit integer type using intrinsics available in MSVC.
         //
         protected:
            uint64_t a; // high
            uint64_t b; // low
            //
         public:
            uint128_t() {}
            uint128_t(uint64_t v) : a(0), b(v) {}
            uint128_t(uint64_t high, uint64_t low) : a(high), b(low) {}
            //
            inline uint64_t& _high() noexcept { return this->a; }
            inline uint64_t& _low()  noexcept { return this->b; }
            inline const uint64_t& _high() const noexcept { return this->a; }
            inline const uint64_t& _low()  const noexcept { return this->b; }
            //
            inline uint128_t& operator=(const uint128_t& v) noexcept {
               this->a = v.a;
               this->b = v.b;
               return *this;
            }
            inline uint128_t& operator=(uint64_t v) noexcept {
               this->a = 0;
               this->b = v;
               return *this;
            }
            //
            #pragma region Arithmetic assign operators
            inline uint128_t& operator+=(const uint128_t& v) noexcept {
               uint8_t CF = 0; // carry flag
               CF = _addcarry_u64(CF, this->b, v.b, &this->b); // sets CF to 1 if it overflows
               CF = _addcarry_u64(CF, this->a, v.a, &this->a); // this->a += v.a + CF;
               return *this;
            }
            inline uint128_t& operator+=(uint64_t v) noexcept {
               uint8_t CF = 0;
               CF = _addcarry_u64(CF, this->b, v, &this->b); // sets CF to 1 if it overflows
               if (CF)
                  ++this->a;
               return *this;
            }
            inline uint128_t& operator-=(const uint128_t& v) noexcept {
               uint8_t BF = 0; // borrow flag
               BF = _subborrow_u64(BF, this->b, v.b, &this->b); // sets BF to 1 if it underflows
               BF = _subborrow_u64(BF, this->a, v.a, &this->a); // this->a -= v.a + BF;
               return *this;
            }
            inline uint128_t& operator-=(uint64_t v) noexcept {
               uint8_t BF = 0;
               BF = _subborrow_u64(BF, this->b, v, &this->b); // sets BF to 1 if it underflows
               if (BF)
                  --this->a;
               return *this;
            }
            inline uint128_t& operator*=(uint64_t operand) noexcept {
               static_assert(sizeof(long long) == sizeof(uint64_t), "MSVC doesn't consider (long long*) and (uint64_t*) interchangeable so we need to cast, but apparently that's wrong in your build environment.");
               uint64_t high;
               this->b = _umul128(this->b, operand, (unsigned long long*)&high); // use _mul128 for signed
               this->a *= operand;
               this->a += high;
               return *this;
            }
            inline uint128_t& operator/=(uint64_t operand) noexcept {
               uint64_t remainder;
               this->b = _udiv128(this->a, this->b, operand, &remainder); // use _div128 for signed
               this->a = 0;
               return *this;
            }
            inline uint128_t& operator%=(uint64_t operand) noexcept {
               _udiv128(this->a, this->b, operand, &this->b); // use _div128 for signed
               this->a = 0;
               return *this;
            }
            #pragma endregion
            #pragma region Bitwise assign operators
            inline uint128_t& operator&=(const uint128_t& by) noexcept {
               this->b &= by.b;
               this->a &= by.a;
               return *this;
            }
            inline uint128_t& operator&=(uint64_t by) noexcept {
               this->b &= by;
               this->a = 0;
               return *this;
            }
            inline uint128_t& operator|=(const uint128_t& by) noexcept {
               this->b |= by.b;
               this->a |= by.a;
               return *this;
            }
            inline uint128_t& operator|=(uint64_t by) noexcept {
               this->b |= by;
               return *this;
            }
            inline uint128_t& operator>>=(uint8_t by) noexcept {
               if (by == 0x40) { // __shiftright128 does not handle this properly
                  this->b = this->a;
                  this->a = 0;
                  return *this;
               }
               this->b = __shiftright128(this->b, this->a, by);
               this->a >>= by;
               return *this;
            }
            inline uint128_t& operator<<=(uint8_t by) noexcept {
               if (by == 0x40) { // __shiftleft128 does not handle this properly
                  this->a = this->b;
                  this->b = 0;
                  return *this;
               }
               this->a = __shiftleft128(this->b, this->a, by);
               this->b <<= by;
               return *this;
            }
            #pragma endregion
            #pragma region Increment/decrement operators
            uint128_t& operator++() noexcept { // ++foo
               *this += 1;
               return *this;
            }
            uint128_t operator++(int) noexcept { // foo++
               return *this + 1;
            }
            uint128_t& operator--() noexcept { // --foo
               *this -= 1;
               return *this;
            }
            uint128_t operator--(int) noexcept { // foo--
               return *this - 1;
            }
            #pragma endregion
            //
            #pragma region Arithmetic operators
            inline uint128_t operator+(const uint128_t& v) const noexcept {
               uint128_t result = *this;
               return result += v;
            }
            inline uint128_t operator+(uint64_t v) const noexcept {
               uint128_t result = *this;
               return result += v;
            }
            inline uint128_t operator-(const uint128_t& v) const noexcept {
               uint128_t result = *this;
               return result -= v;
            }
            inline uint128_t operator-(uint64_t v) const noexcept {
               uint128_t result = *this;
               return result -= v;
            }
            inline uint128_t operator*(uint64_t v) const noexcept {
               uint128_t result = *this;
               return result *= v;
            }
            inline uint128_t operator/(uint64_t v) const noexcept {
               uint128_t result = *this;
               return result /= v;
            }
            inline uint128_t operator%(uint64_t v) const noexcept {
               uint128_t result = *this;
               return result %= v;
            }
            #pragma endregion
            #pragma region Bitwise operators
            inline uint128_t operator&(const uint128_t& v) const noexcept {
               uint128_t result = *this;
               return result &= v;
            }
            inline uint128_t operator&(uint64_t v) const noexcept {
               uint128_t result = *this;
               return result &= v;
            }
            inline uint128_t operator|(const uint128_t& v) const noexcept {
               uint128_t result = *this;
               return result |= v;
            }
            inline uint128_t operator|(uint64_t v) const noexcept {
               uint128_t result = *this;
               return result |= v;
            }
            inline uint128_t operator>>(const uint128_t& v) const noexcept {
               if (v.a)
                  return 0;
               uint128_t result = *this;
               return result >>= v.b;
            }
            inline uint128_t operator>>(uint8_t v) const noexcept {
               uint128_t result = *this;
               return result >>= v;
            }
            inline uint128_t operator<<(const uint128_t& v) const noexcept {
               if (v.a)
                  return 0;
               uint128_t result = *this;
               return result <<= v.b;
            }
            inline uint128_t operator<<(uint8_t v) const noexcept {
               uint128_t result = *this;
               return result <<= v;
            }
            inline bool operator!() const noexcept {
               return *this == 0;
            }
            #pragma endregion
            //
            #pragma region Comparisons with uint64_t
            inline bool operator<(uint64_t v) const noexcept {
               if (this->a)
                  return false;
               return this->b < v;
            }
            inline bool operator>(uint64_t v) const noexcept {
               if (this->a)
                  return true;
               return this->b > v;
            }
            inline bool operator<=(uint64_t v) const noexcept {
               if (this->a)
                  return false;
               return this->b <= v;
            }
            inline bool operator>=(uint64_t v) const noexcept {
               if (this->a)
                  return true;
               return this->b >= v;
            }
            inline bool operator==(uint64_t v) const noexcept {
               if (this->a)
                  return false;
               return this->b == v;
            }
            inline bool operator!=(uint64_t v) const noexcept {
               if (this->a)
                  return true;
               return this->b != v;
            }
            #pragma endregion
            #pragma region Comparisons with uint128_t
            inline bool operator<(const uint128_t& v) const noexcept {
               if (this->a == v.a)
                  return this->b < v.b;
               return this->a < v.a;
            }
            inline bool operator>(const uint128_t& v) const noexcept {
               if (this->a == v.a)
                  return this->b > v.b;
               return this->a > v.a;
            }
            inline bool operator<=(const uint128_t& v) const noexcept {
               if (this->a == v.a)
                  return this->b <= v.b;
               return this->a < v.a;
            }
            inline bool operator>=(const uint128_t& v) const noexcept {
               if (this->a == v.a)
                  return this->b >= v.b;
               return this->a > v.a;
            }
            inline bool operator==(const uint128_t& v) const noexcept {
               return this->a == v.a && this->b == v.b;
            }
            inline bool operator!=(const uint128_t& v) const noexcept {
               return this->a != v.a || this->b != v.b;
            }
            #pragma endregion
            //
            inline uint64_t div_and_get_remainder(uint64_t operand) { // divides and modifies (this), but retains the remainder
               uint64_t remainder;
               this->b = _udiv128(this->a, this->b, operand, &remainder); // use _div128 for signed
               this->a = 0;
               return remainder;
            }
            void to_hex(std::string& out) const noexcept;
            void to_string(std::string& out) const noexcept;
            //
            inline explicit operator uint64_t() const noexcept { return this->b; } // must be explicit or operator overloads will break because C++ can implicitly cast EITHER SIDE of an operator
      };
   #endif
}