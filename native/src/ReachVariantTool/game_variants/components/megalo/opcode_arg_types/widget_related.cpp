#include "widget_related.h"
#include "../../../errors.h"
#include "../../../types/multiplayer.h"
#include "../compiler/compiler.h"

namespace {
   constexpr int ce_max_index      = Megalo::Limits::max_script_widgets;
   constexpr int ce_index_bitcount = cobb::bitcount(ce_max_index - 1);
   //
   using _index_t = uint8_t;
   static_assert(std::numeric_limits<_index_t>::max() >= ce_max_index, "Use a larger type.");
}
namespace Megalo {
   #pragma region OpcodeArgValueWidget
      OpcodeArgTypeinfo OpcodeArgValueWidget::typeinfo = OpcodeArgTypeinfo(
         "script_widget",
         "HUD Widget",
         "A HUD widget controllable by script.",
         //
         OpcodeArgTypeinfo::flags::none,
         OpcodeArgTypeinfo::default_factory<OpcodeArgValueWidget>,
         OpcodeArgTypeinfo::no_properties,
         Limits::max_script_widgets
      );
      //
      bool OpcodeArgValueWidget::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
         if (stream.read_bits(1) != 0) { // absence bit
            return true;
         }
         _index_t index = stream.read_bits(ce_index_bitcount);
         auto& list = mp.scriptContent.widgets;
         if (index >= list.size()) {
            auto& e = GameEngineVariantLoadError::get();
            e.state    = GameEngineVariantLoadError::load_state::failure;
            e.reason   = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
            e.detail   = GameEngineVariantLoadError::load_failure_detail::bad_opcode_impossible_index;
            e.extra[0] = index;
            e.extra[1] = ce_max_index - 1;
            return false;
         }
         this->value = &list[index];
         return true;
      }
      void OpcodeArgValueWidget::write(cobb::bitwriter& stream) const noexcept {
         if (!this->value) {
            stream.write(1, 1); // absence bit
            return;
         }
         stream.write(0, 1); // absence bit
         stream.write(this->value->index, ce_index_bitcount);
      }
      void OpcodeArgValueWidget::to_string(std::string& out) const noexcept {
         if (!this->value) {
            out = "no HUD widget";
            return;
         }
         HUDWidgetDeclaration* f = this->value;
         cobb::sprintf(out, "HUD widget %u", f->index);
         return;
      }
      void OpcodeArgValueWidget::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
         if (!this->value) {
            out.write("no_widget");
            return;
         }
         std::string temp;
         cobb::sprintf(temp, "script_widget[%u]", this->value->index);
         out.write(temp);
      }
      arg_compile_result OpcodeArgValueWidget::compile(Compiler& compiler, cobb::string_scanner& arg_text, uint8_t part) noexcept {
         auto arg = compiler.arg_to_variable(arg_text);
         if (!arg)
            return arg_compile_result::failure();
         auto result = this->compile(compiler, *arg, part);
         delete arg;
         return result;
      }
      arg_compile_result OpcodeArgValueWidget::compile(Compiler& compiler, Script::VariableReference& arg, uint8_t part) noexcept {
         auto type = arg.get_type();
         if (type != &OpcodeArgValueWidget::typeinfo)
            return arg_compile_result::failure();
         if (!arg.is_statically_indexable_value()) {
            //
            // This should only be possible if we accessed the bare "no_widget" NamespaceMember.
            //
            this->value = nullptr;
            return arg_compile_result::success();
         }
         auto  index = arg.resolved.top_level.index;
         auto& mp    = compiler.get_variant();
         auto& list  = mp.scriptContent.widgets;
         auto  size  = list.size();
         if (index >= size) {
            if (size)
               return arg_compile_result::failure(QString("You specified widget index %1, but only indices up to %2 are defined.").arg(index).arg(size - 1));
            return arg_compile_result::failure(QString("You specified widget index %1, but no widgets are defined.").arg(index));
         }
         this->value = &list[index];
         return arg_compile_result::success();
      }
      void OpcodeArgValueWidget::copy(const OpcodeArgValue* other) noexcept {
         auto cast = dynamic_cast<const OpcodeArgValueWidget*>(other);
         assert(cast);
         this->value = cast->value;
      }
   #pragma endregion
   //
   #pragma region OpcodeArgValueMeterParameters
      namespace enums {
         auto widget_meter_parameters_type = DetailedEnum({ // currently only needed so we can import names via OpcodeArgTypeinfo
            DetailedEnumValue("none"),
            DetailedEnumValue("number"),
            DetailedEnumValue("timer"),
         });
      }
      OpcodeArgTypeinfo OpcodeArgValueMeterParameters::typeinfo = OpcodeArgTypeinfo(
         "_meter_parameters",
         "HUD Widget Meter Parameters",
         "Options for displaying a meter on the HUD.",
         //
         OpcodeArgTypeinfo::flags::none,
         OpcodeArgTypeinfo::default_factory<OpcodeArgValueMeterParameters>,
         enums::widget_meter_parameters_type
      );
      //
      bool OpcodeArgValueMeterParameters::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
         this->type.read(stream);
         switch (this->type) {
            case MeterType::none:
               break;
            case MeterType::timer:
               this->timer.read(stream, mp);
               break;
            case MeterType::number:
               this->numerator.read(stream, mp);
               this->denominator.read(stream, mp);
               break;
            default:
               printf("Widget Meter Parameters had bad type %u.\n", (int)this->type);
               auto& e = GameEngineVariantLoadError::get();
               e.state    = GameEngineVariantLoadError::load_state::failure;
               e.reason   = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
               e.detail   = GameEngineVariantLoadError::load_failure_detail::bad_widget_meter_parameters_type;
               e.extra[0] = (int32_t)this->type;
               return false;
         }
         return true;
      }
      void OpcodeArgValueMeterParameters::write(cobb::bitwriter& stream) const noexcept {
         this->type.write(stream);
         switch (this->type) {
            case MeterType::none:
               break;
            case MeterType::timer:
               this->timer.write(stream);
               break;
            case MeterType::number:
               this->numerator.write(stream);
               this->denominator.write(stream);
               break;
            default:
               assert(false && "Widget Meter Parameters had a bad type.");
         }
      }
      void OpcodeArgValueMeterParameters::to_string(std::string& out) const noexcept {
         std::string temp;
         switch (this->type) {
            case MeterType::none:
               out = "no meter";
               return;
            case MeterType::timer:
               this->timer.to_string(out);
               out = "timer " + out;
               return;
            case MeterType::number:
               this->numerator.to_string(out);
               this->denominator.to_string(temp);
               cobb::sprintf(out, "%s / %s", out.c_str(), temp.c_str());
               return;
         }
      }
      void OpcodeArgValueMeterParameters::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
         switch (this->type) {
            case MeterType::none:
               out.write("none");
               return;
            case MeterType::timer:
               out.write("timer, ");
               this->timer.decompile(out, flags);
               return;
            case MeterType::number:
               out.write("number, ");
               this->numerator.decompile(out, flags);
               out.write(", ");
               this->denominator.decompile(out, flags);
               return;
         }
      }
      arg_compile_result OpcodeArgValueMeterParameters::compile(Compiler& compiler, cobb::string_scanner& arg_text, uint8_t part) noexcept {
         if (part > 0) {
            auto arg = compiler.arg_to_variable(arg_text);
            if (!arg)
               return arg_compile_result::failure("This argument is not a variable.");
            auto result = this->compile(compiler, *arg, part);
            delete arg;
            return result;
         }
         //
         // Get the meter type.
         //
         QString word = arg_text.extract_word();
         if (word.isEmpty())
            return arg_compile_result::failure(true);
         auto alias = compiler.lookup_absolute_alias(word);
         if (alias) {
            if (alias->is_imported_name())
               word = alias->target_imported_name;
            else
               return arg_compile_result::failure(QString("Alias \"%1\" cannot be used here. Only a meter type (or an alias of one) may appear here.").arg(alias->name), true);
         }
         auto value = enums::widget_meter_parameters_type.lookup(word);
         if (value < 0)
            return arg_compile_result::failure(QString("Value \"%1\" is not a recognized shape type.").arg(word), true);
         this->type = (MeterType)value;
         //
         return arg_compile_result::success().set_needs_more(part < this->sub_variable_count());
      }
      arg_compile_result OpcodeArgValueMeterParameters::compile(Compiler& compiler, Script::VariableReference& arg, uint8_t part) noexcept {
         if (part < 1)
            return arg_compile_result::failure();
         auto count = this->sub_variable_count();
         --part;
         if (part > count)
            return arg_compile_result::failure();
         auto result = this->sub_variable(part).compile(compiler, arg, 0);
         result.set_needs_more(part < count - 1);
         return result;
      }
      void OpcodeArgValueMeterParameters::copy(const OpcodeArgValue* other) noexcept {
         auto cast = dynamic_cast<const OpcodeArgValueMeterParameters*>(other);
         assert(cast);
         this->type = cast->type;
         this->timer.copy(&cast->timer);
         this->numerator.copy(&cast->numerator);
         this->denominator.copy(&cast->denominator);
      }
      void OpcodeArgValueMeterParameters::mark_used_variables(Script::variable_usage_set& usage) const noexcept {
         switch (this->type) {
            case MeterType::timer:
               this->timer.mark_used_variables(usage);
               break;
            case MeterType::number:
               this->numerator.mark_used_variables(usage);
               this->denominator.mark_used_variables(usage);
               break;
         }
      }

      Variable& OpcodeArgValueMeterParameters::sub_variable(uint8_t i) noexcept {
         switch (this->type) {
            case MeterType::none:
               break;
            case MeterType::timer:
               if (i == 0)
                  return this->timer;
               break;
            case MeterType::number:
               switch (i) {
                  case 0: return this->numerator;
                  case 1: return this->denominator;
               }
               break;
         }
         return this->numerator;
      }
      uint8_t OpcodeArgValueMeterParameters::sub_variable_count() const noexcept {
         switch (this->type) {
            case MeterType::none:   return 0;
            case MeterType::timer:  return 1;
            case MeterType::number: return 2;
         }
         assert(false && "Bad meter type specified!");
         __assume(0); // tell MSVC that this is unreachable
      }
   #pragma endregion
}