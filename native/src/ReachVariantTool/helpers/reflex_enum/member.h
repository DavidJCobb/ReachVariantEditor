#pragma once
#include "./constants.h"
#include "./cs.h"

namespace cobb {
   template<
      cs Name,
      impl::reflex_enum::compile_time_value_type Value = impl::reflex_enum::undefined
   >
   struct reflex_enum_member {
      reflex_enum_member()  = delete;
      ~reflex_enum_member() = delete;

      static constexpr auto name  = Name;
      static constexpr auto value = Value;
   };

   namespace impl::reflex_enum {
      template<typename T> concept is_member_type = requires {
         { T::name };
         { T::name.capacity() } -> std::same_as<size_t>;
         { T::name.size() } -> std::same_as<size_t>;
         requires std::is_same_v<std::decay_t<decltype(T::name)>, cs<T::name.capacity()>>;
         //
         { T::value };
         requires std::is_same_v<std::decay_t<T>, reflex_enum_member<std::decay_t<decltype(T::name)>(T::name), std::decay_t<decltype(T::value)>(T::value)>>;
      };
   }
}
