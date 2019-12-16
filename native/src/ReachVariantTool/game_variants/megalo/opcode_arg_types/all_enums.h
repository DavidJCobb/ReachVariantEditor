#pragma once
#include "../opcode_arg.h"

namespace Megalo {
   #define megalo_opcode_arg_value_enum(name) \
      class name : public OpcodeArgValueBaseEnum { \
         public: \
            name##(); \
            static OpcodeArgValue* factory(cobb::bitstream&) { \
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
   megalo_opcode_arg_value_enum(OpcodeArgValueWaypointPriorityEnum);
}