#pragma once
#include "halo/reach/bitstreams.fwd.h"
#include "../../operand.h"
#include "../../operand_typeinfo.h"
#include "../../variable_scope.h"

namespace halo::reach::megalo::operands::variables {
   namespace impl {
      class base : public operand {
         protected:
            size_t  target_id = 0;
         public:
            uint8_t which = 0;
            int16_t index = 0;
            
         protected:
            void read_target_id(bitreader&, size_t target_count);
            void read_which(bitreader&, variable_scope which);
            void read_index(bitreader&, size_t bitcount); // forces unsigned
            void read_immediate(bitreader&, size_t bitcount); // forces signed

            virtual variable_type get_type() const = 0;
      };
   }

   // for things that don't care what they have a pointer to:
   using unknown_type = impl::base;
}