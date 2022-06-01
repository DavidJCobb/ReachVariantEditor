#pragma once
#include <bit>
#include "helpers/reflex/enumeration.h"
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

#include "./variables/number.h"

namespace halo::reach::megalo::operands {
   class waypoint_icon : public operand {
      public:
         static constexpr operand_typeinfo typeinfo = {
            .internal_name = "waypoint_icon",
         };

      public:
         using value_type = cobb::reflex::enumeration<
            cobb::reflex::member<cobb::cs("speaker")>,
            cobb::reflex::member<cobb::cs("dead_teammate_marker")>,
            cobb::reflex::member<cobb::cs("lightning_bolt")>,
            cobb::reflex::member<cobb::cs("bullseye")>,
            cobb::reflex::member<cobb::cs("diamond")>,
            cobb::reflex::member<cobb::cs("bomb")>,
            cobb::reflex::member<cobb::cs("flag")>,
            cobb::reflex::member<cobb::cs("skull")>,
            cobb::reflex::member<cobb::cs("crown")>, // KOTH
            cobb::reflex::member<cobb::cs("vip")>,
            cobb::reflex::member<cobb::cs("padlock")>,
            cobb::reflex::member<cobb::cs("territory_a")>,
            cobb::reflex::member<cobb::cs("territory_b")>,
            cobb::reflex::member<cobb::cs("territory_c")>,
            cobb::reflex::member<cobb::cs("territory_d")>,
            cobb::reflex::member<cobb::cs("territory_e")>,
            cobb::reflex::member<cobb::cs("territory_f")>,
            cobb::reflex::member<cobb::cs("territory_g")>,
            cobb::reflex::member<cobb::cs("territory_h")>,
            cobb::reflex::member<cobb::cs("territory_i")>,
            cobb::reflex::member<cobb::cs("supply")>,
            cobb::reflex::member<cobb::cs("supply_health")>,
            cobb::reflex::member<cobb::cs("supply_air_drop")>,
            cobb::reflex::member<cobb::cs("supply_ammo")>,
            cobb::reflex::member<cobb::cs("arrow")>,
            cobb::reflex::member<cobb::cs("defend")>,
            cobb::reflex::member<cobb::cs("ordnance")>,
            cobb::reflex::member<cobb::cs("inward")>//,
         >;

         using index_type = bitnumber<
            std::bit_width((unsigned int)value_type::underlying_value_range),
            value_type,
            bitnumber_params<value_type>{
               .initial = -1,
               .offset  = 1,
            }
         >;

      public:
         index_type icon;
         variables::number number;

         virtual void read(bitreader& stream) override;
   };
}

