#include "variant_string_id.h"
#include "../compiler/compiler.h"

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
      //
      // As a side note, it's a shame this doesn't allow arbitrary strings. If I'm understanding right, 
      // this could be used to activate "model variants" on arbitrary objects. The Banshee, for example, 
      // has an alternate color scheme keyed to the variant name "heretic".
      //
   }
   
   OpcodeArgTypeinfo OpcodeArgValueVariantStringID::typeinfo = OpcodeArgTypeinfo(
      "_variant_string",
      "Variant String",
      "A small name indicating a variation of an object type, such as an armor permutation for a biped.",
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
      auto    result = compiler.try_get_integer_or_word(arg, value, word, QString("object variant names"), nullptr, max_value);
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