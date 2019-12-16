#pragma once
#include <cassert>
#include <string>
#include "../../helpers/bitstream.h"
#include "../../helpers/strings.h"
#include "enums.h"

namespace Megalo {
   class OpcodeArgBase;
   
   class OpcodeArgValue {
      public:
         virtual bool read(cobb::bitstream&) noexcept = 0;
         virtual void to_string(std::string& out) const noexcept = 0;
         virtual void configure_with_base(const OpcodeArgBase&) noexcept {}; // used for bool options so they can stringify intelligently
         //
         static OpcodeArgValue* factory(cobb::bitstream& stream) {
            assert(false && "OpcodeArgValue::factory should never be called; any subclasses that can actually appear in a file should override it.");
            return nullptr;
         }
   };
   using OpcodeArgValueFactory = OpcodeArgValue* (*)(cobb::bitstream& stream);
   extern OpcodeArgValue* OpcodeArgAnyVariableFactory(cobb::bitstream& stream);
   extern OpcodeArgValue* OpcodeArgTeamOrPlayerVariableFactory(cobb::bitstream& stream);
   //
   class OpcodeArgBase {
      public:
         const char* name = "";
         OpcodeArgValueFactory factory = nullptr;
         bool is_out_variable = false;
         //
         const char* text_true  = "true";
         const char* text_false = "false";
         //
         OpcodeArgBase(const char* n, OpcodeArgValueFactory f, bool io = false) : name(n), factory(f), is_out_variable(io) {};
         OpcodeArgBase(const char* n, OpcodeArgValueFactory f, const char* tt, const char* tf) : name(n), factory(f), text_true(tt), text_false(tf) {}
   };
   //
   class OpcodeArgValueBaseEnum : public OpcodeArgValue {
      public:
         OpcodeArgValueBaseEnum(const SmartEnum& base, uint32_t offset = 0) : 
            baseEnum(base),
            baseOffset(offset)
         {}
         //
         const SmartEnum& baseEnum;
         uint32_t baseOffset = 0;
         //
         uint32_t value = 0; // loaded value
         //
         virtual bool read(cobb::bitstream& stream) noexcept override {
            this->value = stream.read_bits(this->baseEnum.index_bits());
            return true;
         }
         virtual void to_string(std::string& out) const noexcept override {
            this->baseEnum.to_string(out, this->value);
         }
   };
   class OpcodeArgValueBaseFlags : public OpcodeArgValue {
      public:
         OpcodeArgValueBaseFlags(const SmartFlags& base) : base(base) {}
         //
         const SmartFlags& base;
         uint32_t value = 0; // loaded value
         //
         virtual bool read(cobb::bitstream& stream) noexcept override {
            this->value = stream.read_bits(this->base.bits());
            return true;
         }
         virtual void to_string(std::string& out) const noexcept override {
            this->base.to_string(out, this->value);
         }
   };

   enum class index_quirk {
      none,
      presence, // index value is preceded by an "is none" bit
      reference,
      offset,
      word,
   };
   class OpcodeArgValueBaseIndex : public OpcodeArgValue {
      public:
         static constexpr int32_t none = -1;
      public:
         OpcodeArgValueBaseIndex(const char* name, uint32_t max, index_quirk quirk = index_quirk::none) : 
            name(name), max(max), quirk(quirk)
         {};

         const char* name;
         uint32_t    max;
         index_quirk quirk;
         int32_t     value = 0; // loaded value
         //
         virtual bool read(cobb::bitstream& stream) noexcept override {
            if (this->quirk == index_quirk::presence) {
               bool absence = stream.read_bits(1) != 0;
               if (absence) {
                  this->value = OpcodeArgValueBaseIndex::none;
                  return true;
               }
            }
            this->value = stream.read_bits(cobb::bitcount(this->max - 1));
            if (this->quirk == index_quirk::offset)
               --this->value;
            return true;
         }
         virtual void to_string(std::string& out) const noexcept override {
            if (this->value == OpcodeArgValueBaseIndex::none) {
               cobb::sprintf(out, "No %s", this->name);
               return;
            }
            cobb::sprintf(out, "%s #%d", this->name, this->value);
         }
   };

   class OpcodeArgValueNone : public OpcodeArgValue { // one of the possibilities for team-or-player-vars.
      public:
         OpcodeArgValueNone() {};
         //
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueNone();
         }
         virtual bool read(cobb::bitstream& stream) noexcept override {
            return true;
         }
         virtual void to_string(std::string& out) const noexcept override {
            out = "no value";
         }

   };
};