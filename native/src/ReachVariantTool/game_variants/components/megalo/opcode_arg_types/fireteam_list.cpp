#include "fireteam_list.h"
#include <limits>
#include "../../../errors.h"
#include "../compiler/compiler.h"

namespace {
   constexpr int ce_bitcount  = cobb::bits_in<decltype(Megalo::OpcodeArgValueFireteamList::value)>;
   constexpr int ce_max_index = ce_bitcount - 1;
}
namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueFireteamList::typeinfo = OpcodeArgTypeinfo(
      "_fireteam_list",
      "Fireteam List",
      "A list of fireteam indices.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueFireteamList>
   ).import_names({ "none", "all" });
   //
   bool OpcodeArgValueFireteamList::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      stream.read(this->value);
      return true;
   }
   void OpcodeArgValueFireteamList::write(cobb::bitwriter& stream) const noexcept {
      stream.write(this->value);
   }
   void OpcodeArgValueFireteamList::to_string(std::string& out) const noexcept {
      if (this->value == 0) {
         out = "none";
         return;
      }
      if (this->value == cobb::bitmax(ce_bitcount)) {
         out = "all";
         return;
      }
      out.clear();
      for (uint8_t i = 0; i <= ce_max_index; ++i) {
         if (value & (1 << i)) {
            if (!out.empty())
               out += ", ";
            cobb::sprintf(out, "%s%u", out.c_str(), i);
         }
      }
   }
   void OpcodeArgValueFireteamList::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      std::string temp;
      this->to_string(temp);
      out.write(temp);
   }
   arg_compile_result OpcodeArgValueFireteamList::compile(Compiler& compiler, cobb::string_scanner& arg, uint8_t part) noexcept {
      QString word;
      int32_t index;
      auto    result = compiler.try_get_integer_or_word(arg, index, word, "", &OpcodeArgValueFireteamList::typeinfo, -1);
      if (result.is_failure())
         return result;
      if (!word.isEmpty()) {
         if (part != 0)
            return arg_compile_result::failure();
         if (word.compare("none", Qt::CaseInsensitive) == 0) {
            this->value = 0;
            return arg_compile_result::success();
         }
         if (word.compare("all", Qt::CaseInsensitive) == 0) {
            this->value = std::numeric_limits<decltype(this->value)>::max();
            return arg_compile_result::success();
         }
         return arg_compile_result::failure(); // unrecognized word
      }
      if (index < 0)
         return arg_compile_result::failure("Fireteam indices cannot be negative.").set_more(arg_compile_result::more_t::optional);
      if (index > ce_max_index)
         compiler.raise_warning(QString("Fireteam indices cannot be greater than %2; you specified %1.").arg(index).arg(ce_max_index));
      else
         this->value |= (1 << index);
      return arg_compile_result::success().set_more(arg_compile_result::more_t::optional);
   }
   void OpcodeArgValueFireteamList::copy(const OpcodeArgValue* other) noexcept {
      auto cast = dynamic_cast<const OpcodeArgValueFireteamList*>(other);
      assert(cast);
      this->value = cast->value;
   }
}