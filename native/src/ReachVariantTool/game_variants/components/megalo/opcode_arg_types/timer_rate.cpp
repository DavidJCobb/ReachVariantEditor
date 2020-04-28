#include "timer_rate.h"
#include "../compiler/compiler.h"

namespace {
   //
   // The supported timer rates are:
   //
   //  - 0
   //  - All negative supported values
   //  - All positive supported values
   //
   // where the supported negative and positive values differ only by their sign (i.e. it's 
   // the same numbers).
   //
   constexpr uint32_t _percentages[] = {
      0,
      -10,
      -25,
      -50,
      -75,
      -100,
      -125,
      -150,
      -175,
      -200,
      -300,
      -400,
      -500,
      -1000,
      10,
      25,
      50,
      75,
      100,
      125,
      150,
      175,
      200,
      300,
      400,
      500,
      1000
   };
   constexpr int _percentage_count = std::extent<decltype(_percentages)>::value;
}
namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueTimerRate::typeinfo = OpcodeArgTypeinfo(
      "_timer_rate",
      "Timer Rate",
      "The speed at which a timer's value changes, and whether the value increases (positive) or decreases (negative) with the passage of time.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueTimerRate>
   );
   //
   bool OpcodeArgValueTimerRate::read(cobb::ibitreader & stream, GameVariantDataMultiplayer& mp) noexcept {
      this->value = stream.read_bits(bitcount);
      return true;
   }
   void OpcodeArgValueTimerRate::write(cobb::bitwriter & stream) const noexcept {
      stream.write(this->value, bitcount);
   }
   void OpcodeArgValueTimerRate::to_string(std::string& out) const noexcept {
      if (this->value <= _percentage_count) {
         cobb::sprintf(out, "%d%%", _percentages[this->value]);
         return;
      }
      cobb::sprintf(out, "invalid[%d]", this->value);
   }
   void OpcodeArgValueTimerRate::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      std::string temp;
      this->to_string(temp);
      out.write(temp);
   }
   arg_compile_result OpcodeArgValueTimerRate::compile(Compiler& compiler, Script::string_scanner& arg, uint8_t part) noexcept {
      int32_t value;
      if (arg.extract_integer_literal(value)) {
         if (!arg.extract_specific_char('%'))
            compiler.raise_warning("You omitted the \"%\" suffix on the end of the timer rate. This may be considered an error in future versions of the compiler.");
      } else {
         auto word = arg.extract_word();
         if (word.isEmpty())
            return arg_compile_result::failure();
         auto alias = compiler.lookup_absolute_alias(word);
         if (!alias || !alias->is_integer_constant())
            return arg_compile_result::failure();
         value = alias->get_integer_constant();
      }
      //
      for (int i = 0; i < _percentage_count; ++i) {
         if (_percentages[i] == value) {
            this->value = i;
            return arg_compile_result::success();
         }
      }
      return arg_compile_result::failure(QString("Percentage %1 is not one of the supported timer rates.").arg(value));
   }
   void OpcodeArgValueTimerRate::copy(const OpcodeArgValue* other) noexcept {
      auto cast = dynamic_cast<const OpcodeArgValueTimerRate*>(other);
      assert(cast);
      this->value = cast->value;
   }
}