#pragma once
#include "./constants.h"
#include "./cs.h"

namespace cobb {

   //
   // Member type for a reflex enum. If you do not specify a value, then the value is 
   // computed automatically (as would be done for a 'real" enum).
   //
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

   // Can be used to insert gaps into a reflex enum, i.e. the following are equivalent:
   //
   //    cobb::reflex_enum<
   //       cobb::reflex_enum_member<cobb::cs("A")>,
   //       cobb::reflex_enum_member<cobb::cs("B"), 2>, // explicit value
   //    >;
   // 
   // and
   //
   //    cobb::reflex_enum<
   //       cobb::reflex_enum_member<cobb::cs("A")>,
   //       cobb::reflex_enum_gap,
   //       cobb::reflex_enum_member<cobb::cs("B")>,
   //    >;
   // 
   // Both of these are the same as
   // 
   //    enum class my_enum {
   //       A,
   //       B = 2,
   //    };
   // 
   // Useful for gaps in very large enums.
   //
   struct reflex_enum_gap {};

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
