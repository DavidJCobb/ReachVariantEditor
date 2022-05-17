#pragma once
#include "helpers/bitnumber.h"
#include "halo/util/scalar_trait.h"
#include "./trait_information.h"

namespace halo {
   namespace impl::trait_bitnumber {
      template<bool B> using presence_bit_type = std::conditional_t<
         B,
         std::true_type,
         cobb::bitnumber_no_presence_bit
      >;

      template<typename Trait> concept is_scalar_trait = requires {
         typename Trait::underlying_type;
         { Trait::unchanged };
         requires std::is_base_of_v<
            util::scalar_trait<typename Trait::underlying_type, Trait::unchanged>,
            Trait
         >;
      };

      template<typename Trait> struct get_bitnumber_suitable_type {
         using type = Trait;
      };
      template<typename Trait> requires is_scalar_trait<Trait> struct get_bitnumber_suitable_type<Trait> {
         using type = Trait::underlying_type;
      };

      template<typename Trait> struct helper {
         static constexpr trait_information info = ::halo::trait_information_for<Trait>;

         static constexpr size_t bitcount = info.bitcount;
         using presence_bit_flag = std::conditional_t<info.uses_presence_bit, std::true_type, cobb::bitnumber_no_presence_bit>;

         static constexpr Trait default_value = ::halo::default_trait_value<Trait>;

         using base_bitnumber = cobb::bitnumber<
            bitcount,
            get_bitnumber_suitable_type<Trait>::type,
            false,
            presence_bit_flag,
            default_value
         >;
      };
   }
   
   //
   // Creates a cobb::bitnumber template instantiation given a trait, with the appropriate 
   // bitcount, underlying type, and so on.  This is implemented as a subclass in order to 
   // allow us to set the default value automatically, too.
   //
   template<typename Trait> requires trait_has_compile_time_information<Trait>
   class trait_bitnumber : public impl::trait_bitnumber::helper<Trait>::base_bitnumber {
      protected:
         using helper = impl::trait_bitnumber::helper<Trait>;
      public:
         trait_bitnumber() {
            this->value = helper::default_value;
         }
   };
}