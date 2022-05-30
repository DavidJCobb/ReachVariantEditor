#pragma once
#include <limits>
#include <type_traits>

namespace cobb::reflex::impl::enumeration {
   template<typename T> requires (std::is_arithmetic_v<T> && std::is_integral_v<T>)
   struct no_overflow_int {
      T value = {};

      static constexpr T min = std::numeric_limits<T>::min();
      static constexpr T max = std::numeric_limits<T>::max();

      constexpr no_overflow_int() {}
      template<typename V> requires std::is_convertible_v<V, T> constexpr no_overflow_int(V v) : value(v) {}

      operator T const() {
         return value;
      }

      constexpr no_overflow_int& operator++() {
         if (value == max)
            throw;
         ++value;
         return *this;
      }
      constexpr no_overflow_int& operator--() {
         if (value == min)
            throw;
         --value;
         return *this;
      }

      template<typename V> requires std::is_convertible_v<V, T>
      constexpr no_overflow_int operator+=(V v) {
         if (max - v < value)
            throw;
         value += v;
         return *this;
      }
      template<typename V> requires std::is_convertible_v<V, T>
      constexpr no_overflow_int operator-=(V v) {
         if (min + v > value)
            throw;
         value -= v;
         return *this;
      }
      template<typename V> requires std::is_convertible_v<V, T>
      constexpr no_overflow_int operator*=(V v) {
         if (max / v > value) // overflow
            throw;
         if (min / v < value) // underflow
            throw;
         value *= v;
         return *this;
      }
      template<typename V> requires std::is_convertible_v<V, T>
      constexpr no_overflow_int operator*=(V v) {
         value /= v;
         return *this;
      }

      template<typename V> requires std::is_convertible_v<V, T>
      constexpr no_overflow_int operator+(V v) const {
         return (no_overflow_int(*this) += v);
      }

      template<typename V> requires std::is_convertible_v<V, T>
      constexpr no_overflow_int operator-(V v) const {
         return (no_overflow_int(*this) -= v);
      }

      template<typename V> requires std::is_convertible_v<V, T>
      constexpr no_overflow_int operator*(V v) const {
         return (no_overflow_int(*this) *= v);
      }

      template<typename V> requires std::is_convertible_v<V, T>
      constexpr no_overflow_int operator/(V v) const {
         return (no_overflow_int(*this) /= v);
      }
   };
}