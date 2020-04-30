#include "decompiler.h"
#include "../compiler/string_scanner.h"
#include "../../../types/multiplayer.h"

namespace {
   int _index_of(const std::vector<int16_t>& v, int16_t e) {
      size_t size = v.size();
      for (size_t i = 0; i < size; ++i)
         if (v[i] == e)
            return i;
      return -1;
   }
}
namespace Megalo {
   void Decompiler::decompile() {
      auto mp = this->variant.get_multiplayer_data();
      if (!mp)
         return;
      {
         auto& declarations = mp->scriptContent.variables;
         declarations.global.decompile(*this);
         declarations.player.decompile(*this);
         declarations.object.decompile(*this);
         declarations.team.decompile(*this);
         this->write_line("");
      }
      //
      auto&  triggers = mp->scriptContent.triggers;
      size_t count    = triggers.size();
      for (size_t i = 0; i < count; ++i)
         triggers[i].decompile_state.clear(&triggers[i], i);
      for (auto& trigger : triggers)
         trigger.decompile_state.setup_callees(triggers, &trigger);
      for (auto& trigger : triggers)
         trigger.decompile_state.check_if_is_function();
      //
      for (auto& trigger : triggers) {
         if (!trigger.decompile_state.is_function)
            continue;
         trigger.decompile(*this);
         this->write_line("");
      }
      for (auto& trigger : triggers) {
         if (trigger.entryType == Megalo::entry_type::subroutine)
            continue;
         if (trigger.decompile_state.is_function)
            continue;
         trigger.decompile(*this);
         this->write_line("");
      }
      for (size_t i = 0; i < count; ++i)
         triggers[i].decompile_state.clear();
   }
   //
   void Decompiler::modify_indent_count(int16_t nesting) noexcept {
      if (nesting >= 0) {
         size_t spaces = (size_t)nesting * this->indent_size;
         for (size_t i = 0; i < spaces; i++)
            this->current_indent += ' ';
      } else {
         size_t spaces = (size_t)(-nesting) * this->indent_size;
         size_t size   = this->current_indent.size();
         if (size < spaces)
            size = 0;
         else
            size -= spaces;
         this->current_indent.resize(size);
      }
   }

   void Decompiler::write(string_type& content) {
      this->current_content += content;
   }
   void Decompiler::write(std::string& content) {
      this->current_content += QString::fromUtf8(content.c_str());
   }
   void Decompiler::write(const char* content) {
      this->current_content += content;
   }
   void Decompiler::write(char_type c) {
      this->current_content += c;
   }
   void Decompiler::write_line(string_type& content) {
      this->current_content += "\r\n";
      this->current_content += this->current_indent;
      this->write(content);
   }
   void Decompiler::write_line(std::string& content) {
      this->current_content += "\r\n";
      this->current_content += this->current_indent;
      this->write(content);
   }
   void Decompiler::write_line(const char* content) {
      this->current_content += "\r\n";
      this->current_content += this->current_indent;
      this->write(content);
   }
   void Decompiler::write_string_literal(const char* content) {
      QString t = Script::string_scanner::escape(QString::fromUtf8(content), '"');
      this->write('"');
      this->write(t);
      this->write('"');
   }
   void Decompiler::write_string_literal(std::string& content) {
      QString t = Script::string_scanner::escape(QString::fromStdString(content), '"');
      this->write('"');
      this->write(t);
      this->write('"');
   }
}