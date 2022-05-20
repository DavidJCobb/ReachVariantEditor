#pragma once

namespace halo::util {
   template<typename Underlying, Underlying Unchanged = Underlying(-1)> class scalar_trait {
      public:
         using underlying_type = Underlying;
         static const underlying_type unchanged = Unchanged;
      public:
         underlying_type value = unchanged; // must be public to meet "structural class type" requirements for use [in anything being used] as a non-type template parameter

      public:
         constexpr scalar_trait() {}
         constexpr scalar_trait(Underlying v) : value(v) {}

         constexpr operator Underlying() const { return value; }

         scalar_trait& operator=(const Underlying v) { value = v; return *this; }
         scalar_trait& operator+=(const Underlying v) { value += v; return *this; }
         scalar_trait& operator-=(const Underlying v) { value -= v; return *this; }
         scalar_trait& operator*=(const Underlying v) { value *= v; return *this; }
         scalar_trait& operator/=(const Underlying v) { value /= v; return *this; }
         scalar_trait& operator%=(const Underlying v) { value %= v; return *this; }
         scalar_trait& operator^=(const Underlying v) { value ^= v; return *this; }
   };
}