#include "trigger.h"
#include "limits.h"
#include "opcode_arg_types/all_indices.h"

namespace Megalo {
   bool Trigger::read(cobb::bitstream& stream) noexcept {
      this->blockType = (block_type)stream.read_bits(3);
      this->entry     = (trigger_type)stream.read_bits(3);
      if (this->blockType == block_type::for_each_object_with_label) {
         this->labelIndex = stream.read_bits(cobb::bitcount(Limits::max_script_labels - 1)) - 1;
      }
      this->raw.conditionStart = stream.read_bits(cobb::bitcount(Limits::max_conditions) - 1);
      this->raw.conditionCount = stream.read_bits(cobb::bitcount(Limits::max_conditions));
      this->raw.actionStart = stream.read_bits(cobb::bitcount(Limits::max_actions) - 1);
      this->raw.actionCount = stream.read_bits(cobb::bitcount(Limits::max_actions));
      return true;
   }
   void Trigger::postprocess_opcodes(const std::vector<Condition>& ac, const std::vector<Action>& aa) noexcept {
      auto& raw = this->raw;
      if (raw.conditionStart >= 0 && raw.conditionStart < ac.size()) {
         size_t end = raw.conditionStart + raw.conditionCount;
         if (end <= ac.size())
            for (size_t i = raw.conditionStart; i < end; i++)
               this->conditions.push_back(ac[i]);
      }
      if (raw.actionStart >= 0 && raw.actionStart < ac.size()) {
         size_t end = raw.actionStart + raw.actionCount;
         if (end <= aa.size())
            for (size_t i = raw.actionStart; i < end; i++)
               this->actions.push_back(aa[i]);
      }
   }
   void Trigger::to_string(const std::vector<Trigger>& allTriggers, std::string& out, std::string& indent) const noexcept {
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
            cobb::sprintf(line, "for each object with label #%d", this->labelIndex);
            out += line;
            break;
         case block_type::for_each_player:
            out += "for each player";
            break;
         case block_type::for_each_player_random:
            out += "for each player random?";
            break;
         case block_type::for_each_team:
            out += "for each team";
            break;
         default:
            cobb::sprintf(line, "unknown %u", (uint32_t)this->blockType);
      }
      out += '\n';
      //
      out += indent;
      out += "Entry type: ";
      switch (this->entry) {
         case trigger_type::local:             out += "local";          break;
         case trigger_type::normal:            out += "normal";         break;
         case trigger_type::on_host_migration: out += "host migration"; break;
         case trigger_type::on_init:           out += "init";           break;
         case trigger_type::on_local_init:     out += "local init";     break;
         case trigger_type::on_object_death:   out += "object death";   break;
         case trigger_type::pregame:           out += "pregame";        break;
         case trigger_type::subroutine:        out += "subroutine";     break;
         default:
            cobb::sprintf(line, "unknown %u", (uint32_t)this->entry);
      }
      out += '\n';
      //
      cobb::sprintf(line, "%sConditions:\n", indent.c_str());
      out += line;
      if (!this->conditions.size())
         out += "   <No Conditions>\n";
      for (auto& opcode : this->conditions) {
         cobb::sprintf(line, "%s   ", indent.c_str());
         out += line;
         opcode.to_string(line);
         out += line;
         out += '\n';
      }
      //
      cobb::sprintf(line, "%sActions:\n", indent.c_str());
      out += line;
      if (!this->actions.size())
         out += "   <No Actions>\n";
      for (auto& opcode : this->actions) {
         if (opcode.function == &actionFunction_runNestedTrigger) {
            cobb::sprintf(line, "%s   Run nested trigger:\n", indent.c_str());
            out += line;
            //
            auto index = dynamic_cast<OpcodeArgValueTrigger*>(opcode.arguments[0]);
            if (index) {
               auto i = index->value;
               if (i < 0 || i >= allTriggers.size()) {
                  cobb::sprintf(line, "%s      <INVALID TRIGGER INDEX %d>\n", indent.c_str(), i);
                  out += line;
                  continue;
               }
               indent += "      ";
               line.clear();
               allTriggers[i].to_string(allTriggers, line, indent);
               out += line;
               indent.resize(indent.size() - 6);
               continue;
            }
            out += "      <INVALID>\n";
            continue;
         }
         cobb::sprintf(line, "%s   ", indent.c_str());
         out += line;
         opcode.to_string(line);
         out += line;
         out += '\n';
      }
   }
}