#pragma once
#include <type_traits>

namespace cobb {
   // Compile-time string suitable for use as a template parameter
   // Must be passed as some_template<cs("value")>
   // In some cases e.g. template functions' parameters, IntelliSense may get confused unless you do some_function<(cs("value"))>();
   template<size_t Size> class cs {
      public:
         static constexpr size_t length = Size;
         static constexpr size_t length_incl_null_terminator = Size + 1;

         char value[Size + 1] = { '\0' };

      public:
         constexpr cs() {}
         constexpr cs(const char (&lit)[Size + 1]) {
            for (size_t i = 0; i < length_incl_null_terminator; ++i)
               value[i] = lit[i];
            if (lit[Size] != '\0')
               throw;
         }

         constexpr size_t capacity() const { return length; }
         constexpr size_t size() const { return length; }
         constexpr const char* c_str() const { return (const char*)value; }

         constexpr const char& operator[](size_t i) const noexcept { return this->value[i]; }

         template<size_t OtherSize> constexpr bool operator==(const cs<OtherSize>& o) const {
            if constexpr (Size != OtherSize)
               return false;
            for (size_t i = 0; i < length_incl_null_terminator; ++i)
               if (value[i] != o.value[i])
                  return false;
            return true;
         }
         template<size_t OtherSize> constexpr bool operator!=(const cs<OtherSize>& o) const {
            if constexpr (Size != OtherSize)
               return true;
            for (size_t i = 0; i < length_incl_null_terminator; ++i)
               if (value[i] != o.value[i])
                  return true;
            return false;
         }

         template<size_t OtherSize> constexpr cs<Size + OtherSize> operator+(const cs<OtherSize>& o) const {
            cs<length_incl_null_terminator + OtherSize> out = {};
            for (size_t i = 0; i < length; ++i)
               out.value[i] = value[i];
            for (size_t i = 0; i < o.size(); ++i)
               out.value[i + length] = o.value[i];
            out.value[length + OtherSize] = '\0';
            return out;
         }
   };

   // Deduction guide now required since we don't just use Size verbatim in the constructor.
   template <size_t SizeInclNullTerminator> cs(const char(&)[SizeInclNullTerminator]) -> cs<SizeInclNullTerminator - 1>;

   template<typename T> concept is_cs = requires (T x){
      { T::length };
      requires std::is_same_v<std::decay_t<T>, cobb::cs<T::length>>;
   };
}