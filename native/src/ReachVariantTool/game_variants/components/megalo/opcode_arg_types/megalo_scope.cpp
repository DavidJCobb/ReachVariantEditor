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
      this->conditionStart.read(stream);
      this->conditionCount.read(stream);
      this->actionStart.read(stream);
      this->actionCount.read(stream);
      return true;
   }
   void OpcodeArgValueMegaloScope::write(cobb::bitwriter& stream) const noexcept {
      this->conditionStart.write(stream);
      this->conditionCount.write(stream);
      this->actionStart.write(stream);
      this->actionCount.write(stream);
   }
   void OpcodeArgValueMegaloScope::to_string(std::string& out) const noexcept {
      // TODO
      out = "<inline-trigger>";
   }
   void OpcodeArgValueMegaloScope::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      // TODO
      out.write("<inline-trigger>");
   }
   arg_compile_result OpcodeArgValueMegaloScope::compile(Compiler& compiler, cobb::string_scanner& arg, uint8_t part) noexcept {
      // TODO
      return arg_compile_result::failure(QString("not yet implemented"));
   }
   void OpcodeArgValueMegaloScope::copy(const OpcodeArgValue* other) noexcept {
      auto cast = dynamic_cast<const OpcodeArgValueMegaloScope*>(other);
      assert(cast);
      this->conditionStart = cast->conditionStart;
      this->conditionCount = cast->conditionCount;
      this->actionStart = cast->actionStart;
      this->actionCount = cast->actionCount;
   }
}