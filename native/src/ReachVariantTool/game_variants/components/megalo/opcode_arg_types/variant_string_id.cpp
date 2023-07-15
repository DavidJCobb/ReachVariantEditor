#include "variant_string_id.h"
#include "../compiler/compiler.h"

namespace {
   constexpr int ce_bitcount = cobb::bitcount(Megalo::Limits::max_string_ids - 1);
}
namespace Megalo {
   namespace enums {
      //
      // The variant string list is defined in the MSIT tag in any given map. Variants can appear in 
      // the following places:
      //
      //  - HLMT (Model): Variant
      //  - JMAD (Animation Model): Animation Name
      //
      auto variant_string_id = DetailedEnum({
         DetailedEnumValue("mp_boneyard_a_idle_start",
            DetailedEnumValueInfo::make_description("An animation name for Phantom models.")
         ),
         DetailedEnumValue("mp_boneyard_a_fly_in",
            DetailedEnumValueInfo::make_description("An animation name for Phantom models.")
         ),
         DetailedEnumValue("mp_boneyard_a_idle_mid",
            DetailedEnumValueInfo::make_description("An animation name for Phantom models.")
         ),
         DetailedEnumValue("mp_boneyard_a_fly_out",
            DetailedEnumValueInfo::make_description("An animation name for Phantom models.")
         ),
         DetailedEnumValue("mp_boneyard_b_fly_in",
            DetailedEnumValueInfo::make_description("An animation name for Phantom models.")
         ),
         DetailedEnumValue("mp_boneyard_b_idle_mid",
            DetailedEnumValueInfo::make_description("An animation name for Phantom models.")
         ),
         DetailedEnumValue("mp_boneyard_b_fly_out",
            DetailedEnumValueInfo::make_description("An animation name for Phantom models.")
         ),
         DetailedEnumValue("mp_boneyard_b_idle_start",
            DetailedEnumValueInfo::make_description("An animation name for Phantom models.")
         ),
         DetailedEnumValue("mp_boneyard_a_leave1",
            DetailedEnumValueInfo::make_description("An animation name for Phantom models.")
         ),
         DetailedEnumValue("mp_boneyard_b_leave1",
            DetailedEnumValueInfo::make_description("An animation name for Phantom models.")
         ),
         DetailedEnumValue("mp_boneyard_b_pickup",
            DetailedEnumValueInfo::make_description("An animation name for Phantom models.")
         ),
         DetailedEnumValue("mp_boneyard_b_idle_pickup",
            DetailedEnumValueInfo::make_description("An animation name for Phantom models.")
         ),
         DetailedEnumValue("mp_boneyard_a",
            DetailedEnumValueInfo::make_description("An animation name for Phantom models.")
         ),
         DetailedEnumValue("mp_boneyard_b",
            DetailedEnumValueInfo::make_description("An animation name for Phantom models.")
         ),
         DetailedEnumValue("default"),
         DetailedEnumValue("carter",
            DetailedEnumValueInfo::make_description("A variant name for Spartan bipeds; produces a male Spartan with Noble 1 (Carter)'s armor.")
         ),
         DetailedEnumValue("jun",
            DetailedEnumValueInfo::make_description("A variant name for Spartan bipeds; produces a male Spartan with Noble 3 (Jun)'s armor.")
         ),
         DetailedEnumValue("female",
            DetailedEnumValueInfo::make_description("A variant name for Spartan bipeds; produces a female Spartan with default armor.")
         ),
         DetailedEnumValue("male",
            DetailedEnumValueInfo::make_description("A variant name for Spartan bipeds; produces a male Spartan with default armor.")
         ),
         DetailedEnumValue("emile",
            DetailedEnumValueInfo::make_description("A variant name for Spartan bipeds; produces a male Spartan with Noble 4 (Emile)'s armor.")
         ),
         DetailedEnumValue("player_skull",
            DetailedEnumValueInfo::make_description("A variant name for Spartan bipeds; produces a male Spartan with the Haunted helmet.")
         ),
         DetailedEnumValue("kat",
            DetailedEnumValueInfo::make_description("A variant name for Spartan bipeds; produces a female Spartan with Noble 2 (Kat)'s armor.")
         ),
         DetailedEnumValue("minor",
            DetailedEnumValueInfo::make_description("A variant name for Elite bipeds; produces Elite Minor armor.")
         ),
         DetailedEnumValue("officer",
            DetailedEnumValueInfo::make_description("A variant name for Elite bipeds; produces Elite Officer armor.")
         ),
         DetailedEnumValue("ultra",
            DetailedEnumValueInfo::make_description("A variant name for Elite bipeds; produces Elite Ultra armor.")
         ),
         DetailedEnumValue("space",
            DetailedEnumValueInfo::make_description("A variant name for Elite bipeds; produces Elite Ranger armor.")
         ),
         DetailedEnumValue("spec_ops",
            DetailedEnumValueInfo::make_description("A variant name for Elite bipeds; produces Elite Spec Ops armor.")
         ),
         DetailedEnumValue("general",
            DetailedEnumValueInfo::make_description("A variant name for Elite bipeds; produces Elite General armor.")
         ),
         DetailedEnumValue("zealot",
            DetailedEnumValueInfo::make_description("A variant name for Elite bipeds; produces Elite Zealot armor.")
         ),
         DetailedEnumValue("mp",
            DetailedEnumValueInfo::make_description("A variant name for Elite bipeds; produces a random armor set.")
         ),
         DetailedEnumValue("jetpack",
            DetailedEnumValueInfo::make_description("A variant name for Elite bipeds; produces Elite Ranger armor.")
         ),
         DetailedEnumValue("gauss",
            DetailedEnumValueInfo::make_description("A variant name. When applied to a standard Warthog, produces a Gauss Warthog instead.")
         ),
         DetailedEnumValue("troop",
            DetailedEnumValueInfo::make_description("A variant name. When applied to a standard Warthog, produces a Troop Transport Warthog instead.")
         ),
         DetailedEnumValue("rocket",
            DetailedEnumValueInfo::make_description("A variant name. When applied to a standard Warthog, produces a Rocket Warthog instead.")
         ),
         DetailedEnumValue("fr"), // frag?
         DetailedEnumValue("pl"), // plasma?
         DetailedEnumValue("35_spire_fp"),
         DetailedEnumValue("mp_spire_fp",
            DetailedEnumValueInfo::make_description("An animation name for Pelican models.")
         ),
         DetailedEnumValue("none",
            DetailedEnumValueInfo::make_description("A variant name. When applied to a standard Warthog, produces a Warthog with no turret.")
         ),
         DetailedEnumValue("destroyed"),
         DetailedEnumValue("damaged"),
         DetailedEnumValue("no_sideguns"),
         DetailedEnumValue("grenade"),
         DetailedEnumValue("multiplayer"),
         DetailedEnumValue("no_side_turrets"),
         DetailedEnumValue("no_turrets"),
         DetailedEnumValue("no_chin_gun"),
         DetailedEnumValue("bed_long",
            DetailedEnumValueInfo::make_description("A variant name. When applied to a truck, spawns a truck with an attached truck bed.")
         ),
         DetailedEnumValue("bed_long_container",
            DetailedEnumValueInfo::make_description("A variant name. When applied to a truck, spawns a truck with an attached trailer.")
         ),
         DetailedEnumValue("bed_long_tanker",
            DetailedEnumValueInfo::make_description("A variant name. When applied to a truck, spawns a truck with an attached fuel tank.")
         ),
         DetailedEnumValue("bed_small",
            DetailedEnumValueInfo::make_description("A variant name. When applied to a truck, spawns a truck with a small attached truck bed.")
         ),
         DetailedEnumValue("bed_small_container",
            DetailedEnumValueInfo::make_description("A variant name. When applied to a truck, spawns a truck with a small attached trailer.")
         ),
         DetailedEnumValue("bed_small_tanker",
            DetailedEnumValueInfo::make_description("A variant name. When applied to a truck, spawns a truck with a small attached fuel tank.")
         ),
         DetailedEnumValue("plasma_cannon"),
         DetailedEnumValue("flak_cannon"),
         DetailedEnumValue("auto"),
         DetailedEnumValue("noblade",
            DetailedEnumValueInfo::make_description("A variant name. When applied to an Energy Sword, produces a hilt with no visible blade.")
         ),
      });
   }
   
