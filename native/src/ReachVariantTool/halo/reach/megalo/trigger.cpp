#include "trigger.h"
#include "helpers/string/strcmp.h"
#include "halo/reach/bitstreams.h"
#include "halo/reach/megalo/variant_data.h"
#include "./create_operand.h"

#include "halo/reach/megalo/load_process_messages/trigger/bad_forge_label.h"
#include "halo/reach/megalo/load_process_messages/trigger/bad_opcode_slice.h"

#include "./opcodes/action.h"
#include "./opcodes/condition.h"
#include "./opcodes/all_actions.h"
#include "./operands/trigger.h"
#include "./variant_data.h"

namespace halo::reach::megalo {
   namespace {
      constexpr const auto& call_opcode = [](){
         for (const auto& item : all_actions) {
            if (cobb::strcmp(item.name, "call") == 0)
               return item;
         }
         throw;
      }();
   }

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
               stream.emit_error_at<load_process_messages::megalo::trigger_bad_forge_label>(
                  {
                     .label_index = idx,
                  },
                  stream.get_position().rewound_by_bits(decltype(idx)::max_bitcount)
               );
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
         stream.emit_error<load_process_messages::megalo::trigger_bad_opcode_slice>({
            .opcode_type  = opcode_type::action,
            .start        = ls.action_start,
            .end          = (size_t)(ls.action_start + ls.action_count),
            .count_of_all = all_actions.size(),
         });
      }
      if (ls.condition_start + ls.condition_count > all_conditions.size()) {
         valid = false;
         stream.emit_error<load_process_messages::megalo::trigger_bad_opcode_slice>({
            .opcode_type  = opcode_type::condition,
            .start        = ls.condition_start,
            .end          = (size_t)(ls.condition_start + ls.condition_count),
            .count_of_all = all_conditions.size(),
         });
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
         auto* instance = item.clone();
         //
         local_actions[i] = instance;
         this->opcodes.push_back(instance);
      }
      //
      for (size_t i = 0; i < ls.condition_count; ++i) {
         const auto& item = all_conditions[ls.condition_start + i];
         //
         auto* instance = item.clone();
         //
         if (item.load_state.execute_before >= ls.action_count) {
            this->opcodes.push_back(instance);
            continue;
         }
         //
         constexpr size_t none = -1;
         auto& list = this->opcodes;
         //
         const auto* before = local_actions[item.load_state.execute_before];
         auto it = std::find(list.begin(), list.end(), before);
         assert(it != list.end() && "Action not present in the opcode list, even though we have to have inserted it?!");
         list.insert(it, instance);
      }
   }

   void trigger::extract_nested_trigger_indices(const megalo_variant_data& variant, std::vector<size_t>& out, seen_indices_list& seen, bool recursively) const {
      //
      // Opcodes are written to the file as flat lists: all conditions in the entire script, consecutively; then 
      // all actions in the entire script, consecutively; and then all triggers. Indices and counts in the triggers 
      // and conditions make it possible to reconstruct the triggers (and properly interleave conditions and actions) 
      // once all data is loaded.
      //
      // But... how is the data written?
      //
      // I examined some of Bungie's scripts by hand. It looks like for any given trigger:
      //
      //  - The opcodes of all nested triggers are written first.
      //  - Then, the trigger's own opcodes are written.
      //  - If a trigger doesn't contain any of a given kind of opcode, it'll still have a non-zero start index.
      //
      // As an example:
      //
      //    do              | Trigger 0 |           | Trigger indices are in order from outermost to innermost...
      //       condition    |           | Cond.n  2 | ...but opcodes are ordered from innermost to outermost.
      //       action       |           | Action  3 | 
      //       action       |           | Action  4 | 
      //       condition    |           | Cond.n  5 | 
      //       do           | Trigger 1 |           | 
      //          action    | Action  0 |           | 
      //          condition | Cond.n  0 |           | 
      //          action    | Action  1 |           | All content is sequential otherwise.
      //       end          |           |           | 
      //       do           | Trigger 2 |           | 
      //          action    | Action  2 |           | 
      //       end          |           |           | 
      //       action       |           | Action  5 | 
      //    end
      //
      // Below is Trigger 2 from Alpha Zombies, but we'll start our numbering off from 0 for simplicity. This time, 
      // we'll also note that nested triggers are "execute" actions.
      /*
            CODE               | TRIGGERS  | OPS    | BY SEQUENCE            |
            -------------------+-----------+--------+------------------------+
            do                 | Trigger 0 |        |    :    :    :    :    |
               do              | Trigger 1 | A |  9 |    |    |    |    |  9 |
                  condition    |           | C |  2 |    |  2 |    |    |    |
                  do           | Trigger 2 | A |  2 |    |  2 |    |    |    |
                     condition |           | C |  0 |  0 |    |    |    |    |
                     action    |           | A |  0 |  0 |    |    |    |    |
                  end          |           |        |    :    :    :    :    |
                  do           | Trigger 3 | A |  3 |    |  3 |    |    |    |
                     condition |           | C |  1 |  1 |    |    |    |    |
                     action    |           | A |  1 |  1 |    |    |    |    |
                  end          |           |        |    :    :    :    :    |
               end             |           |        |    :    :    :    :    |
               do              | Trigger 4 | A | 10 |    |    |    |    | 10 |
                  condition    |           | C |  5 |    |    |    |  5 |    |
                  action       |           | A |  6 |    |    |    |  6 |    |
                  do           | Trigger 5 | A |  7 |    |    |    |  7 |    |
                     condition |           | C |  3 |    |    |  3 |    |    |
                     action    |           | A |  4 |    |    |  4 |    |    |
                  end          |           |        |    :    :    :    :    |
                  do           | Trigger 6 | A |  8 |    |    |    |  8 |    |
                     condition |           | C |  4 |    |    |  4 |    |    |
                     action    |           | A |  5 |    |    |  5 |    |    |
                  end          |           |        |    :    :    :    :    |
               end             |           |        |    :    :    :    :    |
            end                |           |        |    :    :    :    :    |
      */
      // As you can see, triggers are numbered in sequence with the outermost first, while opcodes are numbered 
      // in sequence with the innermost first. However, this doesn't "cross boundaries." Triggers 2, 3, 5, and 6 
      // are all nested to the same depth, but Triggers 2 and 3 share a parent (Trigger 1) while Triggers 5 and 
      // 6 share a different parent (Trigger 4); therefore, Bungie's code serializes the opcodes for the triggers 
      // in this order: 2, 3, 1, 5, 6, 4; before then writing the opcodes for the root trigger 0.
      // 
      // This function will retrieve the indices of nested triggers, matching that order. We enforce the additional 
      // limitation of avoiding duplicates in the list, since we allow user-defined functions (i.e. triggers nested 
      // under multiple places).
      //
      auto& all_triggers  = variant.script.triggers;
      auto  trigger_count = all_triggers.size();
      //
      auto& list = this->opcodes;
      auto  size = list.size();
      for (size_t i = 0; i < size; i++) {
         const auto* opcode = list[i].get();
         const auto* casted = dynamic_cast<const action*>(opcode);
         if (casted) {
            if (casted->function == &call_opcode) {
               const auto* target = dynamic_cast<const operands::trigger*>(casted->operands[0]);
               assert(target && "Found a Run Nested Trigger action with no argument specifying the trigger?!");
               //
               auto index = target->index;
               assert(index >= 0);
               assert(index < variant.script.triggers.size());
               if (seen.test(index))
                  continue;
               seen.set(index);
               if (recursively) {
                  all_triggers[index].extract_nested_trigger_indices(variant, out, seen, recursively);
               }
               out.push_back(index);
            }
         }
      }
      // Done.
   }

   void trigger::flatten_opcodes(std::vector<const condition*>& all_conditions, std::vector<const action*>& all_actions) const {
      this->load_state.condition_start = all_conditions.size();
      this->load_state.condition_count = 0;
      this->load_state.action_start    = all_actions.size();
      this->load_state.action_count    = 0;
      //
      auto& list = this->opcodes;
      auto  size = list.size();
      for (size_t i = 0; i < size; ++i) {
         const auto* opcode = list[i].get();
         if (const auto* casted = dynamic_cast<const condition*>(opcode)) {
            this->load_state.condition_count += 1;
            all_conditions.push_back(casted);
            //
            casted->load_state.execute_before = this->load_state.action_count;
            //
            continue;
         }
         if (const auto* casted = dynamic_cast<const action*>(opcode)) {
            this->load_state.action_count += 1;
            all_actions.push_back(casted);
            continue;
         }
      }
   }

   void trigger::write(bitwriter& stream) const {
      stream.write(
         block_type,
         entry_type
      );
      if (this->block_type == trigger_block_type::for_each_object_with_label) {
         forge_label_index_optional idx = -1;
         if (this->loop_forge_label) {
            idx = this->loop_forge_label->index;
         }
         stream.write(idx);
      }
      stream.write(
         load_state.condition_start,
         load_state.condition_count,
         load_state.action_start,
         load_state.action_count
      );
   }
}