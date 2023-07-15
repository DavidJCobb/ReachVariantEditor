#pragma once
#include "../opcode_arg.h"
#include "../../../formats/detailed_enum.h"

namespace Megalo {
   class OpcodeArgValueEnumSuperclass : public OpcodeArgValue {
      public:
         struct deprecation {
            std::string name;
            uint32_t    index;
         };
      public:
         const DetailedEnum& base;
         uint32_t value = 0;
         
         OpcodeArgValueEnumSuperclass(const DetailedEnum& b) : base(b) {}
         
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
         virtual arg_compile_result compile(Compiler&, cobb::string_scanner&, uint8_t part) noexcept override;
         virtual void copy(const OpcodeArgValue*) noexcept override;

         virtual const std::vector<deprecation>* get_deprecations() const noexcept {
            return nullptr;
         };
   };

   #define megalo_opcode_arg_value_enum(name) \
      class name : public OpcodeArgValueEnumSuperclass { \
         public: \
            name##(); \
            static OpcodeArgTypeinfo typeinfo; \
            megalo_opcode_arg_value_make_create_override; \
      };

   class OpcodeArgValueCompareOperatorEnum : public OpcodeArgValueEnumSuperclass {
      public:
         OpcodeArgValueCompareOperatorEnum();
         static OpcodeArgTypeinfo typeinfo;
         megalo_opcode_arg_value_make_create_override;
         //
         void invert();
   };
   megalo_opcode_arg_value_enum(OpcodeArgValueAddWeaponEnum);
   megalo_opcode_arg_value_enum(OpcodeArgValueAttachPositionEnum);
   megalo_opcode_arg_value_enum(OpcodeArgValueCHUDDestinationEnum);
   //megalo_opcode_arg_value_enum(OpcodeArgValueCompareOperatorEnum);
   megalo_opcode_arg_value_enum(OpcodeArgValueDropWeaponEnum);
   megalo_opcode_arg_value_enum(OpcodeArgValueGrenadeTypeEnum);
   class OpcodeArgValueMathOperatorEnum : public OpcodeArgValueEnumSuperclass {
      public:
         OpcodeArgValueMathOperatorEnum();
         static OpcodeArgTypeinfo typeinfo;
         megalo_opcode_arg_value_make_create_override;

         virtual bool uses_mcc_exclusive_data() const {
            switch (this->value) {
               case 0b1010: // <<=
               case 0b1011: // >>=
               case 0b1100: // abs=
                  return true;
            }
            return false;
         }
   };
   class OpcodeArgValuePickupPriorityEnum : public OpcodeArgValueEnumSuperclass {
      public:
         OpcodeArgValuePickupPriorityEnum();
         static OpcodeArgTypeinfo typeinfo;
         megalo_opcode_arg_value_make_create_override;
         
         static std::vector<deprecation> deprecations;

         virtual const std::vector<deprecation>* get_deprecations() const noexcept {
            return &deprecations;
         }
   };
   megalo_opcode_arg_value_enum(OpcodeArgValueTeamAllianceStatus); // KSoft calls this "team disposition"
   megalo_opcode_arg_value_enum(OpcodeArgValueWaypointPriorityEnum);
   megalo_opcode_arg_value_enum(OpcodeArgValueWeaponSlotEnum);

   megalo_opcode_arg_value_enum(OpcodeArgValueLoadoutPalette);
}