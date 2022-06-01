#include "any.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo::operands {
   namespace variables {
      void any::read(bitreader& stream) {
         stream.read(this->type);
         switch (this->type) {
            case variable_type::number:
               this->value = new number;
               break;
            case variable_type::object:
               this->value = new object;
               break;
            case variable_type::player:
               this->value = new player;
               break;
            case variable_type::team:
               this->value = new team;
               break;
            case variable_type::timer:
               this->value = new timer;
               break;
         }
         if (!this->value) {
            if constexpr (bitreader::has_load_process) {
               static_assert(false, "TODO: emit fatal error");
            }
            return;
         }
         this->value->read(stream);
      }
   }
}