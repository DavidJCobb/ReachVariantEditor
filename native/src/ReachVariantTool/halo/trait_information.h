#pragma once
#include <array>
#include <concepts>
#include <cstdint>
#include "helpers/unreachable.h"

namespace halo {
   template<typename T> concept trait_can_be_unchanged = requires {
      { T::unchanged } -> std::same_as<T>;
   } || requires {
      typename T::underlying_type;
      { T::unchanged } -> std::same_as<const typename T::underlying_type&>;
   };

   namespace impl {
      //
      // This metadata should be made available for each trait. See impl::trait_information_for below.
      //
      struct bare_trait_information {
         using type_erased_enum = int32_t;

         using to_integer_cast_t   = int(type_erased_enum _type_erased_enum_value);
         using from_integer_cast_t = type_erased_enum(int);

         uint32_t bitcount = 0; // Number of bits used to encode the value. If not set, it will be computed automatically assuming a tight enum (no gaps; no padding).
         //
         to_integer_cast_t*   integer_cast_to   = nullptr; // Do not call directly; use trait_value_to_integer.
         from_integer_cast_t* integer_cast_from = nullptr; // Do not call directly; use trait_value_from_integer.
         //
         bool is_integer = false;
         //
         type_erased_enum min = 0; // Lowest  enum value that the game considers valid.
         type_erased_enum max = 0; // Highest enum value that the game considers valid.
         //
         bool uses_presence_bit = false;
         bool uses_sign_bit     = false;
      };
   }
   struct trait_information : public impl::bare_trait_information {
      constexpr trait_information(impl::bare_trait_information d);
   };

   namespace impl {

      // Specialize this struct for each trait, and define a static constexpr trait_information member 
      // named "data."
      template<typename T> struct trait_information_for;

      // Specialize this struct for each trait, and define a static constexpr T member named "value". 
      // You can skip specializing this for any trait that has an "unchanged" value.
      template<typename T> struct default_trait_value;

      //
      // EXAMPLE OF A SIMPLE TRAIT:
      // 
      // This example also shows how to define a default value; however, for the specific trait shown 
      // here, that isn't necessary; it has an "unchanged" value, so our templates automatically pull 
      // that.
      // 
      //    namespace halo::impl {
      //       template<> struct default_trait_value<halo::reach::traits::ability_usage> {
      //          static constexpr auto value = halo::reach::traits::ability_usage::unchanged;
      //       };
      //       
      //       template<> struct trait_information_for<halo::reach::traits::ability_usage> {
      //          using trait = halo::reach::traits::ability_usage;
      //          static constexpr auto data = halo::trait_information({
      //             .min = (int)trait::unchanged,
      //             .max = (int)trait::enabled,
      //         });
      //       };
      //    }
      // 
      // EXAMPLE OF AN INTEGER TRAIT:
      // 
      // This uses some helpers near the bottom of this file.
      // 
      // If a trait has an "unchanged" value, that's supposed to be the default for (below_range) and 
      // (above_range) in the integer-cast info. However, MSVC 2022 is broken, and if you don't specify 
      // those fields by hand, you get an internal compiler error.
      // 
      //    namespace halo::impl {
      //       template<> struct default_trait_value<halo::reach::traits::damage_resistance> {
      //          static constexpr auto value = halo::reach::traits::damage_resistance::unchanged;
      //       };
      //       
      //       template<> struct trait_information_for<halo::reach::traits::damage_resistance> {
      //          using trait = halo::reach::traits::damage_resistance;
      //    
      //          using integer_cast_helpers = trait_integer_cast_helpers<
      //             trait,
      //             trait_integer_cast_info<trait>{
      //                .below_range   = trait::unchanged,
      //                .first_integer = trait::value_0010,
      //                .range         = { 10, 50, 90, 100, 110, 150, 200, 300, 500, 1000, 2000 },
      //                .above_range   = trait::invulnerable,
      //             }
      //          >;
      //    
      //          static constexpr auto data = trait_information({
      //             .integer_cast_to   = &integer_cast_helpers::to_integer,
      //             .integer_cast_from = &integer_cast_helpers::from_integer,
      //             .min = (int)trait::unchanged,
      //             .max = (int)trait::invulnerable,
      //          });
      //       };
      //    }
      //
   }

   template<typename Trait> constexpr const trait_information trait_information_for = impl::trait_information_for<Trait>::data;
   template<typename Trait> concept trait_has_compile_time_information = requires {
      { trait_information_for<Trait> };
      { trait_information_for<Trait> } -> std::same_as<const trait_information&>;
   };

   template<typename Trait> constexpr Trait default_trait_value = impl::default_trait_value<Trait>::value;

   template<typename Trait> requires trait_has_compile_time_information<Trait> constexpr int trait_value_to_integer(Trait v) {
      return (trait_information_for<Trait>.integer_cast_to)((impl::bare_trait_information::type_erased_enum)v);
   }
   template<typename Trait> requires trait_has_compile_time_information<Trait> constexpr Trait trait_value_from_integer(int v) {
      return (Trait)(trait_information_for<Trait>.integer_cast_from)(v);
   }

   //
   // Helpers for defining integer traits:
   //

   namespace impl {
      struct type_erased_trait_integer_cast_info {
         using type_erased_enum = bare_trait_information::type_erased_enum;

         type_erased_enum below_range = 0;
         type_erased_enum first_integer = 0;
         std::array<int, 99> range = {};
         type_erased_enum above_range = 0;

         constexpr size_t end_of_range() const;
         constexpr int maximum() const;
         constexpr int minimum() const;
      };

      template<typename T> struct trait_integer_cast_info {
         protected:
            static constexpr auto _unchanged_or_unset = ([]() { if constexpr (trait_can_be_unchanged<T>) { return T::unchanged; } else { return T{}; } })();
         public:
            T below_range = _unchanged_or_unset;
            T first_integer;
            std::array<int, 99> range = {};
            T above_range = _unchanged_or_unset;

            constexpr size_t end_of_range() const;
            constexpr int maximum() const;
            constexpr int minimum() const;
      };

      template<typename T, trait_integer_cast_info<T> Info> struct trait_integer_cast_helpers {
         protected:
            static constexpr auto info = Info;

            static constexpr int _to_integer(T v);
            static constexpr T _from_integer(int v);

         public:
            static constexpr int to_integer(bare_trait_information::type_erased_enum v) {
               return _to_integer((T)v);
            }
            static constexpr bare_trait_information::type_erased_enum from_integer(int v) {
               return (bare_trait_information::type_erased_enum)_from_integer(v);
            }
      };
   }

   //
   // Internal stuff:
   //

   // Auto-specialize default_trait_value for all traits with an "unchanged" value:
   namespace impl {
      template<typename T> requires trait_can_be_unchanged<T> struct default_trait_value<T> {
         static constexpr T value = T::unchanged;
      };
   }
}

#include "trait_information.inl"