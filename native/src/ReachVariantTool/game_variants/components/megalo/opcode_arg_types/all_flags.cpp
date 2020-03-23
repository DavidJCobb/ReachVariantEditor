#include "all_flags.h"
#include "../enums.h"

namespace {
   namespace _MegaloArgValueFlagsBase {
      megalo_define_smart_flags(CreateObject,
         "never garbage-collect",
         "unk_1",
         "unk_2"
      );
      megalo_define_smart_flags(KillerType,
         "guardians",
         "suicide",
         "kill",
         "betrayal",
         "quit"
      );
      megalo_define_smart_flags(PlayerUnusedMode,
         "unk_0",
         "unk_1",
         "unk_2",
         "unk_3",
      );
   }
}
namespace Megalo {
   namespace flags_masks {
      auto create_object = DetailedFlags({
         DetailedFlagsValue("never_garbage_collect", DetailedFlagsValueInfo::make_friendly_name("never garbage-collect")),
         DetailedFlagsValue("unk_1"),
         DetailedFlagsValue("unk_2"),
      });
      auto killer_type = DetailedFlags({
         DetailedFlagsValue("guardians",
            DetailedFlagsValueInfo::make_friendly_name("guardians"),
            DetailedFlagsValueInfo::make_description("Used when a player is killed by non-fall-damage physics, by a Kill Ball, or by any inexplicable cause.")
         ),
         DetailedFlagsValue("suicide",
            DetailedFlagsValueInfo::make_friendly_name("suicide"),
            DetailedFlagsValueInfo::make_description("Used when a player dies by their own hand (unless an enemy contributed significantly to their death), or is killed by a level boundary.")
         ),
         DetailedFlagsValue("kill",
            DetailedFlagsValueInfo::make_friendly_name("kill"),
            DetailedFlagsValueInfo::make_description("Used when a player is killed primarily by an enemy.")
         ),
         DetailedFlagsValue("betrayal",
            DetailedFlagsValueInfo::make_friendly_name("betrayal"),
            DetailedFlagsValueInfo::make_description("Used when a player is killed primarily by a teammate.")
         ),
         DetailedFlagsValue("quit",
            DetailedFlagsValueInfo::make_friendly_name("quit"),
            DetailedFlagsValueInfo::make_description("Used when a player's biped dies as a result of the player quitting the match.")
         )
      });
      auto player_unused_mode = DetailedFlags({
         DetailedFlagsValue("unk_0"),
         DetailedFlagsValue("unk_1"),
         DetailedFlagsValue("unk_2"),
         DetailedFlagsValue("unk_3"),
      });
   }

   #pragma region OpcodeArgValueFlagsSuperclass member functions
   bool OpcodeArgValueFlagsSuperclass::read(cobb::ibitreader& stream) noexcept {
      this->value = stream.read_bits(this->base.bitcount());
      return true;
   }
   void OpcodeArgValueFlagsSuperclass::write(cobb::bitwriter& stream) const noexcept {
      stream.write(this->value, this->base.bitcount());
   }
   void OpcodeArgValueFlagsSuperclass::to_string(std::string& out) const noexcept {
      auto& base = this->base;
      auto  size = base.size();
      //
      out.clear();
      size_t i     = 0;
      size_t found = 0;
      for (; i < size; i++) {
         if (value & (1 << i)) {
            ++found;
            if (found == 2)
               out = '(' + out;
            if (found > 1)
               out += ", ";
            out += base[i].name; // friendly names are QStrings; can't use that unless and until we retool OpcodeArgValue around them
         }
      }
      if (found > 1)
         out += ')';
   }
   void OpcodeArgValueFlagsSuperclass::decompile(Decompiler& decompiler, uint64_t flags) noexcept {
      auto& base = this->base;
      auto  size = base.size();
      //
      std::string out;
      size_t i     = 0;
      size_t found = 0;
      for (; i < size; i++) {
         if (value & (1 << i)) {
            ++found;
            if (found == 2)
               out = '(' + out;
            if (found > 1)
               out += ", ";
            out += base[i].name;
         }
      }
      if (found > 1)
         out += ')';
      decompiler.write(out);
   }
   #pragma endregion

   OpcodeArgValueCreateObjectFlags::OpcodeArgValueCreateObjectFlags() : OpcodeArgValueFlagsSuperclass(flags_masks::create_object) {}
   OpcodeArgTypeinfo OpcodeArgValueCreateObjectFlags::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueCreateObjectFlags>
   );
   //
   OpcodeArgValueKillerTypeFlags::OpcodeArgValueKillerTypeFlags() : OpcodeArgValueFlagsSuperclass(flags_masks::killer_type) {}
   OpcodeArgTypeinfo OpcodeArgValueKillerTypeFlags::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueKillerTypeFlags>
   );
   //
   OpcodeArgValuePlayerUnusedModeFlags::OpcodeArgValuePlayerUnusedModeFlags() : OpcodeArgValueFlagsSuperclass(flags_masks::player_unused_mode) {}
   OpcodeArgTypeinfo OpcodeArgValuePlayerUnusedModeFlags::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValuePlayerUnusedModeFlags>
   );
}