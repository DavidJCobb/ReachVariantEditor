#pragma once
#include <bit>
#include <cstdint>
#include "halo/util/dummyable.h"
#include "halo/util/indexed.h"
#include "halo/util/refcount.h"
#include "halo/reach/bitstreams.fwd.h"
#include "./limits.h"
#include "./static_team.h"
#include "./strings.h"

namespace halo::reach::megalo {
   struct forge_label : public util::passively_refcounted, public util::dummyable, public util::indexed {
      struct requirement_flag {
         enum type {
            objects_of_type = 0x01,
            assigned_team   = 0x02,
            number          = 0x04,
         };
      };
      using object_type_index = bitnumber<
         std::bit_width(limits::object_types),
         int32_t,
         bitnumber_params<int32_t>{
            .has_sign_bit = true,
            .initial      = -1, // "none"
            .offset       =  1,
         }
      >;

      string_ref_optional name;
      struct {
         bitnumber<3, uint8_t> flags = 0;
         object_type_index object_type;
         static_team_index team = static_team::none;
         bytenumber<int16_t>   number = 0;
         bitnumber<7, uint8_t> map_must_have_at_least = 0;
      } requirements;

      void read(bitreader&);
   };
}