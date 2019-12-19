#pragma once
#include "../opcode_arg.h"

namespace Megalo {
   class OpcodeArgValueConstBool : public OpcodeArgValue {
      public:
         const char* baseStringTrue  = "true";
         const char* baseStringFalse = "false";
         bool value = false; // loaded value
         //
         virtual bool read(cobb::bitstream& stream) noexcept override {
            stream.read(this->value);
            return true;
         }
         virtual void write(cobb::bitwriter& stream) const noexcept override {
            stream.write(this->value);
         }
         virtual void to_string(std::string& out) const noexcept override {
            out = this->value ? this->baseStringTrue : this->baseStringFalse;
         }
         virtual void configure_with_base(const OpcodeArgBase& base) noexcept override {
            if (base.text_true)
               this->baseStringTrue  = base.text_true;
            if (base.text_false)
               this->baseStringFalse = base.text_false;
         };
         //
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueConstBool();
         }
   };
   class OpcodeArgValueConstSInt8 : public OpcodeArgValue {
      public:
         int8_t value = 0; // loaded value
         //
         virtual bool read(cobb::bitstream& stream) noexcept override {
            stream.read(this->value);
            return true;
         }
         virtual void write(cobb::bitwriter& stream) const noexcept override {
            stream.write(this->value);
         }
         virtual void to_string(std::string& out) const noexcept override {
            cobb::sprintf(out, "%d", this->value);
         }
         //
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueConstSInt8();
         }
   };

}