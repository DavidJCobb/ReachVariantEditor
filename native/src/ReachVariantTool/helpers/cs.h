#pragma once
#include <array>
#include <type_traits>

namespace cobb {
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
         constexpr const char* c_str() const { return (const char*)value.data(); }

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

   template<typename T> concept is_cs = requires (T x){
      { T::length };
      requires std::is_same_v<std::decay_t<T>, cobb::cs<T::length>>;
   };
}