#include "./variable_declarations.h"
#include "halo/reach/bitstreams.h"

#include "halo/reach/megalo/operands/variables/number.h"

namespace halo::reach::megalo {
   namespace impl {
      void variable_declaration::read(variable_type t, bitreader& stream) {
         switch (t) {
            case variable_type::team:
               stream.read(this->initial.team);
               break;
            case variable_type::number:
               [[fallthrough]];
            case variable_type::timer:
               if (!this->initial.value)
                  this->initial.value = new operands::variables::number;
               stream.read(*this->initial.value);
               break;
         }
         switch (t) {
            case variable_type::number:
            case variable_type::player:
            case variable_type::object:
            case variable_type::team:
               stream.read(this->network_priority);
               break;
         }
      }
      void variable_declaration::write(variable_type t, bitwriter& stream) const {
         switch (t) {
            case variable_type::team:
               stream.write(this->initial.team);
               break;
            case variable_type::number:
               [[fallthrough]];
            case variable_type::timer:
               if (!this->initial.value) {
                  auto dummy = operands::variables::number();
                  dummy.set_to_immediate(0);
                  stream.write(dummy);
               }
               stream.write(*this->initial.value);
               break;
         }
         switch (t) {
            case variable_type::number:
            case variable_type::player:
            case variable_type::object:
            case variable_type::team:
               stream.write(this->network_priority);
               break;
         }
      }
   }
}