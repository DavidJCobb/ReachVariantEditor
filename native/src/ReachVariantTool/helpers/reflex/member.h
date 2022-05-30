#pragma once
#include <type_traits>
#include "../cs.h"
#include "./constants.h"

namespace cobb::reflex {
   enum class no_member_metadata {};

   template<cobb::cs Name, auto Value = undefined, auto Metadata = no_member_metadata{}>
   struct member {
      member() = delete;
      ~member() = delete;

      using value_type    = decltype(Value);
      using metadata_type = decltype(Metadata);

      static constexpr cobb::cs      name     = Name;
      static constexpr value_type    value    = Value;
      static constexpr metadata_type metadata = Metadata;
   };

   struct member_gap {};

   template<typename T> concept is_member = requires {
      { T::name };
      { T::value };
      { T::metadata };
      requires is_cs<std::decay_t<decltype(T::name)>>;
      requires std::is_same_v<
         std::decay_t<T>,
         member<
            std::decay_t<decltype(T::name)>(T::name),
            std::decay_t<decltype(T::value)>(T::value),
            std::decay_t<decltype(T::metadata)>(T::metadata)
         >
      >;
   };
}