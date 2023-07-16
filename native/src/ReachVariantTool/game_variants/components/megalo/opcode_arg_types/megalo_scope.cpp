#include "megalo_scope.h"
#include "../compiler/compiler.h"

namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueMegaloScope::typeinfo = OpcodeArgTypeinfo(
      "_megalo_scope",
      "Megalo Scope",
      "Data indicating which conditions and actions in a game variant are invoked by a particular code block.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueMegaloScope>
   );

   bool OpcodeArgValueMegaloScope::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      this->data.read(stream, mp);
      return true;
   }
   void OpcodeArgValueMegaloScope::write(cobb::bitwriter& stream) const noexcept {
      this->data.write(stream);
   }
   void OpcodeArgValueMegaloScope::to_string(std::string& out) const noexcept {
      out = "<inline-trigger>";
   }
   void OpcodeArgValueMegaloScope::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      //
      // This should never run; the decompile code for triggers and code blocks should special-case 
      // this operand's containing opcode, same as the Run Nested Trigger opcode.
      //
      out.write("<inline-trigger>");
   }
   arg_compile_result OpcodeArgValueMegaloScope::compile(Compiler& compiler, cobb::string_scanner& arg, uint8_t part) noexcept {
      //
      // This should never run; the compile code should special-case this operand's containing opcode, 
      // same as the Run Nested Trigger opcode.
      //
      return arg_compile_result::failure(QString("internal error: OpcodeArgValueMegaloScope::compile should never be called directly; contact ReachVariantTool's current author/maintainer"));
   }
   void OpcodeArgValueMegaloScope::copy(const OpcodeArgValue* other) noexcept {
      auto cast = dynamic_cast<const OpcodeArgValueMegaloScope*>(other);
      assert(cast);
      this->data = cast->data;
   }
}