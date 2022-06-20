#pragma once
#include <memory>
#include "helpers/cs.h"
#include "helpers/owned_ptr.h"
#include "halo/reach/bitstreams.h"
#include "../../create_operand.h"
#include "../../operand.h"
#include "../../operand_typeinfo.h"
#include "../../variable_scope.h"
#include "../../variable_type.h"

#include "./all_core_types.h"

namespace halo::reach::megalo::operands::variables {
   template<cobb::cs Name, variable_scope Scope, variable_type Member>
   class variable_and_member : public operand {
      public:
         inline static constexpr operand_typeinfo typeinfo = {
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
         cobb::owned_ptr<base_type> owner = nullptr;
         index_type index;

         virtual void read(bitreader& stream) override {
            if (!owner)
               owner = new base_type;
            stream.read(*owner.get());
            stream.read(index);
         }
         virtual void write(bitwriter& stream) const override {
            assert(owner != nullptr);
            stream.write(*owner.get());
            stream.write(index);
         }

         variable_and_member& operator=(const variable_and_member& other) {
            this->owner = (base_type*)clone_operand(*other.owner.get());
            this->index = other.index;
            return *this;
         }
   };

   using object_and_player_member = variable_and_member<
      cobb::cs("object.player"),
      variable_scope::object,
      variable_type::player
   >;
}