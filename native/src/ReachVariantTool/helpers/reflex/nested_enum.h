#pragma once
#include "../cs.h"

namespace cobb::reflex {
   template<cobb::cs Name, typename Enum> struct nested_enum {
      nested_enum() = delete;
      ~nested_enum() = delete;

      static constexpr cobb::cs name = Name;

      using enumeration = Enum;
   };
   
   template<typename T> concept is_nested_enum = requires {
      { T::name };
      typename T::enumeration;
      //
      requires is_cs<decltype(T::name)>;
      requires std::is_same_v<
         std::decay_t<T>,
         nested_enum<
            std::decay_t<decltype(T::name)>(T::name),
            typename T::enumeration
         >
      >;
   };
}