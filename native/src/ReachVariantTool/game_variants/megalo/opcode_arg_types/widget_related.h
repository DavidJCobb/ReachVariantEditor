#pragma once
#include "../opcode_arg.h"
#include "scalar.h"
#include "timer.h"

// NOTE: widget index is in all_indices.h

namespace Megalo {
   enum class MeterType {
      none,
      timer,
      number,
   };
   class OpcodeArgValueMeterParameters : public OpcodeArgValue {
      public:
         MeterType type = MeterType::none;
         OpcodeArgValueTimer  timer;
         OpcodeArgValueScalar numerator;
         OpcodeArgValueScalar denominator;
         //
         virtual bool read(cobb::bitstream& stream) noexcept override {
            this->type = (MeterType)stream.read_bits(2);
            switch (this->type) {
               case MeterType::none:
                  break;
               case MeterType::timer:
                  this->timer.read(stream);
                  break;
               case MeterType::number:
                  this->numerator.read(stream);
                  this->denominator.read(stream);
                  break;
               default:
                  printf("Widget Meter Parameters had bad type %d.\n", (int)this->type);
                  return false;
            }
            return true;
         }
         virtual void to_string(std::string& out) const noexcept override {
            std::string temp;
            switch (this->type) {
               case MeterType::none:
                  out = "no meter";
                  return;
               case MeterType::timer:
                  this->timer.to_string(out);
                  out = "timer " + out;
                  return;
               case MeterType::number:
                  this->numerator.to_string(out);
                  this->denominator.to_string(temp);
                  cobb::sprintf(out, "%s / %s", out.c_str(), temp.c_str());
                  return;
            }
         }
         //
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueMeterParameters;
         }
   };
}