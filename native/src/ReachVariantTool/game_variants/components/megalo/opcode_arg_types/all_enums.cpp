#include "all_enums.h"
#include "../enums.h"

namespace {
   namespace _MegaloArgValueEnumBase {
      megalo_define_smart_enum(AddWeapon,
         "add",
         "unk_1",
         "swap"
      );
      megalo_define_smart_enum(CHUDDestination,
         "unk_0",
         "unk_1"
      );
      megalo_define_smart_enum(CompareOperator,
         "less than",
         "greater than",
         "equal to",
         "less than or equal to",
         "greater than or equal to",
         "not equal to",
      );
      megalo_define_smart_enum(DropWeapon,
         "primary",
         "secondary"
      );
      megalo_define_smart_enum(GrenadeType,
         "frag",
         "plasma"
      );
      megalo_define_smart_enum(MathOperator,
         "add",
         "subtract",
         "multiply",
         "divide",
         "set to",
         "modulo by",
         "bitwise AND with",
         "bitwise OR with",
         "bitwise XOR with",
         "set to bitwise NOT of",
         "bitshift left by",
         "bitshift right by",
         "arith? shift left by",
      );
      megalo_define_smart_enum(PickupPriority,
         "unk_0",
         "hold action",
         "automatic",
         "unk_3"
      );
      megalo_define_smart_enum(TeamDisposition,
         "unk_0",
         "unk_1",
         "unk_2"
      );
      megalo_define_smart_enum(TimerRate,
         "none",
         "-10",
         "-25",
         "-50",
         "-75",
         "-100",
         "-125",
         "-150",
         "-175",
         "-200",
         "-300",
         "-400",
         "-500",
         "-1000",
         "+10",
         "+25",
         "+50",
         "+75",
         "+100",
         "+125",
         "+150",
         "+175",
         "+200",
         "+300",
         "+400",
         "+500",
         "+1000",
      );
      megalo_define_smart_enum(WaypointPriority,
         "none",
         "low",
         "high",
         "default",
      );
   }
}
namespace Megalo {
   OpcodeArgValueAddWeaponEnum::OpcodeArgValueAddWeaponEnum()
      : OpcodeArgValueBaseEnum(_MegaloArgValueEnumBase::AddWeapon)
   {}
   //
   OpcodeArgValueCHUDDestinationEnum::OpcodeArgValueCHUDDestinationEnum()
      : OpcodeArgValueBaseEnum(_MegaloArgValueEnumBase::CHUDDestination)
   {}
   //
   OpcodeArgValueCompareOperatorEnum::OpcodeArgValueCompareOperatorEnum()
      : OpcodeArgValueBaseEnum(_MegaloArgValueEnumBase::CompareOperator)
   {}
   //
   OpcodeArgValueDropWeaponEnum::OpcodeArgValueDropWeaponEnum()
      : OpcodeArgValueBaseEnum(_MegaloArgValueEnumBase::DropWeapon)
   {}
   //
   OpcodeArgValueGrenadeTypeEnum::OpcodeArgValueGrenadeTypeEnum()
      : OpcodeArgValueBaseEnum(_MegaloArgValueEnumBase::GrenadeType)
   {}
   //
   OpcodeArgValueMathOperatorEnum::OpcodeArgValueMathOperatorEnum()
      : OpcodeArgValueBaseEnum(_MegaloArgValueEnumBase::MathOperator)
   {}
   //
   OpcodeArgValuePickupPriorityEnum::OpcodeArgValuePickupPriorityEnum()
      : OpcodeArgValueBaseEnum(_MegaloArgValueEnumBase::PickupPriority)
   {}
   //
   OpcodeArgValueTeamDispositionEnum::OpcodeArgValueTeamDispositionEnum()
      : OpcodeArgValueBaseEnum(_MegaloArgValueEnumBase::TeamDisposition)
   {}
   //
   OpcodeArgValueTimerRateEnum::OpcodeArgValueTimerRateEnum()
      : OpcodeArgValueBaseEnum(_MegaloArgValueEnumBase::TimerRate)
   {}
   //
   OpcodeArgValueWaypointPriorityEnum::OpcodeArgValueWaypointPriorityEnum()
      : OpcodeArgValueBaseEnum(_MegaloArgValueEnumBase::WaypointPriority)
   {}
   //
   bool OpcodeArgValueGenericEnum::read(cobb::ibitreader& stream) noexcept {
      auto& list = this->typeinfo.elements;
      this->value = stream.read_bits(cobb::bitcount(list.size() - 1));
      return true;
   }
   void OpcodeArgValueGenericEnum::write(cobb::bitwriter& stream) const noexcept {
      auto& list = this->typeinfo.elements;
      stream.write(this->value, cobb::bitcount(list.size() - 1));
   }
   void OpcodeArgValueGenericEnum::to_string(std::string& out) const noexcept {
      auto& list = this->typeinfo.elements;
      if (this->value < list.size()) {
         out = list[this->value];
         return;
      }
      cobb::sprintf(out, "%u", this->value);
   }
   void OpcodeArgValueGenericEnum::decompile(Decompiler& out, uint64_t flags) noexcept {
      std::string s;
      this->to_string(s);
      out.write(s);
   }
   //
   namespace { // factories
      template<OpcodeArgTypeinfo& ti> OpcodeArgValue* _enumFactory(cobb::ibitreader& stream) {
         return new OpcodeArgValueGenericEnum(ti);
      }
   }
   OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoAddWeapon = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      {
         "add",
         "unk_1",
         "swap"
      },
      &_enumFactory<OpcodeArgValueEnumTypeinfoAddWeapon>
   );
   OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoCHUDDestination = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      {
         "unk_0",
         "unk_1"
      },
      &_enumFactory<OpcodeArgValueEnumTypeinfoCHUDDestination>
   );
   OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoCompareOperator = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      {
         "<",
         ">",
         "==",
         "<=",
         ">=",
         "!=",
      },
      &_enumFactory<OpcodeArgValueEnumTypeinfoCompareOperator>
   );
   OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoDropWeapon = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      {
         "primary",
         "secondary"
      },
      &_enumFactory<OpcodeArgValueEnumTypeinfoDropWeapon>
   );
   OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoGrenadeType = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      {
         "frag_grenades",
         "plasma_grenades"
      },
      &_enumFactory<OpcodeArgValueEnumTypeinfoGrenadeType>
   );
   OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoMathOperator = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      {
         "+=",
         "-=",
         "*=",
         "/=",
         "=",
         "%=",
         "&=",
         "|=",
         "^=",
         "~=",
         "<<=",
         ">>=",
         "<<<=",
      },
      &_enumFactory<OpcodeArgValueEnumTypeinfoMathOperator>
   );
   OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoPickupPriority = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      {
         "unk_0",
         "hold_action",
         "automatic",
         "unk_3"
      },
      &_enumFactory<OpcodeArgValueEnumTypeinfoPickupPriority>
   );
   OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoTeamDisposition = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      {
         "unk_0",
         "unk_1",
         "unk_2"
      },
      &_enumFactory<OpcodeArgValueEnumTypeinfoTeamDisposition>
   );
   OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoTimerRate = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      {
         "none",
         "-10",
         "-25",
         "-50",
         "-75",
         "-100",
         "-125",
         "-150",
         "-175",
         "-200",
         "-300",
         "-400",
         "-500",
         "-1000",
         "10",
         "25",
         "50",
         "75",
         "100",
         "125",
         "150",
         "175",
         "200",
         "300",
         "400",
         "500",
         "1000",
      },
      &_enumFactory<OpcodeArgValueEnumTypeinfoTimerRate>
   );
   OpcodeArgTypeinfo OpcodeArgValueEnumTypeinfoWaypointPriority = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      {
         "none",
         "low",
         "high",
         "default",
      },
      &_enumFactory<OpcodeArgValueEnumTypeinfoWaypointPriority>
   );
}