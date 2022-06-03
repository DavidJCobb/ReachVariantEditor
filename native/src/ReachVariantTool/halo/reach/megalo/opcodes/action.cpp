#include "action.h"
#include "all_actions.h"
#include "halo/reach/bitstreams.h"
#include "halo/reach/megalo/create_operand.h"

namespace halo::reach::megalo {
   void action::read(bitreader& stream) {
      constexpr auto& list = all_actions;

      size_t fi = stream.read_bits(std::bit_width(list.size() - 1));
      if (fi > list.size()) {
         if constexpr (bitreader::has_load_process) {
            static_assert(false, "TODO: emit fatal error");
         }
         return;
      }
      this->function = &list[fi];
      if (fi == 0) { // "none" opcode
         return;
      }
      //
      // Load args:
      // 
      size_t count = this->function->operands.size();
      this->operands.resize(count);
      for (size_t i = 0; i < count; ++i) {
         auto&  definition = this->function->operands[i];
         auto*& entry      = this->operands[i];
         entry = create_operand(definition.typeinfo);
         assert(entry);
         stream.read(*entry);
      }
   }
}