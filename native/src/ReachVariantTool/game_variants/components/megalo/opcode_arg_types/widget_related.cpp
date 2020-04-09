#include "widget_related.h"
#include "../../../errors.h"
#include "../../../types/multiplayer.h"

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
         OpcodeArgTypeinfo::typeinfo_type::default,
         0,
         OpcodeArgTypeinfo::default_factory<OpcodeArgValueWidget>,
         {},
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
   #pragma endregion
   //
   #pragma region OpcodeArgValueMeterParameters
      OpcodeArgTypeinfo OpcodeArgValueMeterParameters::typeinfo = OpcodeArgTypeinfo(
         "_meter_parameters",
         "HUD Widget Meter Parameters",
         "Options for displaying a meter on the HUD.",
         //
         OpcodeArgTypeinfo::typeinfo_type::default,
         0,
         { "none", "timer", "number" },
         OpcodeArgTypeinfo::default_factory<OpcodeArgValueMeterParameters>
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
   #pragma endregion
}