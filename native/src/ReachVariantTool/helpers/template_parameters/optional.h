#pragma once
#include <optional> // for std::bad_optional_access
#include <type_traits>

namespace cobb::template_parameters {
   //
   // Instances of std::optional aren't usable as non-type template parameters, nor are any 
   // instances of any class that has an std::optional member. This is because std::optional
   // doesn't meet the requirements for a "structural class type" as laid out in the C++ 
   // standard.
   // 
   // A "structural type" is any of the following:
   // 
   //  - A scalar
   //  - An lvalue reference
   //  - A class whose base classes and non-static data members are public, non-mutable, and 
   //    of structural types themselves
   // 
   // The std::optional class does not meet these requirements. This alternative does, making 
   // instances of it suitable for use as a template parameter. For run-time usage, you should 
   // prefer std::optional.
   //
   template<typename T> class optional {
      public:
         using value_type = T;
      public:
         value_type _value = {};
         bool _defined = false;

         constexpr optional() {}
         constexpr optional(value_type v) : _value(v), _defined(true) {}

         inline constexpr bool has_value() const noexcept { return _defined; }
         inline constexpr explicit operator bool() const noexcept { return has_value(); }

         constexpr void reset() noexcept {
            if (has_value()) {
               _value.~value_type();
               _defined = false;
            }
         }

         inline constexpr value_type value() const {
            if (!has_value())
               throw std::bad_optional_access();
            return _value;
         }

         template<class U> requires std::convertible_to<U, value_type>
         constexpr value_type value_or(U&& default_value) const {
            if (has_value())
               return value();
            return (value_type)std::forward<U>(default_value);
         }
   };
}
