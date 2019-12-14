#include "value_types.h"

namespace {
   using _MST = MegaloScopeType;    // just a shorthand
   using _MVT = MegaloVariableType; // just a shorthand
};
namespace reach {
   namespace megalo {
      namespace complex_values {
         extern std::array<ComplexValueSubtype, 43> c_scalar = {
            ComplexValueSubtype::constant<int16_t>("Int16"),
            ComplexValueSubtype::variable<_MST::player,  _MVT::number>("Player.Number"),
            ComplexValueSubtype::variable<_MST::team,    _MVT::number>("Team.Number"),
            ComplexValueSubtype::variable<_MST::globals, _MVT::number>("Global.Number"),
            ComplexValueSubtype("User-Defined Option", MegaloValueIndexType::option, MegaloValueIndexQuirk::reference),
            ComplexValueSubtype("Spawn Sequence", MegaloValueEnum::object),
            ComplexValueSubtype("Team Score",     MegaloValueEnum::team),
            ComplexValueSubtype("Player Score",   MegaloValueEnum::player),
            ComplexValueSubtype("Unknown 09",     MegaloValueEnum::player),
            ComplexValueSubtype("Player Rating",  MegaloValueEnum::player), // runtime: float converted to int
            ComplexValueSubtype("Player Stat",    MegaloValueEnum::player, MegaloValueIndexType::stat, MegaloValueIndexQuirk::reference),
            ComplexValueSubtype("Team Stat",      MegaloValueEnum::team,   MegaloValueIndexType::stat, MegaloValueIndexQuirk::reference),
            ComplexValueSubtype("Current Round", true), // UInt16 extended to int
            ComplexValueSubtype("Symmetry (Read-Only)", true), // bool
            ComplexValueSubtype("Symmetry", false), // bool
            ComplexValueSubtype("Score To Win", true),
            ComplexValueSubtype("unkF7A6", false), // bool
            ComplexValueSubtype("Teams Enabled", false), // bool
            ComplexValueSubtype("Round Time Limit", true),
            ComplexValueSubtype("Round Limit", true),
            ComplexValueSubtype("MiscOptions.unk0.bit3", false), // bool
            ComplexValueSubtype("Round Victories To Win", true),
            ComplexValueSubtype("Sudden Death Time Limit", true),
            ComplexValueSubtype("Grace Period", true),
            ComplexValueSubtype("Lives Per Round", true),
            ComplexValueSubtype("Team Lives Per Round", true),
            ComplexValueSubtype("Respawn Time", true),
            ComplexValueSubtype("Suicide Penalty", true),
            ComplexValueSubtype("Betrayal Penalty", true),
            ComplexValueSubtype("Respawn Growth", true),
            ComplexValueSubtype("Loadout Camera Time", true),
            ComplexValueSubtype("Respawn Traits Duration", true),
            ComplexValueSubtype("Friendly Fire", true), // bool
            ComplexValueSubtype("Betrayal Booting", true), // bool
            ComplexValueSubtype("Social Flags Bit 2", true), // bool
            ComplexValueSubtype("Social Flags Bit 3", true), // bool
            ComplexValueSubtype("Social Flags Bit 4", true), // bool
            ComplexValueSubtype("Grenades On Map", true), // bool
            ComplexValueSubtype("Indestructible Vehicles", true), // bool
            ComplexValueSubtype("Powerup Duration, Red", true),
            ComplexValueSubtype("Powerup Duration, Blue", true),
            ComplexValueSubtype("Powerup Duration, Yellow", true),
            ComplexValueSubtype("Death Event Damage Type", true), // byte
         };
         extern std::array<ComplexValueSubtype, 4> player = {
            ComplexValueSubtype("Player (Explicit)", MegaloValueEnum::player),
            ComplexValueSubtype::variable<_MST::player, _MVT::player>("Player.Player"),
            ComplexValueSubtype::variable<_MST::object, _MVT::player>("Object.Player"),
            ComplexValueSubtype::variable<_MST::team,   _MVT::player>("Team.Player"),
         };
         extern std::array<ComplexValueSubtype, 8> object = {
            ComplexValueSubtype("Object (Explicit)", MegaloValueEnum::object),
            ComplexValueSubtype::variable<_MST::player, _MVT::object>("Player.Object"),
            ComplexValueSubtype::variable<_MST::object, _MVT::object>("Object.Object"),
            ComplexValueSubtype::variable<_MST::team,   _MVT::object>("Team.Object"),
            ComplexValueSubtype("Player.Slave", MegaloValueEnum::object),
            ComplexValueSubtype::variable<_MST::player, _MVT::player>("Player.Player.Slave"),
            ComplexValueSubtype::variable<_MST::object, _MVT::player>("Object.Player.Slave"),
            ComplexValueSubtype::variable<_MST::team,   _MVT::player>("Team.Player.Slave"),
         };
         extern std::array<ComplexValueSubtype, 6> team = {
            ComplexValueSubtype("Team (Explicit)", MegaloValueEnum::team),
            ComplexValueSubtype::variable<_MST::player, _MVT::team>("Player.Team"),
            ComplexValueSubtype::variable<_MST::object, _MVT::team>("Object.Team"),
            ComplexValueSubtype::variable<_MST::team,   _MVT::team>("Team.Team"),
            ComplexValueSubtype("Player.OwnerTeam", MegaloValueEnum::player),
            ComplexValueSubtype("Object.OwnerTeam", MegaloValueEnum::object),
         };
         extern std::array<ComplexValueSubtype, 7> timer = {
            ComplexValueSubtype::variable<_MST::globals, _MVT::timer>("Global.Timer"),
            ComplexValueSubtype::variable<_MST::player,  _MVT::timer>("Player.Timer"),
            ComplexValueSubtype::variable<_MST::team,    _MVT::timer>("Team.Timer"),
            ComplexValueSubtype::variable<_MST::object,  _MVT::timer>("Object.Timer"),
            ComplexValueSubtype("Round Time"),
            ComplexValueSubtype("Sudden Death Time"),
            ComplexValueSubtype("Grace Period Time"),
         };
      }
   }
};

