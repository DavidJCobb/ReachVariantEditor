#pragma once
#include "./constants.h"
#include "./cs.h"

namespace cobb {
   template<
      cs Name,
      size_t Count,
      impl::reflex_enum::compile_time_value_type Value = impl::reflex_enum::undefined
   >
   struct reflex_enum_range_member {
      reflex_enum_range_member()  = delete;
      ~reflex_enum_range_member() = delete;

      static constexpr auto   name  = Name;
      static constexpr size_t count = Count;
      static constexpr auto   value = Value;
   };

   namespace impl::reflex_enum {
      template<typename T> concept is_range_member_type = requires(T) {
         { T::name };
         { T::name.capacity() } -> std::same_as<size_t>;
         { T::name.size() } -> std::same_as<size_t>;
         requires std::is_same_v<std::decay_t<decltype(T::name)>, cs<T::name.capacity()>>;
         //
         { T::count } -> std::same_as<const size_t&>;
         { T::value };
         requires std::is_same_v<std::decay_t<T>, reflex_enum_range_member<std::decay_t<decltype(T::name)>(T::name), (size_t)T::count, std::decay_t<decltype(T::value)>(T::value)>>;
      };
   }
}
