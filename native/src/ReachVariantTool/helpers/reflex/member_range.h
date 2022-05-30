#pragma once
#include <type_traits>
#include "../cs.h"
#include "./constants.h"
#include "./member.h"

namespace cobb::reflex {
   template<cobb::cs Name, size_t Count, auto BaseValue = undefined, auto Metadata = no_member_metadata{}>
   struct member_range {
      member_range() = delete;
      ~member_range() = delete;

      using value_type    = decltype(BaseValue);
      using metadata_type = decltype(Metadata);

      static constexpr cobb::cs      name     = Name;
      static constexpr size_t        count    = Count;
      static constexpr value_type    value    = BaseValue;
      static constexpr metadata_type metadata = Metadata;
   };
   
   template<typename T> concept is_member_range = requires {
      { T::name };
      { T::count };
      { T::value };
      { T::metadata };
      requires is_cs<std::decay_t<decltype(T::name)>>;
      requires std::is_same_v<
         std::decay_t<T>,
         member_range<
            std::decay_t<decltype(T::name)>(T::name),
            std::decay_t<decltype(T::count)>(T::count),
            std::decay_t<decltype(T::value)>(T::value),
            std::decay_t<decltype(T::metadata)>(T::metadata)
         >
      >;
   };
}
