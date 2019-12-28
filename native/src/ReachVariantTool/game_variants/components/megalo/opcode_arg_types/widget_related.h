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
         cobb::bitnumber<2, MeterType> type = MeterType::none;
         OpcodeArgValueTimer  timer;
         OpcodeArgValueScalar numerator;
         OpcodeArgValueScalar denominator;
         //
         virtual bool read(cobb::bitreader& stream) noexcept override {
            this->type.read(stream);
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
                  printf("Widget Meter Parameters had bad type %u.\n", (int)this->type);
                  return false;
            }
            return true;
         }
         virtual void write(cobb::bitwriter& stream) const noexcept override {
            this->type.write(stream);
            switch (this->type) {
               case MeterType::none:
                  break;
               case MeterType::timer:
                  this->timer.write(stream);
                  break;
               case MeterType::number:
                  this->numerator.write(stream);
                  this->denominator.write(stream);
                  break;
               default:
                  assert(false && "Widget Meter Parameters had a bad type.");
            }
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
         static OpcodeArgValue* factory(cobb::bitreader&) {
            return new OpcodeArgValueMeterParameters;
         }
   };
}