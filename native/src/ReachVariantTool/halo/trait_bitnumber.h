#pragma once
#include "halo/bitnumber.h"
#include "halo/util/scalar_trait.h"
#include "./bitnumber.h"
#include "./trait_information.h"

namespace halo {
   namespace impl::trait_bitnumber {
      template<typename Trait> struct helper {
         static_assert(impl::trait_has_default_value<Trait>, "The trait must have a default value set, or it must contain an 'unchanged' value.");

         static constexpr trait_information info = ::halo::trait_information_for<Trait>;

         using trait = Trait;
         static constexpr bitnumber_params<trait> params = ([]() {
            bitnumber_params<trait> v;
            v.initial = ::halo::default_trait_value<trait>;
            if (info.uses_presence_bit)
               v.presence = true;
            if (info.uses_sign_bit)
               v.has_sign_bit = true;
            return v;
         })();

         using type = typename halo::bitnumber<
            info.bitcount,
            trait,
            params
         >;
      };
   }
   
   //
   // Creates a cobb::bitnumber template instantiation given a trait, with the appropriate 
   // bitcount, underlying type, and so on.  This is implemented as a subclass in order to 
   // allow us to set the default value automatically, too.
   //
   template<typename Trait> requires trait_has_compile_time_information<Trait>
   using trait_bitnumber = impl::trait_bitnumber::helper<Trait>::type;
}