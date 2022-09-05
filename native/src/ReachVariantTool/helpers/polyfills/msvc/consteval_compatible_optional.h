#pragma once
#include <optional> // for std::bad_optional_access
#include <type_traits>

namespace cobb::polyfills::msvc {
   //
   // An MSVC bug prevents std::optional from being consteval-compatible even though it is 
   // constexpr-compatible:
   // 
   // https://developercommunity.visualstudio.com/t/using-stdoptional-in-constexpr-functions-and-actua/1474522
   // 
   // This bug was reported on July 14, 2021, and has remained a problem up to at least 
   // September 4, 2022.
   //
   template<typename T> class consteval_compatible_optional {
      private:
         struct dummy {};
      protected:
         union {
            dummy d = {};
            T     v;
         } un;
         bool exists = false;

      public:
         constexpr consteval_compatible_optional() {}
         constexpr consteval_compatible_optional(const T& x) : exists(true) { un.v = x; } // cannot do `constexpr classname(const T& x) : v(x), exists(true) {}` with unnamed union; causes internal compiler error
         constexpr ~consteval_compatible_optional() {
            reset();
         }

         constexpr void reset() {
            if (exists) {
               un.v.~T();
               exists = false;
            }
         }

         constexpr bool has_value() const { return exists; }
         constexpr operator bool() const { return has_value(); }

         constexpr T& value() {
            if (!has_value())
               throw std::bad_optional_access{};
            return un.v;
         }
         constexpr const T& value() const {
            if (!has_value())
               throw std::bad_optional_access{};
            return un.v;
         }

         template<std::copy_constructible U> requires std::is_convertible_v<U, T>
         constexpr T value_of(U&& default_value) const& {
            if (has_value())
               return value();
            return static_cast<T>(std::forward<U>(default_value));
         }
         template<std::move_constructible U> requires std::is_convertible_v<U, T>
         constexpr T value_of(U&& default_value) && {
            if (has_value())
               return value();
            return static_cast<T>(std::forward<U>(default_value));
         }

         constexpr void swap(consteval_compatible_optional& other)
            noexcept(std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_swappable_v<T>)
            requires std::is_move_constructible_v<T>
         {
            if (has_value() == other.has_value()) {
               if (has_value()) {
                  std::swap(value(), other.value());
               }
               return;
            }
            //
            // Only one has a value.
            //
            if (has_value()) {
               other.value() = std::move(value());
               other.exists = true;
               value().~T();
               exists = false;
            } else
               other.swap(*this);
         }
   };
}