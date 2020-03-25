#include "specific_variable.h"

namespace Megalo {
   bool OpcodeArgValueContextualVariableBaseClass::read(cobb::ibitreader& stream) noexcept {
      bool absence = stream.read_bits(1) != 0;
      if (absence)
         return true;
      auto scope = getScopeObjectForConstant(this->baseScope);
      this->index = stream.read_bits(scope.index_bits(this->baseType));
      return true;
   }
   void OpcodeArgValueContextualVariableBaseClass::write(cobb::bitwriter& stream) const noexcept {
      if (this->index == -1) {
         stream.write(1, 1);
         return;
      }
      stream.write(0, 1);
      auto scope = getScopeObjectForConstant(this->baseScope);
      stream.write(this->index, scope.index_bits(this->baseType));
   }
   void OpcodeArgValueContextualVariableBaseClass::to_string(std::string& out) const noexcept {
      const char* owner = "INVALID";
      switch (this->baseScope) {
         case variable_scope::object: owner = "object"; break;
         case variable_scope::player: owner = "player"; break;
         case variable_scope::team:   owner = "team";   break;
      }
      const char* type  = "INVALID";
      switch (this->baseType) {
         case variable_type::object: type = "Object"; break;
         case variable_type::player: type = "Player"; break;
         case variable_type::scalar: type = "Number"; break;
         case variable_type::team:   type = "Team";   break;
         case variable_type::timer:  type = "Timer";  break;
      }
      if (this->index == -1) {
         cobb::sprintf(out, "the %s in question's %s[None] variable", owner, type);
         return;
      }
      cobb::sprintf(out, "the %s in question's %s[%u] variable", owner, type, this->index);
   }
   //
   //
   //
   OpcodeArgTypeinfo OpcodeArgValueObjectTimerVariable::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      &OpcodeArgValueObjectTimerVariable::factory
   );
   //
   //
   //
   OpcodeArgTypeinfo OpcodeArgValueObjectPlayerVariable::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      &OpcodeArgValueObjectPlayerVariable::factory
   );
   //
   bool OpcodeArgValueObjectPlayerVariable::read(cobb::ibitreader& stream) noexcept {
      if (!this->object.read(stream))
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
   void OpcodeArgValueObjectPlayerVariable::decompile(Decompiler& out, uint64_t flags) noexcept {
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