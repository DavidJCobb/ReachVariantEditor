#pragma once
#include <bit>
#include "halo/util/dummyable.h"
#include "halo/util/indexed.h"
#include "halo/util/refcount.h"
#include "halo/reach/bitstreams.fwd.h"
#include "./limits.h"
#include "./strings.h"

namespace halo::reach::megalo {
   class game_option : public util::dummyable, public util::indexed, public util::passively_refcounted {
      public:
         using value_type  = bitnumber<10, int16_t>;
         using value_index = bitnumber<std::bit_width(limits::script_option_values - 1), uint8_t>;

         struct enum_value {
            string_ref name;
            string_ref desc;
            value_type value;

            void read(bitreader&);
         };

      public:
         string_ref name;
         string_ref desc;
         bitbool is_range;
         struct {
            util::indexed_list<enum_value, limits::script_option_values> values;
            enum_value* current = nullptr;
            enum_value* initial = nullptr;
         } enumeration;
         struct {
            value_type initial;
            value_type min;
            value_type max;
            value_type current;
         } range;

         void read(bitreader&);
   };
}