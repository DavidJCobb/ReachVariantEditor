#include "variant_string_id.h"

namespace {
   constexpr int ce_bitcount = cobb::bitcount(Megalo::Limits::max_string_ids - 1);
}
namespace Megalo {
   namespace enums {
      auto variant_string_id = DetailedEnum({
         DetailedEnumValue("mp_boneyard_a_idle_start"),
         DetailedEnumValue("mp_boneyard_a_fly_in"),
         DetailedEnumValue("mp_boneyard_a_idle_mid"),
         DetailedEnumValue("mp_boneyard_a_fly_out"),
         DetailedEnumValue("mp_boneyard_b_fly_in"),
         DetailedEnumValue("mp_boneyard_b_idle_mid"),
         DetailedEnumValue("mp_boneyard_b_fly_out"),
         DetailedEnumValue("mp_boneyard_b_idle_start"),
         DetailedEnumValue("mp_boneyard_a_leave1"),
         DetailedEnumValue("mp_boneyard_b_leave1"),
         DetailedEnumValue("mp_boneyard_b_pickup"),
         DetailedEnumValue("mp_boneyard_b_idle_pickup"),
         DetailedEnumValue("mp_boneyard_a"),
         DetailedEnumValue("mp_boneyard_b"),
         DetailedEnumValue("default"),
         DetailedEnumValue("carter"),
         DetailedEnumValue("jun"),
         DetailedEnumValue("female"),
         DetailedEnumValue("male"),
         DetailedEnumValue("emile"),
         DetailedEnumValue("player_skull"),
         DetailedEnumValue("kat"),
         DetailedEnumValue("minor"),
         DetailedEnumValue("officer"),
         DetailedEnumValue("ultra"),
         DetailedEnumValue("space"),
         DetailedEnumValue("spec_ops"),
         DetailedEnumValue("general"),
         DetailedEnumValue("zealot"),
         DetailedEnumValue("mp"),
         DetailedEnumValue("jetpack"),
         DetailedEnumValue("gauss"),
         DetailedEnumValue("troop"),
         DetailedEnumValue("rocket"),
         DetailedEnumValue("fr"), // frag?!
         DetailedEnumValue("pl"), // plasma?!
         DetailedEnumValue("35_spire_fp"),
         DetailedEnumValue("mp_spire_fp"),
      });
   }
   
   OpcodeArgTypeinfo OpcodeArgValueVariantStringID::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueVariantStringID>
   );

   bool OpcodeArgValueVariantStringID::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      this->value = stream.read_bits(ce_bitcount) - 1;
      return true;
   }
   void OpcodeArgValueVariantStringID::write(cobb::bitwriter& stream) const noexcept {
      stream.write(this->value + 1, ce_bitcount);
   }
   void OpcodeArgValueVariantStringID::to_string(std::string& out) const noexcept {
      if (this->value < 0) {
         out = "no sound";
         return;
      }
      auto item = enums::variant_string_id.item(this->value);
      if (!item) {
         cobb::sprintf(out, "invalid value %u", this->value);
         return;
      }
      QString name = item->get_friendly_name();
      if (!name.isEmpty()) {
         out = name.toStdString();
         return;
      }
      out = item->name;
      if (out.empty()) // enum values should never be nameless but just in case
         cobb::sprintf(out, "%u", this->value);
   }
   void OpcodeArgValueVariantStringID::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      if (this->value < 0) {
         out.write("none");
         return;
      }
      auto item = enums::variant_string_id.item(this->value);
      std::string temp;
      if (!item) {
         cobb::sprintf(temp, "%u", this->value);
         out.write(temp);
         return;
      }
      temp = item->name;
      if (temp.empty())
         cobb::sprintf(temp, "%u", this->value);
      out.write(temp);
   }
}