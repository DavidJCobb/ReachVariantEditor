#pragma once
#include <bit>
#include "helpers/reflex/enumeration.h"
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

namespace halo::reach::megalo::operands {
   class variant_string : public operand {
      public:
         static constexpr operand_typeinfo typeinfo = {
            .internal_name = "variant_string",
         };

      public:
         using value_type = cobb::reflex::enumeration<
            cobb::reflex::member<cobb::cs("mp_boneyard_a_idle_start")>,
            cobb::reflex::member<cobb::cs("mp_boneyard_a_fly_in")>,
            cobb::reflex::member<cobb::cs("mp_boneyard_a_idle_mid")>,
            cobb::reflex::member<cobb::cs("mp_boneyard_a_fly_out")>,
            cobb::reflex::member<cobb::cs("mp_boneyard_b_fly_in")>,
            cobb::reflex::member<cobb::cs("mp_boneyard_b_idle_mid")>,
            cobb::reflex::member<cobb::cs("mp_boneyard_b_fly_out")>,
            cobb::reflex::member<cobb::cs("mp_boneyard_b_idle_start")>,
            cobb::reflex::member<cobb::cs("mp_boneyard_a_leave1")>,
            cobb::reflex::member<cobb::cs("mp_boneyard_b_leave1")>,
            cobb::reflex::member<cobb::cs("mp_boneyard_b_pickup")>,
            cobb::reflex::member<cobb::cs("mp_boneyard_b_idle_pickup")>,
            cobb::reflex::member<cobb::cs("mp_boneyard_a")>,
            cobb::reflex::member<cobb::cs("mp_boneyard_b")>,
            cobb::reflex::member<cobb::cs("default")>,
            cobb::reflex::member<cobb::cs("carter")>,
            cobb::reflex::member<cobb::cs("jun")>,
            cobb::reflex::member<cobb::cs("female")>,
            cobb::reflex::member<cobb::cs("male")>,
            cobb::reflex::member<cobb::cs("emile")>,
            cobb::reflex::member<cobb::cs("player_skull")>,
            cobb::reflex::member<cobb::cs("kat")>,
            cobb::reflex::member<cobb::cs("minor")>,
            cobb::reflex::member<cobb::cs("officer")>,
            cobb::reflex::member<cobb::cs("ultra")>,
            cobb::reflex::member<cobb::cs("space")>,
            cobb::reflex::member<cobb::cs("spec_ops")>,
            cobb::reflex::member<cobb::cs("general")>,
            cobb::reflex::member<cobb::cs("zealot")>,
            cobb::reflex::member<cobb::cs("mp")>,
            cobb::reflex::member<cobb::cs("jetpack")>,
            cobb::reflex::member<cobb::cs("gauss")>,
            cobb::reflex::member<cobb::cs("troop")>,
            cobb::reflex::member<cobb::cs("rocket")>,
            cobb::reflex::member<cobb::cs("fr")>, // frag?
            cobb::reflex::member<cobb::cs("pl")>, // plasma?
            cobb::reflex::member<cobb::cs("35_spire_fp")>,
            cobb::reflex::member<cobb::cs("mp_spire_fp")>//,
         >;

         using index_type = bitnumber<
            std::bit_width((unsigned int)value_type::underlying_value_range),
            value_type,
            bitnumber_params<value_type>{
               .initial  = -1,
               .offset   = 1,
               .presence = false, // absence bit
            }
         >;

      public:
         index_type value;

         virtual void read(bitreader& stream) override;
   };
}

