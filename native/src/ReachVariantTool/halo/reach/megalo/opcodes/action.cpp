#include "action.h"
#include "all_actions.h"
#include "halo/reach/bitstreams.h"
#include "halo/reach/megalo/create_operand.h"

#include "halo/reach/megalo/load_process_messages/opcode/bad_function_index.h"

namespace halo::reach::megalo {
   namespace {
      constexpr auto& function_list = all_actions;
      constexpr auto  function_index_bitcount = std::bit_width(function_list.size() - 1);
   }

   void action::read(bitreader& stream) {
      size_t fi = stream.read_bits(function_index_bitcount);
      if (fi >= function_list.size()) {
         stream.throw_fatal_at<load_process_messages::megalo::opcode_bad_function_index>(
            {
               .opcode_type = opcode_type::action,
               .function_id = fi,
            },
            stream.get_position().rewound_by_bits(function_index_bitcount)
         );
         return;
      }
      this->function = &function_list[fi];
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
   void action::write(bitwriter& stream) const {
      size_t fi = 0;
      for (size_t i = 0; i < function_list.size(); ++i) {
         if (this->function == &function_list[i]) {
            fi = i;
            break;
         }
      }
      //
      stream.write_bits(function_index_bitcount, fi);
      if (fi == 0)
         return;
      //
      size_t count = this->function->operands.size();
      assert(this->operands.size() == count);
      for (size_t i = 0; i < count; ++i) {
         const auto* item = this->operands[i];
         assert(item);
         item->write(stream);
      }
   }

   action* action::clone() const {
      auto* dst = new action;
      dst->function = this->function;
      
      auto size = this->operands.size();
      dst->operands.resize(size);
      for (size_t i = 0; i < size; ++i) {
         dst->operands[i] = clone_operand(*this->operands[i]);
      }

      return dst;
   }
}