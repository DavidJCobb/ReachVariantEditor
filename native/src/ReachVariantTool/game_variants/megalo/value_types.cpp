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
      }
   }
};

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