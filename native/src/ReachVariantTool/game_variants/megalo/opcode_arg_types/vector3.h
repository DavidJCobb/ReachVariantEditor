#pragma once
#include "../opcode_arg.h"

namespace Megalo {
   class OpcodeArgValueVector3 : public OpcodeArgValue {
      public:
         struct {
            int8_t x = 0;
            int8_t y = 0;
            int8_t z = 0;
         } value; // loaded value
         //
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueVector3();
         }
         virtual bool read(cobb::bitstream& stream) noexcept override {
            stream.read(this->value.x);
            stream.read(this->value.y);
            stream.read(this->value.z);
            return true;
         }
         virtual void to_string(std::string& out) const noexcept override {
            cobb::sprintf(out, "(%d, %d, %d)", this->value.x, this->value.y, this->value.z);
         }
   };
}