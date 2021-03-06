#pragma once
#include "../opcode_arg.h"
#include "../../../formats/detailed_enum.h"
#include "../limits.h"

namespace Megalo {
   namespace enums {
      extern DetailedEnum engine_icon;
      extern DetailedEnum hud_widget_icon;
   }
   class OpcodeArgValueIconBase : public OpcodeArgValue {
      protected:
         const DetailedEnum& icons;
         const int bitcount;
         //
      public:
         OpcodeArgValueIconBase(const DetailedEnum& de, int bc) : icons(de), bitcount(bc) {};
         //
         static constexpr int8_t none = -1;
         //
         int8_t value = -1;
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
         virtual arg_compile_result compile(Compiler&, cobb::string_scanner&, uint8_t part) noexcept override;
         virtual void copy(const OpcodeArgValue*) noexcept override;
   };

   class OpcodeArgValueEngineIcon : public OpcodeArgValueIconBase {
      megalo_opcode_arg_value_make_create_override;
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         OpcodeArgValueEngineIcon() : OpcodeArgValueIconBase(enums::engine_icon, 7) {}
   };
   class OpcodeArgValueHUDWidgetIcon : public OpcodeArgValueIconBase {
      megalo_opcode_arg_value_make_create_override;
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         OpcodeArgValueHUDWidgetIcon() : OpcodeArgValueIconBase(enums::hud_widget_icon, 6) {}
   };
}
