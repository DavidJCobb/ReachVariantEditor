#include "object_type.h"
#include "../../../data/object_types.h"

namespace {
   constexpr int ce_bitcount = cobb::bitcount(Megalo::Limits::max_object_types - 1);
}
namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueObjectType::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueObjectType>
   );

   bool OpcodeArgValueObjectType::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      bool absence = stream.read_bits(1);
      if (!absence)
         this->value = stream.read_bits(ce_bitcount);
      return true;
   }
   void OpcodeArgValueObjectType::write(cobb::bitwriter& stream) const noexcept {
      if (this->value < 0) {
         stream.write(1, 1);
         return;
      }
      stream.write(0, 1);
      stream.write(this->value, ce_bitcount);
   }
   void OpcodeArgValueObjectType::to_string(std::string& out) const noexcept {
      if (this->value < 0) {
         out = "no object type";
         return;
      }
      auto item = enums::object_type.item(this->value);
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
   void OpcodeArgValueObjectType::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      if (this->value < 0) {
         out.write("none");
         return;
      }
      auto item = enums::object_type.item(this->value);
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