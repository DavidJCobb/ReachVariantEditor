#include "widget_related.h"
#include "../../../types/multiplayer.h"

namespace Megalo {
   #pragma region OpcodeArgValueWidget
      OpcodeArgTypeinfo OpcodeArgValueWidget::typeinfo = OpcodeArgTypeinfo(
         OpcodeArgTypeinfo::typeinfo_type::default,
         0,
         OpcodeArgTypeinfo::default_factory<OpcodeArgValueWidget>
      );
      //
      bool OpcodeArgValueWidget::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
         if (stream.read_bits(1) != 0) { // absence bit
            this->index = index_of_none;
            return true;
         }
         this->index = stream.read_bits(cobb::bitcount(max_index - 1));
         return true;
      }
      void OpcodeArgValueWidget::write(cobb::bitwriter& stream) const noexcept {
         if (!this->value) {
            stream.write(1, 1); // absence bit
            return;
         }
         stream.write(0, 1); // absence bit
         stream.write(this->value->index, cobb::bitcount(max_index - 1));
      }
      void OpcodeArgValueWidget::postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept {
         this->postprocessed = true;
         if (this->index == index_of_none)
            return;
         auto& list = newlyLoaded->scriptContent.widgets;
         if (this->index >= list.size())
            return;
         this->value = &list[this->index];
      }
      void OpcodeArgValueWidget::to_string(std::string& out) const noexcept {
         if (!this->postprocessed) {
            if (this->index == index_of_none) {
               out = "no HUD widget";
               return;
            }
            cobb::sprintf(out, "HUD widget %u", this->index);
            return;
         }
         if (!this->value) {
            if (this->index == index_of_none) {
               out = "no HUD widget";
               return;
            }
            cobb::sprintf(out, "invalid HUD widget %u", this->index);
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
         OpcodeArgTypeinfo::typeinfo_type::default,
         OpcodeArgTypeinfo::flags::can_be_multiple,
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