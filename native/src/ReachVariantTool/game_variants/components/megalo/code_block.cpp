#include "./code_block.h"
#include <cassert>
#include "../../../helpers/bitnumber.h"
#include "../../../helpers/bitwriter.h"
#include "../../../helpers/bitwise.h"
#include "../../../helpers/stream.h"
#include "../../types/multiplayer.h"
#include "./opcode_arg_types/all_indices.h"
#include "./opcode_arg_types/megalo_scope.h"
#include "./actions.h"
#include "./conditions.h"

namespace Megalo {
   CodeBlock::~CodeBlock() {
      for (auto opcode : this->opcodes)
         delete opcode;
      this->opcodes.clear();
   }
   CodeBlock::CodeBlock(const CodeBlock& o) {
      this->raw = o.raw;

      size_t size = o.opcodes.size();
      this->opcodes.resize(size);
      for (size_t i = 0; i < size; ++i) {
         this->opcodes[i] = o.opcodes[i]->clone();
      }
   }
   CodeBlock::CodeBlock(CodeBlock&& o) {
      *this = std::move(o);
   }

   CodeBlock& CodeBlock::operator=(const CodeBlock& o) {
      for (auto opcode : this->opcodes)
         delete opcode;
      this->opcodes.clear();

      size_t size = o.opcodes.size();
      this->opcodes.resize(size);
      for (size_t i = 0; i < size; ++i) {
         this->opcodes[i] = o.opcodes[i]->clone();
      }

      this->raw = o.raw;

      return *this;
   }
   CodeBlock& CodeBlock::operator=(CodeBlock&& o) {
      std::swap(this->opcodes, o.opcodes);
      std::swap(this->raw, o.raw);
      return *this;
   }
   
