#pragma once
#include "../opcode_arg.h"
#include "../../../formats/detailed_enum.h"

namespace Megalo {
   class OpcodeArgValueEnumSuperclass : public OpcodeArgValue {
      public:
         const DetailedEnum& base;
         uint32_t value = 0;
         //
         OpcodeArgValueEnumSuperclass(const DetailedEnum& b) : base(b) {}
         //
         virtual bool read(cobb::ibitreader& stream) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, uint64_t flags = 0) noexcept override;
   };

   #define megalo_opcode_arg_value_enum(name) \
      class name : public OpcodeArgValueEnumSuperclass { \
         public: \
            name##(); \
            static OpcodeArgTypeinfo typeinfo; \
      };

   megalo_opcode_arg_value_enum(OpcodeArgValueAddWeaponEnum);
   megalo_opcode_arg_value_enum(OpcodeArgValueCHUDDestinationEnum);
   megalo_opcode_arg_value_enum(OpcodeArgValueCompareOperatorEnum);
   megalo_opcode_arg_value_enum(OpcodeArgValueDropWeaponEnum);
   megalo_opcode_arg_value_enum(OpcodeArgValueGrenadeTypeEnum);
   megalo_opcode_arg_value_enum(OpcodeArgValueMathOperatorEnum);
   megalo_opcode_arg_value_enum(OpcodeArgValuePickupPriorityEnum);
   megalo_opcode_arg_value_enum(OpcodeArgValueTeamDispositionEnum);
   megalo_opcode_arg_value_enum(OpcodeArgValueTimerRateEnum);
   megalo_opcode_arg_value_enum(OpcodeArgValueWaypointPriorityEnum);

   megalo_opcode_arg_value_enum(OpcodeArgValueLoadoutPalette);
}