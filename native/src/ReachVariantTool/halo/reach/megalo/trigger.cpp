#include "trigger.h"
#include "halo/reach/bitstreams.h"
#include "halo/reach/megalo/variant_data.h"

namespace halo::reach::megalo {
   void trigger::read(bitreader& stream) {
      stream.read(
         block_type,
         entry_type
      );
      if (this->block_type == trigger_block_type::for_each_object_with_label) {
         forge_label_index_optional idx;
         stream.read(idx);
         if (idx != -1) {
            auto* vd = dynamic_cast<megalo_variant_data*>(stream.get_game_variant_data());
            assert(vd);
            //
            auto& list = vd->script.forge_labels;
            if (idx >= list.size()) {
               if constexpr (bitreader::has_load_process) {
                  static_assert(false, "TODO: emit non-fatal error");
               }
            } else {
               this->loop_forge_label = &list[idx];
            }
         }
      }
      stream.read(
         load_state.condition_start,
         load_state.condition_count,
         load_state.action_start,
         load_state.action_count
      );
   }

   void trigger::extract_opcodes(bitreader& stream, const std::vector<condition>& all_conditions, const std::vector<action>& all_actions) {
      auto& ls = this->load_state;
      //
      bool valid = true;
      if (ls.action_start + ls.action_count > all_actions.size()) {
         valid = false;
         if constexpr (bitreader::has_load_process) {
            static_assert(false, "TODO: emit non-fatal error");
         }
      }
      if (ls.condition_start + ls.condition_count > all_conditions.size()) {
         valid = false;
         if constexpr (bitreader::has_load_process) {
            static_assert(false, "TODO: emit non-fatal error");
         }
      }
      if (!valid)
         return;
      this->opcodes.reserve(ls.action_count + ls.condition_count);
      //
      std::vector<action*> local_actions;
      local_actions.resize(ls.action_count);
      for (size_t i = 0; i < ls.action_count; ++i) {
         const auto& item = all_actions[ls.action_start + i];
         //
         auto* instance = new action;
         *instance = item;
         //
         local_actions[i] = instance;
         this->opcodes.push_back(std::unique_ptr<opcode>(instance));
      }
      //
      for (size_t i = 0; i < ls.condition_count; ++i) {
         const auto& item = all_conditions[ls.condition_start + i];
         //
         auto* instance = new condition;
         *instance = item;
         //
         if (item.load_state.execute_before >= ls.action_count) {
            this->opcodes.push_back(std::unique_ptr<opcode>(instance));
            continue;
         }
         const auto* before = local_actions[item.load_state.execute_before];
         auto it = std::find(this->opcodes.begin(), this->opcodes.end(), before);
         assert(it != this->opcodes.end() && "Action not present in the opcode list, even though we have to have inserted it?!");
         this->opcodes.insert(it, std::unique_ptr<opcode>(instance));
      }
   }
}