   bool CodeBlock::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      #ifdef _DEBUG
         this->raw.bit_offset = stream.get_bitpos();
      #endif
      this->raw.conditionStart.read(stream);
      this->raw.conditionCount.read(stream);
      this->raw.actionStart.read(stream);
      this->raw.actionCount.read(stream);
      return true;
   }
   void CodeBlock::postprocess_opcodes(const std::vector<Condition>& conditions, const std::vector<Action>& actions) noexcept {
      //
      // Opcodes are written to the file as flat lists: all conditions in the entire script, consecutively; then 
      // all actions in the entire script, consecutively; and then all triggers. Indices and counts in the triggers 
      // and conditions make it possible to reconstruct the triggers (and properly interleave conditions and actions) 
      // once all data is loaded.
      //
      auto& raw = this->raw;
      std::vector<Action*> temp;
      //
      this->opcodes.reserve(raw.actionCount + raw.conditionCount);
      temp.reserve(raw.actionCount);
      //
      if (raw.actionCount > 0) {
         assert(raw.actionStart < actions.size() && "Bad trigger first-action-index."); // TODO: fail with an error in-app instead of asserting
         if (raw.actionStart >= 0) {
            size_t end = raw.actionStart + raw.actionCount;
            if (end <= actions.size()) { // TODO: if (end) is too high, fail with an error
               for (size_t i = raw.actionStart; i < end; i++) {
                  auto* instance = new Action(actions[i]);
                  this->opcodes.push_back(instance);
                  temp.push_back(instance);

                  if (instance->function == &actionFunction_runInlineTrigger) {
                     auto* casted = dynamic_cast<OpcodeArgValueMegaloScope*>(instance->arguments[0]);
                     assert(casted);
                     casted->data.postprocess_opcodes(conditions, actions);
                  }
               }
            }
         }
      }
      if (raw.conditionCount > 0) {
         assert(raw.conditionStart < conditions.size() && "Bad trigger first-conditions-index."); // TODO: fail with an error in-app instead of asserting
         if (raw.conditionStart >= 0) {
            size_t end = raw.conditionStart + raw.conditionCount;
            if (end <= conditions.size()) // TODO: if (end) is too high, fail with an error
               for (size_t i = raw.conditionStart; i < end; i++) {
                  auto& condition = conditions[i];
                  if (condition.action >= raw.actionCount) {
                     this->opcodes.push_back(new Condition(condition));
                     continue;
                  }
                  auto target = temp[condition.action];
                  auto it = std::find(this->opcodes.begin(), this->opcodes.end(), target);
                  assert(it != this->opcodes.end() && "Action not present in the opcode list, even though we have to have inserted it?!");
                  this->opcodes.insert(it, new Condition(condition));
               }
         }
      }
   }

   void CodeBlock::write(cobb::bitwriter& stream) const noexcept {
      this->raw.conditionStart.write(stream);
      this->raw.conditionCount.write(stream);
      this->raw.actionStart.write(stream);
      this->raw.actionCount.write(stream);
   }
   
   void CodeBlock::prep_for_flat_opcode_lists() {
      this->raw.serialized = false;
      for (auto* opcode : this->opcodes) {
         auto* action = dynamic_cast<Action*>(opcode);
         if (!action)
            continue;
         if (action->function != &actionFunction_runInlineTrigger)
            continue;

         auto* casted = dynamic_cast<OpcodeArgValueMegaloScope*>(action->arguments[0]);
         if (!casted)
            continue;
         casted->data.prep_for_flat_opcode_lists();
      }
   }
   void CodeBlock::generate_flat_opcode_lists(GameVariantDataMultiplayer& mp, std::vector<Condition*>& allConditions, std::vector<Action*>& allActions) {
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
      if (this->raw.serialized) { // guard needed since we're doing nested triggers out of flat order
         return;
      }
      this->raw.serialized = true;
      //
      auto& triggers = mp.scriptContent.triggers;
      size_t size = this->opcodes.size();
      //
      // Recurse over nested triggers first, so that opcodes are serialized inner-first.
      //
      for (size_t i = 0; i < size; i++) {
         auto* opcode = this->opcodes[i];
         auto  action = dynamic_cast<const Action*>(opcode);
         if (action) {
            if (action->function == &actionFunction_runNestedTrigger) {
               auto* arg = dynamic_cast<OpcodeArgValueTrigger*>(action->arguments[0]);
               assert(arg && "Found a Run Nested Trigger action with no argument specifying the trigger?!");
               auto  i   = arg->value;
               assert(i >= 0 && i < triggers.size() && "Found a Run Nested Trigger action with an out-of-bounds index.");
               triggers[i].generate_flat_opcode_lists(mp, allConditions, allActions);
            } else if (action->function == &actionFunction_runInlineTrigger) {
               auto* arg = dynamic_cast<OpcodeArgValueMegaloScope*>(action->arguments[0]);
               assert(arg && "Found a Run Inline Trigger action with no trigger data?!");
               arg->data.generate_flat_opcode_lists(mp, allConditions, allActions);
            }
         }
      }
      //
      // Now run over our own opcodes.
      //
      this->raw.conditionStart = allConditions.size();
      this->raw.conditionCount = 0;
      this->raw.actionStart    = allActions.size();
      this->raw.actionCount    = 0;
      for (size_t i = 0; i < size; ++i) {
         auto* opcode    = this->opcodes[i];
         auto  condition = dynamic_cast<Condition*>(opcode);
         if (condition) {
            this->raw.conditionCount += 1;
            allConditions.push_back(condition);
            //
            condition->action = this->raw.actionCount;
            //
            continue;
         }
         auto action = dynamic_cast<Action*>(opcode);
         if (action) {
            this->raw.actionCount += 1;
            allActions.push_back(action);
            continue;
         }
      }
   }
   
   void CodeBlock::decompile(Decompiler& out, bool is_function) noexcept {
      //
      // We need to handle block structure, line breaks, and similar formatting here. Normally this would be 
      // pretty straightforward: each trigger is one single code block; we open another code block only when 
      // we find the opcode to run a nested trigger, right? Well yes, but actually no. Conditions can appear 
      // in the middle of a trigger and should be written as the start of a new if-block, which should wrap 
      // everything after those conditions. As such, a single trigger can actually be multiple blocks.
      //
      // If the trigger itself is an if-block (i.e. it's not a loop, and its first opcode is a condition), 
      // then we have to "open" the block not at the start of the trigger but rather when we start iterating 
      // over opcodes. We need a few bools to help keep track of this -- to know when to insert line breaks 
      // before "if" statements and so on.
      //
      auto mp = out.get_variant().get_multiplayer_data();
      if (!mp)
         return;
      auto& triggers = mp->scriptContent.triggers;
      //
      uint16_t indent_count          = 0;     // needed because if-blocks aren't "real;" we "open" one every time we encounter one or more conditions in a row, so we need to remember how many "end"s to write
      bool     is_first_opcode       = true;  // see comment for (trigger_is_if_block)
      bool     is_first_condition    = true;  // if we encounter a condition and this is (true), then we need to write "if "
      bool     trigger_is_if_block   = false; // we write an initial line break on each trigger; if this trigger is an if-block, then we need to avoid writing another line break when we actually write "if". this variable and (is_first_opcode) are used to facilitate this.
      bool     writing_if_conditions = false; // if we encounter an action and this is (true), then we need to reset (is_first_condition

      //
      // Write the opcodes:
      //
      int32_t     last_condition_or_group = -1;
      std::string line;
      for (auto* opcode : this->opcodes) {
         auto condition = dynamic_cast<const Condition*>(opcode);
         bool _first    = is_first_opcode;
         is_first_opcode = false;
         if (condition) {
            if (is_first_condition) {
               if ((!trigger_is_if_block || is_function) || !_first) // if this trigger IS an if-block and we're writing the entire block's "if", then don't write a line break because we did that above
                  out.write_line("");
               out.write("if ");
               out.modify_indent_count(1);
               ++indent_count;
               is_first_condition    = false;
               writing_if_conditions = true;
            } else {
               if (condition->or_group == last_condition_or_group)
                  out.write("or ");
               else
                  out.write("and ");
            }
            last_condition_or_group = condition->or_group;
            opcode->decompile(out);
            out.write(' ');
            continue;
         }
         if (writing_if_conditions) {
            out.write("then ");
            last_condition_or_group = -1;
            writing_if_conditions   = false;
            is_first_condition      = true;
         }
         if (opcode->function == &actionFunction_runNestedTrigger) {
            auto index = dynamic_cast<OpcodeArgValueTrigger*>(opcode->arguments[0]);
            if (index) {
               auto i = index->value;
               if (i < 0 || i >= triggers.size()) {
                  cobb::sprintf(line, "-- execute nested trigger with invalid index %d", i);
                  out.write_line(line);
                  continue;
               }
               auto& other = triggers[i];
               if (other.decompile_state.is_function) {
                  out.write_line(QString("trigger_%1()").arg(i));
                  continue;
               }
               other.decompile(out);
               continue;
            }
            out.write_line("-- invalid \"execute nested trigger\" opcode");
            continue;
         } else if (opcode->function == &actionFunction_runInlineTrigger) {
            auto* casted = dynamic_cast<OpcodeArgValueMegaloScope*>(opcode->arguments[0]);
            assert(casted);
            out.write_line("inline: do");
            out.modify_indent_count(1);
            casted->data.decompile(out);
            out.modify_indent_count(-1);
            out.write_line("end");
            continue;
         }
         out.write_line("");
         opcode->decompile(out);
         continue;
      }
      if (writing_if_conditions) { // can be true if the last opcode was a condition
         out.write("then ");
         writing_if_conditions = false;
         is_first_condition = true;
      }
      //
      // Close all open blocks. Remember: conditions encountered in the middle of the trigger count 
      // as new if-blocks that we have to open, so we have to de-indent and write an "end" keyword 
      // for each of those as well as for the trigger itself.
      //
      while (indent_count-- > 0) {
         out.modify_indent_count(-1);
         out.write_line("end");
      }
   }

   void CodeBlock::count_contents(size_t& conditions, size_t& actions) const noexcept {
      for (auto& opcode : this->opcodes) {
         if (dynamic_cast<const Condition*>(opcode))
            ++conditions;
         else if (dynamic_cast<const Action*>(opcode)) {
            ++actions;
            if (opcode->function == &actionFunction_runInlineTrigger) {
               if (opcode->arguments.size() > 0) {
                  const auto* scope = dynamic_cast<const OpcodeArgValueMegaloScope*>(opcode->arguments[0]);
                  if (scope)
                     scope->data.count_contents(conditions, actions);
               }
            }
         }
      }
   }
}