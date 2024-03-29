#include "trigger.h"
#include "limits.h"
#include "opcode_arg_types/all_indices.h"
#include "../../types/multiplayer.h"

#include "opcode_arg_types/megalo_scope.h"

namespace {
   constexpr int ce_max_index      = Megalo::Limits::max_script_labels;
   constexpr int ce_index_bitcount = cobb::bitcount(ce_max_index); // DON'T subtract 1; you can do "for each with no label" and that encodes as -1, i.e. we need an extra bit for the sign
   //
   using _index_t = int8_t;
   static_assert(std::numeric_limits<_index_t>::max() >= ce_max_index, "Use a larger type.");
}
namespace Megalo {
   #pragma region Trigger
   Trigger::~Trigger() {
      for (auto opcode : this->opcodes)
         delete opcode;
      this->opcodes.clear();
   }
   //
   bool Trigger::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      #ifdef _DEBUG
         this->bit_offset = stream.get_bitpos();
      #endif
      this->blockType.read(stream);
      this->entryType.read(stream);
      if (this->blockType == block_type::for_each_object_with_label) {
         _index_t index = stream.read_bits(ce_index_bitcount);
         if (index != -1) {
            auto& list = mp.scriptContent.forgeLabels;
            if (index < list.size())
               this->forgeLabel = &list[index];
         }
      }
      CodeBlock::read(stream, mp);
      return true;
   }
   void Trigger::write(cobb::bitwriter& stream) const noexcept {
      this->blockType.write(stream);
      this->entryType.write(stream);
      if (this->blockType == block_type::for_each_object_with_label) {
         _index_t index = -1;
         if (this->forgeLabel)
            index = this->forgeLabel->index;
         stream.write(index, ce_index_bitcount);
      }
      CodeBlock::write(stream);
   }
   //
   void Trigger::to_string(const std::vector<Trigger*>& allTriggers, std::string& out, std::string& indent) const noexcept {
      std::string line;
      //
      out += indent;
      out += "Block type: ";
      switch (this->blockType) {
         case block_type::normal:
            out += "normal";
            break;
         case block_type::for_each_object:
            out += "for each object";
            break;
         case block_type::for_each_object_with_label:
            if (!this->forgeLabel) {
               line = "for each object with label none";
            } else {
               ReachForgeLabel* f = this->forgeLabel;
               if (!f->name) {
                  cobb::sprintf(line, "label index %u", f->index);
                  break;
               }
               cobb::sprintf(line, "for each object with label %s", f->name->get_content(reach::language::english).c_str());
            }
            out += line;
            break;
         case block_type::for_each_player:
            out += "for each player";
            break;
         case block_type::for_each_player_randomly:
            out += "for each player randomly";
            break;
         case block_type::for_each_team:
            out += "for each team";
            break;
         default:
            cobb::sprintf(line, "unknown %u", (uint32_t)this->blockType);
      }
      out += "\r\n";
      //
      out += indent;
      out += "Entry type: ";
      switch (this->entryType) {
         case entry_type::local:             out += "local";          break;
         case entry_type::normal:            out += "normal";         break;
         case entry_type::on_host_migration: out += "host migration"; break;
         case entry_type::on_init:           out += "init";           break;
         case entry_type::on_local_init:     out += "local init";     break;
         case entry_type::on_object_death:   out += "object death";   break;
         case entry_type::pregame:           out += "pregame";        break;
         case entry_type::subroutine:        out += "subroutine";     break;
         default:
            cobb::sprintf(line, "unknown %u", (uint32_t)this->entryType);
      }
      out += "\r\n";
      //
      if (!this->opcodes.size()) {
         out += indent;
         out += "<Empty Trigger>";
      }
      int32_t last_condition_or_group = -1;
      for (auto& opcode : this->opcodes) {
         auto condition = dynamic_cast<const Condition*>(opcode);
         if (condition) {
            if (condition->or_group == last_condition_or_group) {
               cobb::sprintf(line, "%s[ OR] ", indent.c_str());
            } else {
               cobb::sprintf(line, "%s[CND] ", indent.c_str());
            }
            last_condition_or_group = condition->or_group;
            out += line;
            opcode->to_string(line);
            out += line;
            out += "\r\n";
            continue;
         }
         auto action = dynamic_cast<const Action*>(opcode);
         if (action) {
            if (action->function == &actionFunction_runNestedTrigger) {
               cobb::sprintf(line, "%s[ACT] Run nested trigger:\r\n", indent.c_str());
               out += line;
               //
               auto index = dynamic_cast<OpcodeArgValueTrigger*>(action->arguments[0]);
               if (index) {
                  auto i = index->value;
                  if (i < 0 || i >= allTriggers.size()) {
                     cobb::sprintf(line, "%s   <INVALID TRIGGER INDEX %d>\r\n", indent.c_str(), i);
                     out += line;
                     continue;
                  }
                  indent += "   ";
                  line.clear();
                  allTriggers[i]->to_string(allTriggers, line, indent);
                  out += line;
                  indent.resize(indent.size() - 3);
                  continue;
               }
               out += "   <INVALID>\r\n";
               continue;
            }
            cobb::sprintf(line, "%s[ACT] ", indent.c_str());
            out += line;
            opcode->to_string(line);
            out += line;
            out += "\r\n";
            continue;
         }
         out += indent;
         out += "Opcode with unrecognized type!\r\n";
      }
   }
   void Trigger::decompile(Decompiler& out) noexcept {
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
      auto& entry    = mp->scriptContent.entryPoints;
      auto& state    = this->decompile_state;
      if (state.index < 0)
         state.index = triggers.index_of(this);
      //
      uint16_t indent_count          = 0;     // needed because if-blocks aren't "real;" we "open" one every time we encounter one or more conditions in a row, so we need to remember how many "end"s to write
      bool     is_first_opcode       = true;  // see comment for (trigger_is_if_block)
      bool     is_first_condition    = true;  // if we encounter a condition and this is (true), then we need to write "if "
      bool     trigger_is_if_block   = false; // we write an initial line break on each trigger; if this trigger is an if-block, then we need to avoid writing another line break when we actually write "if". this variable and (is_first_opcode) are used to facilitate this.
      bool     writing_if_conditions = false; // if we encounter an action and this is (true), then we need to reset (is_first_condition
      //
      // Write the initial line break, and write the event handler if any:
      //
      switch (this->entryType) { // TODO: use the list of event handler trigger indices instead of the individual triggers' entry types; if a trigger has an event entry type but isn't registered as an event handler, note that with a comment
         case entry_type::normal:
         case entry_type::subroutine:
            out.write_line("");
            break;
         case entry_type::local:
            out.write_line("on local: ");
            break;
         case entry_type::on_host_migration:
            if (state.index >= 0 && state.index == entry.indices.doubleHostMigrate)
               out.write_line("on double host migration: ");
            else
               out.write_line("on host migration: ");
            break;
         case entry_type::on_init:
            out.write_line("on init: ");
            break;
         case entry_type::on_local_init:
            out.write_line("on local init: ");
            break;
         case entry_type::on_object_death:
            out.write_line("on object death: ");
            break;
         case entry_type::pregame:
            out.write_line("on pregame: ");
            break;
      }
      //
      // Write the block type, unless it's an if-block (do...end block whose first opcode is a condition), 
      // in which case we write the block type when we hit that first opcode.
      //
      if (this->decompile_state.is_function) {
         out.write(QString("function trigger_%1()").arg(state.index));
         out.modify_indent_count(1);
         ++indent_count;
      }
      switch (this->blockType) {
         case block_type::normal:
            {
               if (this->opcodes.size()) {
                  auto* first = this->opcodes[0];
                  auto* cnd   = dynamic_cast<Condition*>(first);
                  if (cnd) {
                     trigger_is_if_block = true;
                  } else if (!state.is_function) {
                     out.write("do");
                     out.modify_indent_count(1);
                     ++indent_count;
                  }
               } else if (!state.is_function) {
                  out.write("do");
                  out.modify_indent_count(1);
                  ++indent_count;
               }
            }
            break;
         case block_type::for_each_object:
            out.write("for each object do");
            out.modify_indent_count(1);
            ++indent_count;
            break;
         case block_type::for_each_object_with_label:
            {
               out.write("for each object with label ");
               if (ReachForgeLabel* f = this->forgeLabel) {
                  if (!f->name) {
                     std::string temp;
                     cobb::sprintf(temp, "%u", f->index);
                     out.write(temp);
                  } else {
                     auto english = f->name->get_content(reach::language::english).c_str();
                     out.write_string_literal(english);
                  }

               } else {
                  out.write("!ERROR:NULL!");
               }
               out.write(" do");
               out.modify_indent_count(1);
               ++indent_count;
            }
            break;
         case block_type::for_each_player:
            out.write("for each player do");
            out.modify_indent_count(1);
            ++indent_count;
            break;
         case block_type::for_each_player_randomly:
            out.write("for each player randomly do");
            out.modify_indent_count(1);
            ++indent_count;
            break;
         case block_type::for_each_team:
            out.write("for each team do");
            out.modify_indent_count(1);
            ++indent_count;
            break;
         default:
            if (state.is_function) {
               out.write(" -- unknown block type");
            } else {
               out.write("do -- unknown block type");
               out.modify_indent_count(1);
               ++indent_count;
            }
            break;
      }
      //
      // Write the opcodes:
      //
      CodeBlock::decompile(out, state.is_function, trigger_is_if_block);
      //
      // Close all open blocks.
      //
      while (indent_count-- > 0) {
         out.modify_indent_count(-1);
         out.write_line("end");
      }
   }
   #pragma endregion
   //
   #pragma region TriggerDecompileState
   void TriggerDecompileState::clear(Trigger* trigger, int16_t index_of_this_trigger) noexcept {
      this->trigger = trigger;
      this->index   = index_of_this_trigger;
      this->callees.clear();
      this->callers.clear();
      this->is_function = false;
   }
   void TriggerDecompileState::setup_callees(const std::vector<Trigger*>& allTriggers, const Trigger* mine) {
      const auto _recurse = [d_state = this, &allTriggers](this auto&& self, const CodeBlock& inlined) -> void {
         for (const auto* opcode : inlined.opcodes) {
            if (opcode->function == &actionFunction_runInlineTrigger) {
               if (opcode->arguments.empty())
                  continue;
               const auto* arg = dynamic_cast<const OpcodeArgValueMegaloScope*>(opcode->arguments[0]);
               if (!arg)
                  continue;
               self(arg->data);
               continue;
            }
            if (opcode->function != &actionFunction_runNestedTrigger)
               continue;
            const auto* arg = dynamic_cast<OpcodeArgValueTrigger*>(opcode->arguments[0]);
            if (!arg)
               continue;
            auto index = arg->value;
            if (index < 0 || index > allTriggers.size()) // invalid index
               continue;
            auto* target = allTriggers[index];
            auto& state = target->decompile_state;
            d_state->callees.push_back(&state);
            state.callers.push_back(d_state);
         }
      };
      _recurse(*(const CodeBlock*)mine);
   }
   void TriggerDecompileState::check_if_is_function() {
      if (!this->callers.size()) {
         //
         // This trigger is never called from anywhere. If it's a top-level trigger, then that's 
         // normal, but if it's a subroutine, then it must be an unused user-defined function.
         //
         if (this->trigger && this->trigger->entryType == entry_type::subroutine)
            this->is_function = true; // unused "subroutine" triggers should be flagged as functions
         return;
      }
      if (this->callers.size() > 1) {
         this->is_function = true;
         return;
      }
      //
      // The only kinds of recursion that won't be caught above are user-defined functions 
      // that recurse but are never actually called by the script. These can take two forms: 
      // a trigger which calls itself; and a set of nested triggers in which an inner trigger 
      // calls an outer trigger.
      //
      //    -- self-calling trigger: X > X
      //    -- if anything else calls X, then it has more than one caller
      //    function X()
      //       action
      //       if condition then
      //          X()
      //       end
      //    end
      //
      //    -- recursive unused function: A > B > C > A
      //    -- A only has one caller, C, and so is not caught above
      //    function A() -- A
      //       do        -- B
      //          do     -- C
      //             A()
      //          end
      //       end
      //    end
      //
      list_t entries;
      list_t recursing;
      this->get_full_callee_stack(entries, recursing);
      if (recursing.size()) {
         bool all_solo = true;  // all of these have only one caller, yes?
         bool any_func = false; // have any of these already been recognized as a recursive function?
         for (auto state : entries) {
            if (state->callers.size() > 1)
               all_solo = false;
            if (state->is_function)
               any_func = true;
         }
         if (all_solo && !any_func)
            this->is_function = true;
      }
   }
   void TriggerDecompileState::get_full_callee_stack(list_t& out, list_t& recursing) const noexcept {
      bool contains = std::find(out.begin(), out.end(), this) != out.end();
      if (!contains)
         out.push_back(const_cast<TriggerDecompileState*>(this));
      for (auto callee : this->callees) {
         contains = std::find(out.begin(), out.end(), callee) != out.end();
         if (contains) {
            recursing.push_back(callee);
            continue;
         }
         out.push_back(callee);
         callee->get_full_callee_stack(out, recursing);
      }
   }
   bool TriggerDecompileState::is_downstream_from(const TriggerDecompileState* other, const TriggerDecompileState* _checkStart) const noexcept {
      for (auto callee : this->callees) {
         if (callee == other)
            return true;
         if (callee == _checkStart) // avoid choking on recursion e.g. A > B > C > A
            continue;
         if (callee->is_downstream_from(other, _checkStart ? _checkStart : this))
            return true;
      }
      return false;
   }
   #pragma endregion
   //
   #pragma region TriggerEntryPoints
   /*static*/ void TriggerEntryPoints::_stream(cobb::ibitreader& stream, int32_t& index) noexcept {
      index = (int32_t)stream.read_bits(cobb::bitcount(Limits::max_triggers)) - 1;
   }
   /*static*/ void TriggerEntryPoints::_stream(cobb::bitwriter& stream, int32_t index) noexcept {
      if (index < 0)
         index = -1;
      stream.write(index + 1, cobb::bitcount(Limits::max_triggers));
   }
   bool TriggerEntryPoints::read(cobb::ibitreader& stream) noexcept {
      auto& i = this->indices;
      _stream(stream, i.init);
      _stream(stream, i.localInit);
      _stream(stream, i.hostMigrate);
      _stream(stream, i.doubleHostMigrate);
      _stream(stream, i.objectDeath);
      _stream(stream, i.local);
      _stream(stream, i.pregame);
      return true;
   }
   void TriggerEntryPoints::write(cobb::bitwriter& stream) const noexcept {
      auto& i = this->indices;
      _stream(stream, i.init);
      _stream(stream, i.localInit);
      _stream(stream, i.hostMigrate);
      _stream(stream, i.doubleHostMigrate);
      _stream(stream, i.objectDeath);
      _stream(stream, i.local);
      _stream(stream, i.pregame);
   }
   void TriggerEntryPoints::write_placeholder(cobb::bitwriter& stream) const noexcept {
      stream.write(-1 + 1, cobb::bitcount(Limits::max_triggers));
      stream.write(-1 + 1, cobb::bitcount(Limits::max_triggers));
      stream.write(-1 + 1, cobb::bitcount(Limits::max_triggers));
      stream.write(-1 + 1, cobb::bitcount(Limits::max_triggers));
      stream.write(-1 + 1, cobb::bitcount(Limits::max_triggers));
      stream.write(-1 + 1, cobb::bitcount(Limits::max_triggers));
      stream.write(-1 + 1, cobb::bitcount(Limits::max_triggers));
   }
   int32_t TriggerEntryPoints::get_index_of_event(entry_type et) const noexcept {
      auto& i = this->indices;
      switch (et) {
         case entry_type::local: return i.local;
         case entry_type::on_host_migration: return i.hostMigrate;
         case entry_type::on_init: return i.init;
         case entry_type::on_local_init: return i.localInit;
         case entry_type::on_object_death: return i.objectDeath;
         case entry_type::pregame: return i.pregame;
      }
      return TriggerEntryPoints::none;
   }
   void TriggerEntryPoints::set_index_of_event(entry_type et, int32_t index) noexcept {
      auto& i = this->indices;
      switch (et) {
         case entry_type::local: i.local = index; return;
         case entry_type::on_host_migration: i.hostMigrate = index; return;
         case entry_type::on_init: i.init = index; return;
         case entry_type::on_local_init: i.localInit = index; return;
         case entry_type::on_object_death: i.objectDeath = index; return;
         case entry_type::pregame: i.pregame = index; return;
      }
   }
   #pragma endregion
}