   OpcodeArgTypeinfo OpcodeArgValueVariantStringID::typeinfo = OpcodeArgTypeinfo(
      "_variant_string",
      "Variant String",
      "A small name indicating a variation of an object type, such as an armor permutation for a biped, or an animation to be applied to a device-machine.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueVariantStringID>,
      enums::variant_string_id
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
         out = "none";
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
   arg_compile_result OpcodeArgValueVariantStringID::compile(Compiler& compiler, cobb::string_scanner& arg, uint8_t part) noexcept {
      if (part > 0)
         return arg_compile_result::failure();
      //
      constexpr int max_value = Megalo::Limits::max_string_ids - 1;
      //
      int32_t value = 0;
      QString word;
      auto    result = compiler.try_get_integer_or_word(arg, value, word, QString("object variant names"), nullptr, max_value, &OpcodeArgValueVariantStringID::typeinfo);
      if (result.is_failure())
         return result;
      if (!word.isEmpty()) {
         if (word.compare("none", Qt::CaseInsensitive) == 0) { // NOTE: "none" is distinct from "default", apparently
            this->value = -1;
            return arg_compile_result::success();
         }
         value = enums::variant_string_id.lookup(word);
         if (value < 0)
            return arg_compile_result::failure(QString("Value \"%1\" is not a recognized object variant name.").arg(word));
      }
      this->value = value;
      return arg_compile_result::success();
   }
   void OpcodeArgValueVariantStringID::copy(const OpcodeArgValue* other) noexcept {
      auto cast = dynamic_cast<const OpcodeArgValueVariantStringID*>(other);
      assert(cast);
      this->value = cast->value;
   }
}