bool SimpleValue::read(cobb::bitstream& stream) noexcept {
   int  bitcount  = 0;
   bool is_signed = false;
   switch (this->type) {
      case SimpleValueType::integer_8_unsigned:
         bitcount = 8;
         break;
      case SimpleValueType::integer_8_signed:
         bitcount  = 8;
         is_signed = true;
         break;
      case SimpleValueType::integer_16_signed:
         bitcount  = 16;
         is_signed = true;
         break;
   }
   switch (this->type) {
      case SimpleValueType::boolean:
         stream.read(this->value.boolean);
         break;
      case SimpleValueType::enumeration:
         bitcount = reach::megalo::bits_for_enum(this->enumeration);
         this->value.integer_unsigned = stream.read_bits(bitcount) - reach::megalo::offset_for_enum(this->enumeration);
         break;
      case SimpleValueType::flags:
         this->value.integer_unsigned = stream.read_bits(reach::megalo::bits_for_flags(this->flags_mask));
         break;
      case SimpleValueType::integer_8_signed:
      case SimpleValueType::integer_8_unsigned:
      case SimpleValueType::integer_16_signed:
         this->value.integer_signed = stream.read_bits(bitcount, is_signed ? cobb::bitstream_read_flags::is_signed : 0);
         break;
      case SimpleValueType::vector3:
         stream.read(this->value.vector3.x);
         stream.read(this->value.vector3.y);
         stream.read(this->value.vector3.z);
         break;
   }
}

namespace {
   template<int N> ComplexValueSubtype* _getComplexSubtype(ComplexValueType type, const std::array<ComplexValueSubtype, N>& list) {
      int index = (int)type;
      if (index < list.size())
         return list[index];
      return nullptr;
   }
}
void ComplexValue::read(cobb::bitstream& stream) noexcept {
   ComplexValueType type = this->type;
   if (type == ComplexValueType::any)
      type = (ComplexValueType)stream.read_bits(3);
   switch (type) {
      case ComplexValueType::scalar:
         this->subtype = _getComplexSubtype(type, reach::megalo::complex_values::scalar);
         break;
      case ComplexValueType::player:
         this->subtype = _getComplexSubtype(type, reach::megalo::complex_values::player);
         break;
      case ComplexValueType::object:
         this->subtype = _getComplexSubtype(type, reach::megalo::complex_values::object);
         break;
      case ComplexValueType::team:
         this->subtype = _getComplexSubtype(type, reach::megalo::complex_values::team);
         break;
      case ComplexValueType::timer:
         this->subtype = _getComplexSubtype(type, reach::megalo::complex_values::timer);
         break;
   }
   if (!this->subtype) {
      printf("Failed to identify subtype for complex type %d.", (int)type);
      return;
   }
   ComplexValueSubtype& st = *this->subtype;
   if (st.enumeration != MegaloValueEnum::not_applicable) {
      this->enum_value = stream.read_bits(reach::megalo::bits_for_enum(st.enumeration)) - reach::megalo::offset_for_enum(st.enumeration);
   }
   if (st.constant_flags & ComplexValueSubtype::flags::has_constant) {
      this->constant = stream.read_bits(st.constant_bitlength);
   } else if (st.constant_flags & ComplexValueSubtype::flags::has_index) {
      if (st.index_quirk == MegaloValueIndexQuirk::presence) {
         bool presence = stream.read_bits<uint8_t>(1) != 0;
         if (!presence)
            return;
      }
      int bitlength = st.index_bitlength;
      if (!bitlength)
         bitlength = reach::megalo::bits_for_index_type(st.index_type);
      if (st.index_quirk == MegaloValueIndexQuirk::word) {
         this->index = stream.read_bits<uint32_t>(bitlength, cobb::bitstream_read_flags::is_signed);
      } else {
         this->index = stream.read_bits<uint32_t>(bitlength);
         if (st.index_quirk == MegaloValueIndexQuirk::offset)
            this->index--;
      }
   }
}

void MegaloValue::read(cobb::bitstream& stream) noexcept {
   assert(this->type && "Cannot read a value that hasn't had its type set!");
   switch (this->type->underlying_type) {
      case MegaloValueUnderlyingType::boolean:
         this->value.boolean = stream.read_bits(1) != 0;
         break;
      default:
         assert(false && "Type not implemented!");
   }
}