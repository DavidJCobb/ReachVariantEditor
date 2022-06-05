#pragma once
#include "helpers/reflex/enumeration.h"
#include "./base_enum_operand.h"

#include "helpers/tuple_unpack.h"

namespace halo::reach::megalo::operands {
   using timer_rate = base_enum_operand<
      cobb::cs("timer_rate"),
      cobb::reflex::enumeration<
         cobb::reflex::member<
            cobb::cs("0"),
            cobb::reflex::undefined,
            0.0F
         >,
         cobb::reflex::member<
            cobb::cs("-10"),
            cobb::reflex::undefined,
            -0.10F
         >,
         cobb::reflex::member<
            cobb::cs("-25"),
            cobb::reflex::undefined,
            -0.25F
         >,
         cobb::reflex::member<
            cobb::cs("-50"),
            cobb::reflex::undefined,
            -0.50F
         >,
         cobb::reflex::member<
            cobb::cs("-75"),
            cobb::reflex::undefined,
            -0.75F
         >,
         cobb::reflex::member<
            cobb::cs("-100"),
            cobb::reflex::undefined,
            -1.00F
         >,
         cobb::reflex::member<
            cobb::cs("-125"),
            cobb::reflex::undefined,
            -1.25F
         >,
         cobb::reflex::member<
            cobb::cs("-150"),
            cobb::reflex::undefined,
            -1.50F
         >,
         cobb::reflex::member<
            cobb::cs("-175"),
            cobb::reflex::undefined,
            -1.75F
         >,
         cobb::reflex::member<
            cobb::cs("-200"),
            cobb::reflex::undefined,
            -2.00F
         >,
         cobb::reflex::member<
            cobb::cs("-300"),
            cobb::reflex::undefined,
            -3.00F
         >,
         cobb::reflex::member<
            cobb::cs("-400"),
            cobb::reflex::undefined,
            -4.00F
         >,
         cobb::reflex::member<
            cobb::cs("-500"),
            cobb::reflex::undefined,
            -5.00F
         >,
         cobb::reflex::member<
            cobb::cs("-1000"),
            cobb::reflex::undefined,
            -10.00F
         >,
         // positive:
         cobb::reflex::member<
            cobb::cs("10"),
            cobb::reflex::undefined,
            0.10F
         >,
         cobb::reflex::member<
            cobb::cs("25"),
            cobb::reflex::undefined,
            0.25F
         >,
         cobb::reflex::member<
            cobb::cs("50"),
            cobb::reflex::undefined,
            0.50F
         >,
         cobb::reflex::member<
            cobb::cs("75"),
            cobb::reflex::undefined,
            0.75F
         >,
         cobb::reflex::member<
            cobb::cs("100"),
            cobb::reflex::undefined,
            1.00F
         >,
         cobb::reflex::member<
            cobb::cs("125"),
            cobb::reflex::undefined,
            1.25F
         >,
         cobb::reflex::member<
            cobb::cs("150"),
            cobb::reflex::undefined,
            1.50F
         >,
         cobb::reflex::member<
            cobb::cs("175"),
            cobb::reflex::undefined,
            1.75F
         >,
         cobb::reflex::member<
            cobb::cs("200"),
            cobb::reflex::undefined,
            2.00F
         >,
         cobb::reflex::member<
            cobb::cs("300"),
            cobb::reflex::undefined,
            3.00F
         >,
         cobb::reflex::member<
            cobb::cs("400"),
            cobb::reflex::undefined,
            4.00F
         >,
         cobb::reflex::member<
            cobb::cs("500"),
            cobb::reflex::undefined,
            5.00F
         >,
         cobb::reflex::member<
            cobb::cs("1000"),
            cobb::reflex::undefined,
            10.00F
         >//,
      >
   >;
}