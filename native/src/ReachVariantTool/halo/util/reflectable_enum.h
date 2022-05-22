#pragma once
#include <array>
#include <concepts>
#include <limits>
#include <string>
#include <tuple>
#include <type_traits>

//
// EXPERIMENTAL; not sure if i'm gonna use this yet
//

namespace halo::util {
   namespace impl::reflex_enum {
      using compile_time_value_type = int64_t;
      constexpr compile_time_value_type undefined = std::numeric_limits<int64_t>::lowest();

      template<typename First, typename... Values> struct extract_typeinfo {
         static constexpr bool explicit_underlying_type = std::is_integral_v<First> && std::is_arithmetic_v<First>;

         static constexpr size_t value_count = sizeof...(Values) + (explicit_underlying_type ? 0 : 1);
         using values = std::conditional_t<
            explicit_underlying_type,
            std::tuple<Values...>,
            std::tuple<First, Values...>
         >;

         using underlying_type = std::conditional_t<
            explicit_underlying_type,
            First,
            size_t
         >;
      };

      template<typename... Values> class all_names_unique {
         private:
            template<typename A, typename... B> struct worker {
               static constexpr bool value = ([]() {
                  bool a_overlaps = ((A::name == B::name) || ...);
                  if (a_overlaps)
                     return false;
                  if constexpr (sizeof...(B) > 1) {
                     return worker<B...>::value;
                  }
                  return true;
               })();
            };
         public:
            static constexpr bool value = ([]() {
               if constexpr (sizeof...(Values) > 1) {
                  return worker<Values...>::value;
               }
               return true;
            })();
      };
   }

   // Compile-time string suitable for use as a template parameter
   // Must be passed as some_template<cs("value")>
   // In some cases e.g. template functions' parameters, IntelliSense may get confused unless you do some_function<(cs("value"))>();
   template<size_t Size> requires (Size >= 1) class cs {
      public:
         static constexpr size_t length = Size;

         std::array<char, length> value = {};

      public:
         constexpr cs(const char (&lit)[Size]) {
            for (size_t i = 0; i < Size; ++i)
               value[i] = lit[i];
            if (lit[Size - 1] != '\0')
               throw;
         }

         constexpr size_t capacity() const { return length; }
         constexpr size_t size() const { return length - 1; }
         constexpr const char* c_str() const { return value.data(); }

         template<size_t OtherSize> constexpr bool operator==(const cs<OtherSize>& o) const {
            if constexpr (Size != OtherSize)
               return false;
            for (size_t i = 0; i < Size; ++i)
               if (value[i] != o.value[i])
                  return false;
            return true;
         }
         template<size_t OtherSize> constexpr bool operator!=(const cs<OtherSize>& o) const {
            if constexpr (Size != OtherSize)
               return true;
            for (size_t i = 0; i < Size; ++i)
               if (value[i] != o.value[i])
                  return true;
            return false;
         }

         template<size_t OtherSize> constexpr cs<Size + OtherSize> operator+(const cs<OtherSize>& o) const {
            cs<Size + OtherSize> out = {};
            for (size_t i = 0; i < size(); ++i)
               out.value[i] = value[i];
            for (size_t i = 0; i < o.size(); ++i)
               out.value[i + Size] = o.value[i];
            return out;
         }
   };

   template<cs Name, impl::reflex_enum::compile_time_value_type Value = impl::reflex_enum::undefined> struct reflex_enum_value {
      static constexpr auto value = Value;
      static constexpr auto name = Name;
   };

   namespace impl::reflex_enum {
      template<typename T> concept is_value_type = requires(T) {
         { T::name };
         { T::name.capacity() } -> std::same_as<size_t>;
         { T::name.size() } -> std::same_as<size_t>;
         requires std::is_same_v<std::decay_t<decltype(T::name)>, cs<T::name.capacity()>>;
         { T::value };
         requires std::is_same_v<std::decay_t<T>, reflex_enum_value<cs<T::name.capacity()>(T::name), T::value>>;
      };
   }

   template<typename... Values> requires ((impl::reflex_enum::is_value_type<Values> && ...) && impl::reflex_enum::all_names_unique<Values...>::value)
   struct reflex_enum {
      protected:
         using _extractor = impl::reflex_enum::extract_typeinfo<Values...>;

      public:
         using underlying_type = _extractor::underlying_type;
         using values = _extractor::values;
         static constexpr size_t value_count = _extractor::value_count;
         static constexpr size_t missing_type = std::numeric_limits<size_t>::max();

         template<cs Name> static constexpr size_t index_of = ([]() {
            size_t index = 0;
            (
               (Values::name == Name ?
                  false             // if match: set result to current index; use false to short-circuit the "and" operator and stop iteration
                : ((++index), true) // no match: increment current index; use true to avoid short-circuiting, and continue iteration
               ) && ...
            );
            return index < value_count ? index : missing_type;
         })();
         template<cs Name> static constexpr bool has = (index_of<Name> != missing_type);

      protected:
         underlying_type _value = {};

         template<size_t i> using type_by_index = std::tuple_element_t<i, values>;
         template<cs Name> requires has<Name> using name_to_type = type_by_index<index_of<Name>>;

         template<typename Value> static constexpr underlying_type value_of_type = ([]() { // broken
            if constexpr (Value::value != impl::reflex_enum::undefined)
               return (underlying_type)Value::value;
            //
            underlying_type v = -1;
            //
            constexpr size_t index = index_of<Value::name>;
            size_t i = 0;
            (
               ([&v, &i]<typename Current>(Current x) {
                  if (i++ > index_of<Value::name>)
                     return;
                  if (Current::value == impl::reflex_enum::undefined)
                     ++v;
                  else
                     v = Current::value;
               })(Values{}), ...
            );
            return v;
         })();

      public:
         constexpr reflex_enum() {}
         template<cs Name> requires has<Name> static constexpr reflex_enum make() {
            reflex_enum out;
            out._value = value_of<Name>;
            return out;
         }

         template<cs Name> constexpr bool is() const {
            if constexpr (has<Name>)
               return this->_value == value_of<Name>;
            else
               return false;
         }

         template<cs Name> static constexpr underlying_type value_of = value_of_type<name_to_type<Name>>;

         template<cs Name> requires has<Name> reflex_enum& assign() {
            this->_value = value_of<Name>;
         }

         constexpr underlying_type to_int() const {
            return _value;
         }
         constexpr const char* to_string() const {
            const char* out = nullptr;
            (
               ([this, &out]<typename Current>(Current) {
                  if (this->_value == value_of<Current::name>) {
                     out = Current::name.c_str();
                  }
               })(Values{}), ...
            );
            return out;
         }
   };
}