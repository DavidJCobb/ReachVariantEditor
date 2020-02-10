#pragma once
#include "../opcode_arg.h"

namespace Megalo {
   #define megalo_opcode_arg_value_flags(name) \
      class name : public OpcodeArgValueBaseFlags { \
         public: \
            name##(); \
            static OpcodeArgValue* factory(cobb::ibitreader&) { \
               return new name##(); \
            } \
      };

   megalo_opcode_arg_value_flags(OpcodeArgValueCreateObjectFlags);
   megalo_opcode_arg_value_flags(OpcodeArgValueKillerTypeFlags);
   megalo_opcode_arg_value_flags(OpcodeArgValuePlayerUnusedModeFlags);
   
   class OpcodeArgValueGenericFlagsMask : public OpcodeArgValue {
      public:
         OpcodeArgValueGenericFlagsMask(const OpcodeArgTypeinfo& ti) : typeinfo(ti) {};
         //
         const OpcodeArgTypeinfo& typeinfo;
         uint32_t value = 0; // loaded value
         //
         virtual bool read(cobb::ibitreader& stream) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, uint64_t flags = 0) noexcept override;
   };
   extern OpcodeArgTypeinfo OpcodeArgValueFlagsMaskTypeinfoCreateObject;
   extern OpcodeArgTypeinfo OpcodeArgValueFlagsMaskTypeinfoKillerType;
   extern OpcodeArgTypeinfo OpcodeArgValueFlagsMaskTypeinfoPlayerUnusedMode;
}