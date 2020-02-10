#pragma once
#include "../opcode_arg.h"

namespace Megalo {
   #define megalo_opcode_arg_value_enum(name) \
      class name : public OpcodeArgValueBaseEnum { \
         public: \
            name##(); \
            static OpcodeArgValue* factory(cobb::ibitreader&) { \
               return new name##(); \
            } \
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

   class OpcodeArgValueGenericEnum : public OpcodeArgValue {
      public:
         OpcodeArgValueGenericEnum(const OpcodeArgTypeinfo& ti) : typeinfo(ti) {};
         //
         const OpcodeArgTypeinfo& typeinfo;
         uint32_t value = 0; // loaded value
         //
         virtual bool read(cobb::ibitreader& stream) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, uint64_t flags = 0) noexcept override;
   };
   extern OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoAddWeapon;
   extern OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoCHUDDestination;
   extern OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoCompareOperator;
   extern OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoDropWeapon;
   extern OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoGrenadeType;
   extern OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoMathOperator;
   extern OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoPickupPriority;
   extern OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoTeamDisposition;
   extern OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoTimerRate;
   extern OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoWaypointPriority;
}