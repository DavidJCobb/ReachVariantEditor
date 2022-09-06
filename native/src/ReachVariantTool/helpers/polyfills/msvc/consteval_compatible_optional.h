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
   // This "optional" variant unfortunately isn't capable of never constructing a wrapped 
   // value, so it's only really useful as a quick convenience for defining things like 
   // optional ints and simple structs. Anything with side effects will be a bad idea.
   //
   template<typename T> class consteval_compatible_optional {
      public:
         using value_type = std::decay_t<T>;
      private:
         struct dummy {};
         static constexpr bool _can_use_union     = false;
         static constexpr bool _can_destroy_value = _can_use_union || false; // IntelliSense whines if we do this...
      protected:
         /*// far's I can tell, the use of a union (to allow the value to never be constructed if the containing optional is created empty) is what breaks consteval.
         union {
            //dummy      d = {};
            value_type v = {};
         } un;
         //*/
         value_type v = {};
         bool exists = false;

         // _underlying: access either the value, or where it would be if it existed
         constexpr value_type& _underlying() {
            // return un.v;
            return v;
         }
         constexpr const value_type& _underlying() const {
            // return un.v;
            return v;
         }

         constexpr void _create_if_empty(const T& v = {}) {
            if (exists)
               return;
            exists = true;
            if constexpr (_can_use_union)
               std::construct_at<T>((T*)&_underlying());
         }

      public:
         constexpr consteval_compatible_optional() {
            if constexpr (!_can_use_union && _can_destroy_value)
               _underlying().~value_type(); // IntelliSense whines about this
         }
         constexpr consteval_compatible_optional(const value_type& x) {
            _underlying() = x;
            exists = true;
         }
         constexpr ~consteval_compatible_optional() {
            reset();
         }

         constexpr consteval_compatible_optional(const consteval_compatible_optional& other) {
            *this = other;
         }
         constexpr consteval_compatible_optional(consteval_compatible_optional&& other) noexcept {
            *this = std::move(other);
         }

         #pragma region operator=
         constexpr consteval_compatible_optional& operator=(std::nullopt_t) noexcept { reset(); return *this; }

         constexpr consteval_compatible_optional& operator=(const consteval_compatible_optional& other) {
            if (has_value() == other.has_value()) {
               if (has_value())
                  this->value() = other.value();
               return *this;
            }
            if (has_value()) {
               reset();
               return *this;
            }
            _create_if_empty();
            value() = other.value();
            return *this;
         }
         constexpr consteval_compatible_optional& operator=(consteval_compatible_optional&& other) noexcept {
            if (has_value() == other.has_value()) {
               if (has_value())
                  this->value() = std::move(other.value());
               return *this;
            }
            if (has_value()) {
               reset();
               return *this;
            }
            _create_if_empty();
            value() = std::move(other.value());
            return *this;
         }

         template<typename U = T> requires (!std::is_same_v<U, consteval_compatible_optional<T>>)
         constexpr consteval_compatible_optional& operator=(U&& value) {
            _create_if_empty();
            this->value() = static_cast<value_type>(std::forward<U>(value));
            return *this;
         }

         template<typename U> requires (!std::is_same_v<U, T>)
         constexpr consteval_compatible_optional& operator=(consteval_compatible_optional<U>&& other) {
            if (other.has_value()) {
               _create_if_empty();
               this->value() = static_cast<value_type>(other.value());
            } else {
               reset();
            }
            return *this;
         }
         #pragma endregion

         constexpr void reset() {
            if (has_value()) {
               if constexpr (_can_use_union || _can_destroy_value)
                  value().~T();
               exists = false;
            }
         }

         constexpr bool has_value() const { return exists; }
         constexpr operator bool() const { return has_value(); }

         constexpr value_type& value() {
            if (!has_value())
               throw std::bad_optional_access{};
            return _underlying();
         }
         constexpr const value_type& value() const {
            if (!has_value())
               throw std::bad_optional_access{};
            return _underlying();
         }

         template<std::copy_constructible U> requires std::is_convertible_v<U, value_type>
         constexpr value_type value_of(U&& default_value) const& {
            if (has_value())
               return value();
            return static_cast<value_type>(std::forward<U>(default_value));
         }
         template<std::move_constructible U> requires std::is_convertible_v<U, value_type>
         constexpr value_type value_of(U&& default_value) && {
            if (has_value())
               return value();
            return static_cast<value_type>(std::forward<U>(default_value));
         }

         template<std::copy_constructible U> requires std::is_convertible_v<U, value_type>
         constexpr value_type value_or(U&& default_value) const& {
            if (has_value())
               return value();
            return static_cast<value_type>(default_value);
         }
         template<std::move_constructible U> requires std::is_convertible_v<U, value_type>
         constexpr value_type value_or(U&& default_value) && {
            if (has_value())
               return value();
            return static_cast<value_type>(default_value);
         }

         constexpr void swap(consteval_compatible_optional& other)
            noexcept(std::is_nothrow_move_constructible_v<value_type>&& std::is_nothrow_swappable_v<value_type>)
            requires std::is_move_constructible_v<value_type>
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