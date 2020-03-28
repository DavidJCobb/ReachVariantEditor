#pragma once
#include "../opcode_arg.h"
#include "variables/number.h"
#include "variables/timer.h"
#include "../limits.h"
#include "../widgets.h"

class GameVariantDataMultiplayer;

namespace Megalo {
   class OpcodeArgValueWidget : public OpcodeArgValue {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         cobb::refcount_ptr<HUDWidgetDeclaration> value;
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
   };

   enum class MeterType {
      none,
      timer,
      number,
   };
   class OpcodeArgValueMeterParameters : public OpcodeArgValue {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         cobb::bitnumber<2, MeterType> type = MeterType::none;
         OpcodeArgValueTimer  timer;
         OpcodeArgValueScalar numerator;
         OpcodeArgValueScalar denominator;
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
   };
}