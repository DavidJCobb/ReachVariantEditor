#pragma once
#include "helpers/cs.h"
#include "halo/reach/bitstreams.h"
#include "../../operand.h"
#include "../../operand_typeinfo.h"
#include "../../variable_scope.h"
#include "../../variable_type.h"

#include "./all_core_types.h"

namespace halo::reach::megalo::operands::variables {
   template<cobb::cs Name, variable_scope Scope, variable_type Member>
   class variable_and_member : public operand {
      public:
         static constexpr operand_typeinfo typeinfo = {
            .internal_name = Name.c_str(),
         };

      public:
         using base_type = variable_operand_type<variable_type_for_scope(Scope)>;

         using index_type = bitnumber<
            std::bit_width(
               variable_scope_metadata_from_enum(Scope).maximum_of_type(Member) - 1
            ),
            int16_t,
            bitnumber_params<int16_t>{
               .if_absent = -1,
               .initial   = -1,
               .presence  = false, // absence bit
            }
         >;

      public:
         base_type* owner = nullptr;
         index_type index;

         virtual void read(bitreader& stream) override {
            stream.read(owner);
            stream.read(index);
         }
   };

   using object_and_player_member = variable_and_member<
      cobb::cs("object.player"),
      variable_scope::object,
      variable_type::player
   >;
}