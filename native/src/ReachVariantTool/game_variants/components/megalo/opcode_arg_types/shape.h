#pragma once
#include "../opcode_arg.h"
#include "variables/number.h"

namespace Megalo {
   enum class ShapeType {
      none,
      sphere,
      cylinder,
      box,
      //
      _count
   };
   class OpcodeArgValueShape : public OpcodeArgValueScalar {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         cobb::bitnumber<cobb::bitcount((int)ShapeType::_count - 1), ShapeType> shapeType = ShapeType::none; // 2 bits
         OpcodeArgValueScalar radius; // or "width"
         OpcodeArgValueScalar length;
         OpcodeArgValueScalar top;
         OpcodeArgValueScalar bottom;
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
         virtual arg_compile_result compile(Compiler&, Script::string_scanner&, uint8_t part) noexcept override;
         virtual arg_compile_result compile(Compiler&, Script::VariableReference&, uint8_t part) noexcept override;
         //
         OpcodeArgValueScalar& axis(uint8_t) noexcept;
         uint8_t axis_count() const noexcept;
   };
}