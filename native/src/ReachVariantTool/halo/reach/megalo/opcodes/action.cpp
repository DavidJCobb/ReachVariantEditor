#include "action.h"
#include "all_actions.h"
#include "halo/reach/bitstreams.h"
#include "halo/reach/megalo/create_operand.h"

#include "halo/reach/megalo/load_process_messages/opcode/bad_function_index.h"

namespace halo::reach::megalo {
   void action::read(bitreader& stream) {
      constexpr auto& list = all_actions;

      size_t fi = stream.read_bits(std::bit_width(list.size() - 1));
      if (fi >= list.size()) {
         if constexpr (bitreader::has_load_process) {
            stream.load_process().throw_fatal<load_process_messages::megalo::opcode_bad_function_index>({
               .opcode_type = opcode_type::action,
               .function_id = fi,
            });
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