#pragma once
#include "../opcode_arg.h"

namespace Megalo {
   #define megalo_opcode_arg_value_flags(name) \
      class name : public OpcodeArgValueBaseFlags { \
         public: \
            name##(); \
            static OpcodeArgValue* factory(cobb::bitreader&) { \
               return new name##(); \
            } \
      };

   megalo_opcode_arg_value_flags(OpcodeArgValueCreateObjectFlags);
   megalo_opcode_arg_value_flags(OpcodeArgValueKillerTypeFlags);
   megalo_opcode_arg_value_flags(OpcodeArgValuePlayerUnusedModeFlags);
}