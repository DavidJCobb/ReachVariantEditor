#include "all_flags.h"
#include "../enums.h"

namespace Megalo {
   namespace flags_masks {
      auto create_object = DetailedFlags({
         DetailedFlagsValue("never_garbage_collect", DetailedFlagsValueInfo::make_friendly_name("never garbage-collect")),
         DetailedFlagsValue("suppress_effect"),
         DetailedFlagsValue("absolute_orientation", DetailedFlagsValueInfo::make_friendly_name("use absolute orientation")),
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
         DetailedFlagsValue("alive_weapons"),
         DetailedFlagsValue("alive_equipment"),
         DetailedFlagsValue("alive_vehicles"),
         DetailedFlagsValue("dead_weapons"),
         DetailedFlagsValue("dead_equipment") // TODO: previous versions of KSoft only listed four flags; we should double-check this
      });
   }

   #pragma region OpcodeArgValueFlagsSuperclass member functions
   bool OpcodeArgValueFlagsSuperclass::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
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
            if (found > 1)
               out += " | ";
            out += base[i].name;
         }
      }
      decompiler.write(out);
   }
   #pragma endregion

   OpcodeArgValueCreateObjectFlags::OpcodeArgValueCreateObjectFlags() : OpcodeArgValueFlagsSuperclass(flags_masks::create_object) {}
   OpcodeArgTypeinfo OpcodeArgValueCreateObjectFlags::typeinfo = OpcodeArgTypeinfo(
      "_create_object_flags",
      "Create Object Flags",
      "Options for creating objects.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueCreateObjectFlags>,
      flags_masks::create_object
   );
   //
   OpcodeArgValueKillerTypeFlags::OpcodeArgValueKillerTypeFlags() : OpcodeArgValueFlagsSuperclass(flags_masks::killer_type) {}
   OpcodeArgTypeinfo OpcodeArgValueKillerTypeFlags::typeinfo = OpcodeArgTypeinfo(
      "_killer_type_flags",
      "Killer Type Flags",
      "Overall causes of death for players.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueKillerTypeFlags>,
      flags_masks::killer_type
   );
   //
   OpcodeArgValuePlayerReqPurchaseModes::OpcodeArgValuePlayerReqPurchaseModes() : OpcodeArgValueFlagsSuperclass(flags_masks::player_unused_mode) {}
   OpcodeArgTypeinfo OpcodeArgValuePlayerReqPurchaseModes::typeinfo = OpcodeArgTypeinfo(
      "_player_req_purchase_modes",
      "Player Requisition Purchase Modes",
      "Unknown. Related to the scrapped requisition system.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValuePlayerReqPurchaseModes>,
      flags_masks::player_unused_mode
   );
}