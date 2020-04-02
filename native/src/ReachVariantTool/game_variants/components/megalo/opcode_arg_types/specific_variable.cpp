#include "specific_variable.h"

namespace Megalo {
   bool OpcodeArgValueObjectTimerVariable::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      bool absence = stream.read_bits(1) != 0;
      if (absence)
         return true;
      auto scope = getScopeObjectForConstant(variable_scope::object);
      this->index = stream.read_bits(scope.index_bits(variable_type::timer));
      return true;
   }
   void OpcodeArgValueObjectTimerVariable::write(cobb::bitwriter& stream) const noexcept {
      if (this->index == -1) {
         stream.write(1, 1);
         return;
      }
      stream.write(0, 1);
      auto scope = getScopeObjectForConstant(variable_scope::object);
      stream.write(this->index, scope.index_bits(variable_type::timer));
   }
   void OpcodeArgValueObjectTimerVariable::to_string(std::string& out) const noexcept {
      const char* owner = "object";
      const char* type  = "timer";
      if (this->index == -1) {
         cobb::sprintf(out, "its %s[None] variable", owner, type);
         return;
      }
      cobb::sprintf(out, "its %s[%u] variable", owner, type, this->index);
   }
   void OpcodeArgValueObjectTimerVariable::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      if (this->index < 0) {
         out.write("none");
         return;
      }
      std::string temp;
      cobb::sprintf(temp, "%u", this->index);
      out.write(temp);
   }
   //
   //
   //
   OpcodeArgTypeinfo OpcodeArgValueObjectTimerVariable::typeinfo = OpcodeArgTypeinfo(
      "_object_timer_variable",
      "Object Timer Variable Index",
      "A number indicating one of the timer variables on an object.",
      //
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueObjectTimerVariable>
   );
   //
   //
   //
   OpcodeArgTypeinfo OpcodeArgValueObjectPlayerVariable::typeinfo = OpcodeArgTypeinfo(
      "_object_player_variable",
      "Object Player Variable",
      "An object-and-player reference.",
      //
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueObjectPlayerVariable>
   );
   //
   bool OpcodeArgValueObjectPlayerVariable::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      if (!this->object.read(stream, mp))
         return false;
      bool absence = stream.read_bits(1) != 0;
      if (absence)
         return true;
      this->playerIndex = stream.read_bits(MegaloVariableScopeObject.index_bits(variable_type::player));
      return true;
   }
   void OpcodeArgValueObjectPlayerVariable::write(cobb::bitwriter& stream) const noexcept {
      this->object.write(stream);
      if (this->playerIndex == -1) {
         stream.write(1, 1);
         return;
      }
      stream.write(0, 1);
      stream.write(this->playerIndex, MegaloVariableScopeObject.index_bits(variable_type::player));
   }
   void OpcodeArgValueObjectPlayerVariable::to_string(std::string& out) const noexcept {
      this->object.to_string(out);
      if (this->playerIndex == -1) {
         out += " - no/all? players";
         return;
      }
      cobb::sprintf(out, "%s.Player[%d]", out.c_str(), this->playerIndex);
   }
   void OpcodeArgValueObjectPlayerVariable::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      if (flags & Decompiler::flags::is_call_context) {
         this->object.decompile(out);
         return;
      }
      if (this->playerIndex < 0) {
         out.write("none");
         return;
      }
      std::string temp;
      cobb::sprintf(temp, "%u", this->playerIndex);
      out.write(temp);
   }
}